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
#include <iostream>
#include <memory>
#include <utility>

namespace
{
struct Options
{
    QCommandLineOption operation{QStringList{"operation-memento"}, QObject::tr("Operation Memento XML"), "path"};
    QCommandLineOption data{QStringList{"data-memento"}, QObject::tr("Optional detached data Memento XML"), "path"};
    QCommandLineOption project{QStringList{"project-memento"}, QObject::tr("Optional project-data Memento XML"), "path"};
    QCommandLineOption events{QStringList{"events"}, QObject::tr("Optional NDJSON execution event file"), "path"};
    QCommandLineOption workingDirectory{QStringList{"working-directory"}, QObject::tr("Execution working directory"), "path"};
    QList<QCommandLineOption> all() const { return {operation, data, project, events, workingDirectory}; }
};

QString absolute(QString const& path) { return QDir::cleanPath(QFileInfo(path).absoluteFilePath()); }

std::unique_ptr<GtObject> restore(QString const& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return {};
    GtObjectMemento memento(file.readAll());
    return memento.toObject(*gtObjectFactory);
}

int fail(GtStdioExecutionEventEncoder& encoder, QString code, QString message, int rc)
{
    encoder.encodeFailure(std::move(code), std::move(message));
    return rc;
}
}

QList<GtCommandLineOption> gt::console::runOperationFromMementoOptions()
{
    Options options;
    QList<GtCommandLineOption> result;
    for (auto const& option : options.all()) result.append({option.names(), option.description()});
    return result;
}

int gt::console::runOperationFromMemento(QStringList const& args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Execute an operation from Mementos"));
    parser.addHelpOption();
    const GtExecutionId executionId;
    Options options;
    parser.addOptions(options.all());
    QStringList argv{"run_operation_from_memento"};
    argv.append(args);
    if (!parser.parse(argv))
    {
        GtStdioExecutionEventEncoder encoder(executionId, std::cout);
        return fail(encoder, "invalid_arguments", parser.errorText(), 2);
    }
    if (parser.isSet("help")) parser.showHelp(0);
    GtStdioExecutionEventEncoder terminal(executionId, std::cout);
    if (!parser.isSet(options.operation) || parser.value(options.operation).isEmpty() ||
        !parser.positionalArguments().isEmpty())
        return fail(terminal, "invalid_arguments", "--operation-memento is required; positional arguments are not accepted", 2);

    const QString operationPath = absolute(parser.value(options.operation));
    const QString dataPath = parser.isSet(options.data) ? absolute(parser.value(options.data)) : QString();
    const QString projectPath = parser.isSet(options.project) ? absolute(parser.value(options.project)) : QString();
    const QString eventsPath = parser.isSet(options.events) ? absolute(parser.value(options.events)) : QString();
    const QString workingDirectory = parser.isSet(options.workingDirectory)
        ? absolute(parser.value(options.workingDirectory))
        : (projectPath.isEmpty() ? QDir::currentPath() : QFileInfo(projectPath).absolutePath());

    for (QString const& input : {operationPath, dataPath, projectPath})
        if (!input.isEmpty() && eventsPath == input)
            return fail(terminal, "invalid_arguments", "Event output must not overwrite an input Memento", 2);
    if (!QFileInfo(operationPath).isFile() || (!dataPath.isEmpty() && !QFileInfo(dataPath).isFile()) ||
        (!projectPath.isEmpty() && !QFileInfo(projectPath).isFile()) || !QFileInfo(workingDirectory).isDir())
        return fail(terminal, "invalid_input", "An input Memento or working directory does not exist", 3);

    auto operationObject = restore(operationPath);
    auto* executable = qobject_cast<GtExecutableOperation*>(operationObject.get());
    if (!executable) return fail(terminal, "invalid_operation", "Operation Memento could not be restored as an executable operation", 4);
    operationObject.release();
    std::unique_ptr<GtExecutableOperation> operation(executable);
    std::unique_ptr<GtObject> data;
    if (!dataPath.isEmpty() && !(data = restore(dataPath)))
        return fail(terminal, "invalid_data_memento", "Detached data Memento could not be restored", 4);
    std::unique_ptr<GtObjectGroup> projectData;
    if (!projectPath.isEmpty())
    {
        auto object = restore(projectPath);
        auto* group = qobject_cast<GtObjectGroup*>(object.get());
        if (!group) return fail(terminal, "invalid_project_memento", "Project Memento is not project-data format", 4);
        object.release();
        projectData.reset(group);
    }

    const QString previousDirectory = QDir::currentPath();
    if (!QDir::setCurrent(workingDirectory))
        return fail(terminal, "working_directory_failed", "Could not select the working directory", 3);
    struct RestoreDirectory { QString path; ~RestoreDirectory() { QDir::setCurrent(path); } } restoreDirectory{previousDirectory};
    GtProjectRuntime runtime;
    auto initialized = runtime.initialize();
    if (!initialized.succeeded()) return fail(terminal, "runtime_failed", initialized.message, 5);
    if (projectData)
    {
        auto opened = runtime.openProjectFromMemento(std::move(projectData), workingDirectory);
        if (!opened.succeeded()) return fail(terminal, "project_failed", opened.message, 5);
    }

    GtExecutionEventStream stream(executionId);
    std::unique_ptr<GtExecutionEventFileWriter> writer;
    if (!eventsPath.isEmpty())
    {
        writer = std::make_unique<GtExecutionEventFileWriter>(eventsPath);
        if (!writer->isOpen()) return fail(terminal, "event_output_failed", writer->errorString(), 6);
        QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                         writer.get(), &GtExecutionEventFileWriter::writeEvent);
    }
    auto result = runtime.executeOperation(std::move(operation), std::move(data), stream);
    if (writer && writer->hasError()) return fail(terminal, "event_output_failed", writer->errorString(), 6);
    if (!result.succeeded()) return fail(terminal, "execution_failed", result.message, 5);
    return terminal.encodeResult(result.result.get()) ? 0 : 6;
}
