/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_consolerunoperationfrommemento.h"

#include "gt_executionenvironment.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "gt_package.h"
#include "gt_project.h"
#include "gt_qtutilities.h"
#include "operations/gt_executioneventfilewriter.h"
#include "operations/gt_executioneventstream.h"
#include "operations/gt_executableoperation.h"
#include "operations/gt_stdioexecutioneventencoder.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>

namespace
{

    constexpr int invalidArgumentsExitCode = 2;
    constexpr int inputFileExitCode = 3;
    constexpr int mementoExitCode = 4;
    constexpr int executionExitCode = 5;
    constexpr int outputFileExitCode = 6;
    constexpr int protocolOutputExitCode = 7;

    class MementoExecutionProject final : public GtProject
    {
    public:
        explicit MementoExecutionProject(QString const& path) : GtProject(path)
        {
        }
    };

    class CurrentDirectoryGuard final
    {
    public:
        explicit CurrentDirectoryGuard(QString const& path) :
            m_previousDirectory(QDir::currentPath()),
            m_changed(QDir::setCurrent(path))
        {
        }

        CurrentDirectoryGuard(CurrentDirectoryGuard const&) = delete;
        CurrentDirectoryGuard& operator=(CurrentDirectoryGuard const&) = delete;

        ~CurrentDirectoryGuard()
        {
            if (m_changed && !QDir::setCurrent(m_previousDirectory))
            {
                gtWarning()
                    << QObject::tr("Cannot restore working directory: %1")
                           .arg(m_previousDirectory);
            }
        }

        bool isValid() const noexcept
        {
            return m_changed;
        }

    private:
        QString m_previousDirectory;
        bool m_changed;
    };

    struct OperationExecutionOptions
    {
        const QCommandLineOption operation{
            {QStringLiteral("operation-memento")},
            QObject::tr("Serialized executable-operation Memento path"),
            QObject::tr("path")};
        const QCommandLineOption data{
            {QStringLiteral("data-memento")},
            QObject::tr("Optional detached-data Memento path"),
            QObject::tr("path")};
        const QCommandLineOption project{
            {QStringLiteral("project-memento")},
            QObject::tr("Optional project-data Memento path"),
            QObject::tr("path")};
        const QCommandLineOption events{
            {QStringLiteral("events")},
            QObject::tr("Optional execution-event NDJSON path"),
            QObject::tr("path")};
        const QCommandLineOption workingDirectory{
            {QStringLiteral("working-directory")},
            QObject::tr("Optional execution working directory"),
            QObject::tr("path")};

        QList<QCommandLineOption> list() const
        {
            return {operation, data, project, events, workingDirectory};
        }
    };

    QString normalizedFilePath(QString const& filePath)
    {
        return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
    }

    struct OperationExecutionPaths
    {
        QString operationMemento;
        QString dataMemento;
        QString projectMemento;
        QString events;
        QString workingDirectory;

        static OperationExecutionPaths fromParser(
            QCommandLineParser const& parser,
            OperationExecutionOptions const& options)
        {
            OperationExecutionPaths paths;
            paths.operationMemento =
                normalizedFilePath(parser.value(options.operation));
            if (parser.isSet(options.data))
            {
                paths.dataMemento =
                    normalizedFilePath(parser.value(options.data));
            }
            if (parser.isSet(options.project))
            {
                paths.projectMemento =
                    normalizedFilePath(parser.value(options.project));
            }
            if (parser.isSet(options.events))
            {
                paths.events = normalizedFilePath(parser.value(options.events));
            }

            if (parser.isSet(options.workingDirectory))
            {
                paths.workingDirectory =
                    normalizedFilePath(parser.value(options.workingDirectory));
            }
            else if (!paths.projectMemento.isEmpty())
            {
                paths.workingDirectory =
                    QFileInfo(paths.projectMemento).absolutePath();
            }
            else
            {
                paths.workingDirectory = QDir::currentPath();
            }

            return paths;
        }

        bool sanityCheck(QString& error) const
        {
            const std::array inputs{operationMemento, dataMemento,
                                    projectMemento};
            if (!events.isEmpty() && std::find(inputs.cbegin(), inputs.cend(),
                                               events) != inputs.cend())
            {
                error = QObject::tr(
                    "The event output must not overwrite an input Memento.");
                return false;
            }

            if (workingDirectory.isEmpty())
            {
                error = QObject::tr("The working directory is empty.");
                return false;
            }

            return true;
        }

