/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_consolerunoperationfrommemento.h"
#include "gt_executableoperation.h"
#include "gt_executioneventfilewriter.h"
#include "gt_executioneventstream.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "gt_projectruntime.h"
#include "gt_stdioexecutioneventencoder.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>

namespace
{
    struct Options
    {
        QCommandLineOption operation{QStringList{"operation-memento"},
                                     QObject::tr("Operation Memento XML"),
                                     "path"};
        QCommandLineOption data{
            QStringList{"data-memento"},
            QObject::tr("Optional detached data Memento XML"), "path"};
        QCommandLineOption project{
            QStringList{"project-memento"},
            QObject::tr("Optional project-data Memento XML"), "path"};
        QCommandLineOption events{
            QStringList{"events"},
            QObject::tr("Optional NDJSON execution event file"), "path"};
        QCommandLineOption workingDirectory{
            QStringList{"working-directory"},
            QObject::tr("Execution working directory"), "path"};
        QList<QCommandLineOption> all() const
        {
            return {operation, data, project, events, workingDirectory};
        }
    };


    int fail(GtStdioExecutionEventEncoder& encoder, QString code,
             QString message, int rc)
    {
        encoder.encodeFailure(std::move(code), std::move(message));
        return rc;
    }

    struct CommandFailure
    {
        int exitCode{0};
        QString code;
        QString message;
    };

    struct ExecutionPaths
    {
        QString operation;
        QString data;
        QString project;
        QString events;
        QString workingDirectory;
    };

    struct RestoredInputs
    {
        std::unique_ptr<GtExecutableOperation> operation;
        std::unique_ptr<GtObject> data;
        std::unique_ptr<GtObjectGroup> projectData;
    };

    QString absolutePath(QString const& path)
    {
        return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    }

    ExecutionPaths executionPaths(QCommandLineParser const& parser,
                                  Options const& options)
    {
        ExecutionPaths paths;
        paths.operation = absolutePath(parser.value(options.operation));
        paths.data = parser.isSet(options.data)
                         ? absolutePath(parser.value(options.data))
                         : QString();
        paths.project = parser.isSet(options.project)
                            ? absolutePath(parser.value(options.project))
                            : QString();
        paths.events = parser.isSet(options.events)
                           ? absolutePath(parser.value(options.events))
                           : QString();
        paths.workingDirectory =
            parser.isSet(options.workingDirectory)
                ? absolutePath(parser.value(options.workingDirectory))
                : (paths.project.isEmpty()
                       ? QDir::currentPath()
                       : QFileInfo(paths.project).absolutePath());
        return paths;
    }

    std::optional<CommandFailure> validatePaths(ExecutionPaths const& paths)
    {
        const QStringList inputPaths{paths.operation, paths.data,
                                     paths.project};
        const bool overwritesInput =
            std::any_of(inputPaths.cbegin(), inputPaths.cend(),
                        [&paths](QString const& input) {
                            return !input.isEmpty() && paths.events == input;
                        });
        if (overwritesInput)
        {
            return CommandFailure{
                2, QStringLiteral("invalid_arguments"),
                QStringLiteral(
                    "Event output must not overwrite an input Memento")};
        }

        if (!QFileInfo(paths.operation).isFile() ||
            (!paths.data.isEmpty() && !QFileInfo(paths.data).isFile()) ||
            (!paths.project.isEmpty() && !QFileInfo(paths.project).isFile()) ||
            !QFileInfo(paths.workingDirectory).isDir())
        {
            return CommandFailure{
                3, QStringLiteral("invalid_input"),
                QStringLiteral(
                    "An input Memento or working directory does not exist")};
        }
        return std::nullopt;
    }

    std::unique_ptr<GtObject> restoreObject(QString const& path)
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return {};

