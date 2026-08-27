/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_consoleruntaskfrommemento.h"

#include "gt_coreprocessexecutor.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_package.h"
#include "gt_processdata.h"
#include "gt_processfactory.h"
#include "gt_project.h"
#include "gt_task.h"
#include "gt_xmlutilities.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>

#include <algorithm>
#include <array>
#include <memory>

namespace
{

    class MementoExecutionProject final : public GtProject
    {
    public:
        explicit MementoExecutionProject(QString const& path) : GtProject(path)
        {
        }
    };

    std::unique_ptr<GtObjectGroup> restoreProjectData(QString const& fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            gtError() << QObject::tr("Cannot read project Memento '%1': %2")
                             .arg(fileName, file.errorString());
            return {};
        }

        GtObjectMemento memento(file.readAll());
        if (memento.isNull())
        {
            gtError()
                << QObject::tr("Invalid project Memento '%1'").arg(fileName);
            return {};
        }

        auto* group = memento.restore<GtObjectGroup*>(gtObjectFactory);
        if (!group)
        {
            gtError()
                << QObject::tr(
                       "Project Memento '%1' is not a supported object group")
                       .arg(fileName);
            return {};
        }

        return std::unique_ptr<GtObjectGroup>(group);
    }

    std::unique_ptr<GtTask> restoreTask(QString const& fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            gtError() << QObject::tr("Cannot read task Memento '%1': %2")
                             .arg(fileName, file.errorString());
            return {};
        }

        GtObjectMemento memento(file.readAll());
        if (memento.isNull())
        {
            gtError() << QObject::tr("Invalid task Memento '%1'").arg(fileName);
            return {};
        }

        auto* task = memento.restore<GtTask*>(gtProcessFactory);
        if (!task)
        {
            gtError()
                << QObject::tr("Unsupported task Memento '%1'").arg(fileName);
            return {};
        }

        return std::unique_ptr<GtTask>(task);
    }

    bool populateProject(GtObjectGroup& projectData, GtProject& project)
    {
        auto objects = projectData.findDirectChildren<GtObject*>();
        for (GtObject* object : objects)
        {
            if (!qobject_cast<GtPackage*>(object))
            {
                gtError()
                    << QObject::tr(
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
                gtError() << QObject::tr("Cannot add '%1' to execution project")
                                 .arg(object->objectName());
                delete object;
                return false;
            }
        }

        project.acceptChangesRecursively();
        return true;
    }

    bool writeDiff(QString const& fileName, GtObjectMementoDiff const& diff)
    {
        QSaveFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            gtError() << QObject::tr("Cannot write output diff '%1': %2")
                             .arg(fileName, file.errorString());
            return false;
        }

        const QByteArray data = diff.toByteArray();
        if (file.write(data) != data.size())
        {
            gtError() << QObject::tr(
                             "Cannot write complete output diff '%1': %2")
                             .arg(fileName, file.errorString());
            file.cancelWriting();
            return false;
        }

        if (!file.commit())
        {
            gtError() << QObject::tr("Cannot publish output diff '%1': %2")
                             .arg(fileName, file.errorString());
            return false;
        }

        return true;
    }

} // namespace

QList<GtCommandLineOption>
gt::console::runTaskFromMementoOptions()
{
    return {{{"project-memento", "p"}, "Serialized project-data Memento path"},
            {{"task-memento", "t"}, "Serialized task Memento path"},
            {{"output-diff", "o"}, "Output Memento-Diff path"},
            {{"working-directory", "w"},
             "Execution working directory (defaults to project Memento "
             "directory)"}};
}