        bool checkInputFiles(QString& error) const
        {
            const std::array requiredInputs{operationMemento};
            const auto missingRequiredInput = std::find_if(
                requiredInputs.cbegin(), requiredInputs.cend(),
                [](QString const& path) { return !QFileInfo(path).isFile(); });
            if (missingRequiredInput != requiredInputs.cend())
            {
                error = QObject::tr("Input Memento does not exist: %1")
                            .arg(*missingRequiredInput);
                return false;
            }

            const std::array optionalInputs{dataMemento, projectMemento};
            const auto missingOptionalInput = std::find_if(
                optionalInputs.cbegin(), optionalInputs.cend(),
                [](QString const& path) {
                    return !path.isEmpty() && !QFileInfo(path).isFile();
                });
            if (missingOptionalInput != optionalInputs.cend())
            {
                error = QObject::tr("Input Memento does not exist: %1")
                            .arg(*missingOptionalInput);
                return false;
            }

            if (!QFileInfo(workingDirectory).isDir())
            {
                error = QObject::tr("Working directory does not exist: %1")
                            .arg(workingDirectory);
                return false;
            }

            return true;
        }
    };

    int writeFailure(GtStdioExecutionEventEncoder& encoder, int exitCode,
                     QString errorCode, QString message)
    {
        if (message.isEmpty())
        {
            message = QObject::tr("Operation execution failed.");
        }

        gtError() << message;
        if (!encoder.encodeFailure(std::move(errorCode), message))
        {
            gtError() << QObject::tr(
                "Cannot write the terminal operation failure record.");
            return protocolOutputExitCode;
        }

        return exitCode;
    }