        GtObjectMemento memento(file.readAll());
        if (memento.isNull()) return {};
        return memento.toObject(*gtObjectFactory);
    }

    std::optional<CommandFailure> restoreInputs(ExecutionPaths const& paths,
                                                RestoredInputs& inputs)
    {
        auto operationObject = restoreObject(paths.operation);
        auto* operation =
            qobject_cast<GtExecutableOperation*>(operationObject.get());
        if (!operation)
        {
            return CommandFailure{
                4, QStringLiteral("invalid_operation"),
                QStringLiteral("Operation Memento could not be restored as an "
                               "executable operation")};
        }
        auto* ownedOperation =
            static_cast<GtExecutableOperation*>(operationObject.release());
        inputs.operation.reset(ownedOperation);

        if (!paths.data.isEmpty())
        {
            inputs.data = restoreObject(paths.data);
            if (!inputs.data)
            {
                return CommandFailure{
                    4, QStringLiteral("invalid_data_memento"),
                    QStringLiteral(
                        "Detached data Memento could not be restored")};
            }
        }

        if (!paths.project.isEmpty())
        {
            auto projectObject = restoreObject(paths.project);
            auto* projectData =
                qobject_cast<GtObjectGroup*>(projectObject.get());
            if (!projectData)
            {
                return CommandFailure{
                    4, QStringLiteral("invalid_project_memento"),
                    QStringLiteral(
                        "Project Memento is not project-data format")};
            }
            auto* ownedProjectData =
                static_cast<GtObjectGroup*>(projectObject.release());
            inputs.projectData.reset(ownedProjectData);
        }
        return std::nullopt;
    }

    class WorkingDirectoryGuard
    {
    public:
        explicit WorkingDirectoryGuard(QString const& path) :
            m_previous(QDir::currentPath())
        {
            m_changed = QDir::setCurrent(path);
        }

        ~WorkingDirectoryGuard()
        {
            if (m_changed) QDir::setCurrent(m_previous);
        }

        bool changed() const
        {
            return m_changed;
        }

    private:
        QString m_previous;
        bool m_changed{false};
    };

    int executeOperation(GtStdioExecutionEventEncoder& terminal,
                         GtExecutionId const& executionId,
                         ExecutionPaths const& paths, RestoredInputs inputs)
    {
        WorkingDirectoryGuard workingDirectory(paths.workingDirectory);
        if (!workingDirectory.changed())
        {
            return fail(
                terminal, QStringLiteral("working_directory_failed"),
                QStringLiteral("Could not select the working directory"), 3);
        }

        GtProjectRuntime runtime;
        auto initialized = runtime.initialize();
        if (!initialized.succeeded())
        {
            return fail(terminal, QStringLiteral("runtime_failed"),
                        initialized.message, 5);
        }
        if (inputs.projectData)
        {
            auto opened = runtime.openProjectFromMemento(
                std::move(inputs.projectData), paths.workingDirectory);
            if (!opened.succeeded())
            {
                return fail(terminal, QStringLiteral("project_failed"),
                            opened.message, 5);
            }
        }

        GtExecutionEventStream stream(executionId);
        std::unique_ptr<GtExecutionEventFileWriter> writer;
        if (!paths.events.isEmpty())
        {
            writer = std::make_unique<GtExecutionEventFileWriter>(paths.events);
            if (!writer->isOpen())
            {
                return fail(terminal, QStringLiteral("event_output_failed"),
                            writer->errorString(), 6);
            }
            QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                             writer.get(),
                             &GtExecutionEventFileWriter::writeEvent);
        }

        auto result = runtime.executeOperation(std::move(inputs.operation),
                                               std::move(inputs.data), stream);
        if (writer && writer->hasError())
        {
            return fail(terminal, QStringLiteral("event_output_failed"),
                        writer->errorString(), 6);
        }
        if (!result.succeeded())
        {
            return fail(terminal, QStringLiteral("execution_failed"),
                        result.message, 5);
        }
        return terminal.encodeResult(result.result.get()) ? 0 : 6;
    }
} // namespace

QList<GtCommandLineOption>
gt::console::runOperationFromMementoOptions()
{
    Options options;
    const auto commandLineOptions = options.all();
    QList<GtCommandLineOption> result;
    result.reserve(commandLineOptions.size());
    std::transform(
        commandLineOptions.cbegin(), commandLineOptions.cend(),
        std::back_inserter(result), [](auto const& option) {
            return GtCommandLineOption{option.names(), option.description()};
        });
    return result;
}

int
gt::console::runOperationFromMemento(QStringList const& args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QObject::tr("Execute an operation from Mementos"));
    parser.addHelpOption();
    const GtExecutionId executionId;
    Options options;
    parser.addOptions(options.all());

    QStringList argv{"run_operation_from_memento"};
    argv.append(args);
    if (!parser.parse(argv))
    {
        GtStdioExecutionEventEncoder encoder(executionId, std::cout);
        return fail(encoder, QStringLiteral("invalid_arguments"),
                    parser.errorText(), 2);
    }
    if (parser.isSet("help")) parser.showHelp(0);

    GtStdioExecutionEventEncoder terminal(executionId, std::cout);
    if (!parser.isSet(options.operation) ||
        parser.value(options.operation).isEmpty() ||
        !parser.positionalArguments().isEmpty())
    {
        return fail(terminal, QStringLiteral("invalid_arguments"),
                    QStringLiteral("--operation-memento is required; "
                                   "positional arguments are not accepted"),
                    2);
    }

    const ExecutionPaths paths = executionPaths(parser, options);
    if (auto failure = validatePaths(paths))
    {
        return fail(terminal, failure->code, failure->message,
                    failure->exitCode);
    }

    RestoredInputs inputs;
    if (auto failure = restoreInputs(paths, inputs))
    {
        return fail(terminal, failure->code, failure->message,
                    failure->exitCode);
    }

    return executeOperation(terminal, executionId, paths, std::move(inputs));
}