int
gt::console::runTaskFromMemento(QStringList const& args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QObject::tr("Execute a task from project and task Mementos"));
    parser.addHelpOption();

    const QCommandLineOption projectOption({"p", "project-memento"},
                                           QObject::tr("Project-data Memento"),
                                           QObject::tr("path"));
    const QCommandLineOption taskOption({"t", "task-memento"},
                                        QObject::tr("Task Memento"),
                                        QObject::tr("path"));
    const QCommandLineOption outputOption({"o", "output-diff"},
                                          QObject::tr("Output Memento-Diff"),
                                          QObject::tr("path"));
    const QCommandLineOption workingDirectoryOption(
        {"w", "working-directory"}, QObject::tr("Execution working directory"),
        QObject::tr("path"));

    parser.addOptions(
        {projectOption, taskOption, outputOption, workingDirectoryOption});

    QStringList commandLine{QStringLiteral("run_task_from_memento")};
    commandLine.append(args);
    if (!parser.parse(commandLine))
    {
        gtError() << parser.errorText();
        return 2;
    }
    if (parser.isSet(QStringLiteral("help")))
    {
        parser.showHelp(0);
    }

    const std::array requiredOptions{&projectOption, &taskOption,
                                     &outputOption};
    auto missingOption = std::find_if(
        requiredOptions.cbegin(), requiredOptions.cend(),
        [&parser](auto const* option) {
            return !parser.isSet(*option) || parser.value(*option).isEmpty();
        });
    if (missingOption != requiredOptions.cend())
    {
        gtError() << QObject::tr("Missing required option --%1")
                         .arg((*missingOption)->names().constLast());
        return 2;
    }
    if (!parser.positionalArguments().isEmpty())
    {
        gtError() << QObject::tr("Unexpected positional arguments: %1")
                         .arg(parser.positionalArguments().join(' '));
        return 2;
    }

    const QFileInfo projectFile(parser.value(projectOption));
    const QFileInfo taskFile(parser.value(taskOption));
    const QString outputFile =
        QFileInfo(parser.value(outputOption)).absoluteFilePath();
    const QString workingDirectory =
        parser.isSet(workingDirectoryOption)
            ? QFileInfo(parser.value(workingDirectoryOption)).absoluteFilePath()
            : projectFile.absolutePath();

    if (!projectFile.isFile())
    {
        gtError() << QObject::tr("Project Memento does not exist: %1")
                         .arg(projectFile.absoluteFilePath());
        return 3;
    }
    if (!taskFile.isFile())
    {
        gtError() << QObject::tr("Task Memento does not exist: %1")
                         .arg(taskFile.absoluteFilePath());
        return 3;
    }
    if (!QFileInfo(workingDirectory).isDir())
    {
        gtError() << QObject::tr("Working directory does not exist: %1")
                         .arg(workingDirectory);
        return 3;
    }

    auto projectData = restoreProjectData(projectFile.absoluteFilePath());
    auto task = restoreTask(taskFile.absoluteFilePath());
    if (!projectData || !task)
    {
        return 4;
    }

    const GtObjectMemento initialProjectMemento = projectData->toMemento(true);

    MementoExecutionProject project(workingDirectory);
    project.setObjectName(projectData->objectName());
    project.setUuid(projectData->uuid());
    if (!populateProject(*projectData, project))
    {
        return 4;
    }

    GtProcessData processData;
    GtTask* executionTask = task.get();
    if (!processData.appendChild(executionTask))
    {
        gtError() << QObject::tr("Cannot attach task to execution context");
        return 4;
    }
    task.release();

    const QString previousWorkingDirectory = QDir::currentPath();
    if (!QDir::setCurrent(workingDirectory))
    {
        gtError() << QObject::tr("Cannot use working directory: %1")
                         .arg(workingDirectory);
        return 3;
    }

    GtCoreProcessExecutor executor;
    executor.setSource(&project);
    executor.setCustomProjectPath(workingDirectory);
    const auto state = executor.startTask(executionTask);

    if (!QDir::setCurrent(previousWorkingDirectory))
    {
        gtWarning() << QObject::tr("Cannot restore working directory: %1")
                           .arg(previousWorkingDirectory);
    }

    if (state != GtCoreProcessExecutor::TaskExecState::Started ||
        executionTask->currentState() != GtProcessComponent::FINISHED)
    {
        gtError() << QObject::tr("Task execution failed");
        return 5;
    }

    GtObjectMemento resultProjectMemento = project.toProjectDataMemento();
    resultProjectMemento.setIdent(initialProjectMemento.ident());
    GtObjectMementoDiff diff(initialProjectMemento, resultProjectMemento);
    if (!writeDiff(outputFile, diff))
    {
        return 6;
    }

    return 0;
}