    std::unique_ptr<GtObject> restoreObject(QString const& fileName,
                                            QString const& objectDescription,
                                            QString& error)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            error = QObject::tr("Cannot read %1 Memento '%2': %3")
                        .arg(objectDescription, fileName, file.errorString());
            return {};
        }

        GtObjectMemento memento(file.readAll());
        if (memento.isNull())
        {
            error = QObject::tr("Invalid %1 Memento '%2'")
                        .arg(objectDescription, fileName);
            return {};
        }

        if (!gtObjectFactory->knownClass(memento.className()))
        {
            error = QObject::tr("Unsupported %1 Memento '%2'")
                        .arg(objectDescription, fileName);
            return {};
        }

        GtObject* object = memento.restore<GtObject*>(gtObjectFactory);
        if (!object)
        {
            error = QObject::tr("Unsupported %1 Memento '%2'")
                        .arg(objectDescription, fileName);
            return {};
        }

        return std::unique_ptr<GtObject>(object);
    }

    std::unique_ptr<GtObjectGroup> restoreProjectData(QString const& fileName,
                                                      QString& error)
    {
        auto object = restoreObject(fileName, QObject::tr("project"), error);
        if (!object)
        {
            return {};
        }

        auto group = gt::unique_qobject_cast<GtObjectGroup>(std::move(object));
        if (!group)
        {
            error = QObject::tr(
                        "Project Memento '%1' is not a supported object group")
                        .arg(fileName);
            return {};
        }

        return group;
    }

    bool populateProject(GtObjectGroup& projectData, GtProject& project,
                         QString& error)
    {
        const auto objects = projectData.findDirectChildren<GtObject*>();
        for (GtObject* object : objects)
        {
            if (!qobject_cast<GtPackage*>(object))
            {
                error = QObject::tr(
                            "Project Memento contains unsupported root object "
                            "'%1' (%2)")
                            .arg(object->objectName(),
                                 object->metaObject()->className());
                return false;
            }
        }

        for (GtObject* object : objects)
        {
            object->disconnectFromParent();
            if (!project.appendChild(object))
            {
                error = QObject::tr("Cannot add '%1' to execution project")
                            .arg(object->objectName());
                delete object;
                return false;
            }
        }

        project.acceptChangesRecursively();
        return true;
    }

    QString executionErrorCode(GtExecutionResult::Error error)
    {
        switch (error)
        {
        case GtExecutionResult::Error::ProjectRequired:
            return QStringLiteral("project_required");
        case GtExecutionResult::Error::WrongThread:
            return QStringLiteral("wrong_thread");
        case GtExecutionResult::Error::UnhandledException:
            return QStringLiteral("unhandled_exception");
        case GtExecutionResult::Error::None:
            break;
        }

        return QStringLiteral("execution_error");
    }

    int parseExecutionPaths(QStringList const& args,
                            OperationExecutionOptions const& options,
                            GtStdioExecutionEventEncoder& encoder,
                            OperationExecutionPaths& paths)
    {
        QCommandLineParser parser;
        parser.setApplicationDescription(
            QObject::tr("Execute an executable operation from Mementos"));
        parser.addHelpOption();
        parser.addOptions(options.list());

        QStringList commandLine{QStringLiteral("run_operation_from_memento")};
        commandLine.append(args);
        if (!parser.parse(commandLine))
        {
            return writeFailure(encoder, invalidArgumentsExitCode,
                                QStringLiteral("invalid_arguments"),
                                parser.errorText());
        }
        if (parser.isSet(QStringLiteral("help")))
        {
            parser.showHelp(0);
        }

        if (!parser.isSet(options.operation) ||
            parser.value(options.operation).isEmpty())
        {
            return writeFailure(
                encoder, invalidArgumentsExitCode,
                QStringLiteral("invalid_arguments"),
                QObject::tr("Missing required option --operation-memento"));
        }
        if ((parser.isSet(options.data) &&
             parser.value(options.data).isEmpty()) ||
            (parser.isSet(options.project) &&
             parser.value(options.project).isEmpty()) ||
            (parser.isSet(options.events) &&
             parser.value(options.events).isEmpty()) ||
            (parser.isSet(options.workingDirectory) &&
             parser.value(options.workingDirectory).isEmpty()))
        {
            return writeFailure(
                encoder, invalidArgumentsExitCode,
                QStringLiteral("invalid_arguments"),
                QObject::tr("Command options must have non-empty values."));
        }
        if (!parser.positionalArguments().isEmpty())
        {
            return writeFailure(
                encoder, invalidArgumentsExitCode,
                QStringLiteral("invalid_arguments"),
                QObject::tr("Unexpected positional arguments: %1")
                    .arg(parser.positionalArguments().join(' ')));
        }

        paths = OperationExecutionPaths::fromParser(parser, options);
        QString error;
        if (!paths.sanityCheck(error))
        {
            return writeFailure(encoder, invalidArgumentsExitCode,
                                QStringLiteral("invalid_arguments"), error);
        }
        if (!paths.checkInputFiles(error))
        {
            return writeFailure(encoder, inputFileExitCode,
                                QStringLiteral("input_file_error"), error);
        }

        return 0;
    }

    int restoreExecutableOperation(
        QString const& fileName, GtStdioExecutionEventEncoder& encoder,
        std::unique_ptr<GtExecutableOperation>& operation)
    {
        QString error;
        auto object = restoreObject(fileName, QObject::tr("operation"), error);
        if (!object)
        {
            return writeFailure(encoder, mementoExitCode,
                                QStringLiteral("invalid_operation_memento"),
                                error);
        }

        auto executableOperation =
            gt::unique_qobject_cast<GtExecutableOperation>(std::move(object));
        if (!executableOperation)
        {
            return writeFailure(
                encoder, mementoExitCode,
                QStringLiteral("not_an_executable_operation"),
                QObject::tr("Operation Memento does not describe a "
                            "GtExecutableOperation."));
        }

        operation = std::move(executableOperation);
        return 0;
    }

    int restoreDetachedData(QString const& fileName,
                            GtStdioExecutionEventEncoder& encoder,
                            std::unique_ptr<GtObject>& data)
    {
        if (fileName.isEmpty())
        {
            return 0;
        }

        QString error;
        data = restoreObject(fileName, QObject::tr("data"), error);
        if (data)
        {
            return 0;
        }

        return writeFailure(encoder, mementoExitCode,
                            QStringLiteral("invalid_data_memento"), error);
    }

    int restoreExecutionProject(
        OperationExecutionPaths const& paths,
        GtStdioExecutionEventEncoder& encoder,
        std::unique_ptr<MementoExecutionProject>& project)
    {
        if (paths.projectMemento.isEmpty())
        {
            return 0;
        }

        QString error;
        auto projectData = restoreProjectData(paths.projectMemento, error);
        if (!projectData)
        {
            return writeFailure(encoder, mementoExitCode,
                                QStringLiteral("invalid_project_memento"),
                                error);
        }

        project =
            std::make_unique<MementoExecutionProject>(paths.workingDirectory);
        project->setObjectName(projectData->objectName());
        project->setUuid(projectData->uuid());
        if (!populateProject(*projectData, *project, error))
        {
            return writeFailure(encoder, mementoExitCode,
                                QStringLiteral("invalid_project_memento"),
                                error);
        }

        return 0;
    }

    struct EventOutput
    {
        std::unique_ptr<GtExecutionEventFileWriter> writer;
    };

    int configureEventOutput(OperationExecutionPaths const& paths,
                             GtExecutionEventStream const& events,
                             GtStdioExecutionEventEncoder& encoder,
                             EventOutput& output)
    {
        if (paths.events.isEmpty())
        {
            return 0;
        }

        output.writer =
            std::make_unique<GtExecutionEventFileWriter>(paths.events);
        if (!output.writer->isOpen())
        {
            return writeFailure(
                encoder, outputFileExitCode,
                QStringLiteral("event_output_error"),
                QObject::tr("Cannot open event output '%1': %2")
                    .arg(paths.events, output.writer->errorString()));
        }

        // The writer synchronizes its own file access, so direct delivery
        // preserves streaming when an operation publishes from a worker thread.
        QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                         output.writer.get(),
                         &GtExecutionEventFileWriter::writeEvent,
                         Qt::DirectConnection);
        return 0;
    }

    std::optional<GtExecutionResult> executeOperation(
        OperationExecutionPaths const& paths, GtExecutableOperation& operation,
        GtObject* data, GtProject* project, GtExecutionEventStream& events,
        QString& error)
    {
        CurrentDirectoryGuard workingDirectory(paths.workingDirectory);
        if (!workingDirectory.isValid())
        {
            error = QObject::tr("Cannot use working directory: %1")
                        .arg(paths.workingDirectory);
            return std::nullopt;
        }

        GtExecutionEnvironment environment(project);
        return environment.execute(operation, data, events);
    }

    int reportOperationResult(GtOperationExecutionResult const& result,
                              GtStdioExecutionEventEncoder& encoder)
    {
        if (result.result)
        {
            const GtObjectMemento resultMemento = result.result->toMemento();
            if (resultMemento.isNull() || resultMemento.toByteArray().isEmpty())
            {
                return writeFailure(
                    encoder, outputFileExitCode,
                    QStringLiteral("result_serialization_error"),
                    QObject::tr("Cannot serialize the operation result."));
            }
        }

        if (!encoder.encodeOutcome(result))
        {
            gtError() << QObject::tr(
                "Cannot write the terminal operation outcome record.");
            return protocolOutputExitCode;
        }

        return result.status == GtOperationExecutionResult::Status::Success
                   ? 0
                   : executionExitCode;
    }

    int reportExecutionResult(GtExecutionResult const& execution,
                              EventOutput const& eventOutput,
                              OperationExecutionPaths const& paths,
                              GtStdioExecutionEventEncoder& encoder)
    {
        if (eventOutput.writer && eventOutput.writer->hasError())
        {
            const QString eventError = eventOutput.writer->errorString();
            return writeFailure(
                encoder, outputFileExitCode,
                QStringLiteral("event_output_error"),
                QObject::tr("Cannot write event output '%1': %2")
                    .arg(paths.events, eventError));
        }

        if (execution.error() != GtExecutionResult::Error::None)
        {
            return writeFailure(encoder, executionExitCode,
                                executionErrorCode(execution.error()),
                                execution.message());
        }

        const GtOperationExecutionResult* operationResult =
            execution.operationResult();
        if (!operationResult)
        {
            return writeFailure(
                encoder, executionExitCode,
                QStringLiteral("missing_operation_result"),
                QObject::tr(
                    "Execution environment returned no operation result."));
        }

        return reportOperationResult(*operationResult, encoder);
    }

} // namespace

QList<GtCommandLineOption>
gt::console::runOperationFromMementoOptions()
{
    OperationExecutionOptions options;
    const QList<QCommandLineOption> commandOptions = options.list();
    QList<GtCommandLineOption> result;
    result.reserve(commandOptions.size());
    std::transform(
        commandOptions.cbegin(), commandOptions.cend(),
        std::back_inserter(result), [](QCommandLineOption const& option) {
            return GtCommandLineOption{option.names(), option.description()};
        });
    return result;
}

int
gt::console::runOperationFromMemento(QStringList const& args)
{
    const GtExecutionId executionId;
    GtStdioExecutionEventEncoder terminalEncoder(executionId, std::cout);
    OperationExecutionOptions options;
    OperationExecutionPaths paths;
    if (const int status =
            parseExecutionPaths(args, options, terminalEncoder, paths))
    {
        return status;
    }

    std::unique_ptr<GtExecutableOperation> operation;
    std::unique_ptr<GtObject> data;
    std::unique_ptr<MementoExecutionProject> project;
    if (const int status = restoreExecutableOperation(
            paths.operationMemento, terminalEncoder, operation))
    {
        return status;
    }
    if (const int status =
            restoreDetachedData(paths.dataMemento, terminalEncoder, data))
    {
        return status;
    }
    if (const int status =
            restoreExecutionProject(paths, terminalEncoder, project))
    {
        return status;
    }

    GtExecutionEventStream events(executionId);
    EventOutput eventOutput;
    if (const int status =
            configureEventOutput(paths, events, terminalEncoder, eventOutput))
    {
        return status;
    }

    QString error;
    auto execution = executeOperation(paths, *operation, data.get(),
                                      project.get(), events, error);
    if (!execution)
    {
        return writeFailure(terminalEncoder, inputFileExitCode,
                            QStringLiteral("working_directory_error"), error);
    }

    return reportExecutionResult(*execution, eventOutput, paths,
                                 terminalEncoder);
}
