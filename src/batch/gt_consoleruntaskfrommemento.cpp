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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QMetaEnum>

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


    struct MementoExecutionOptions
    {
        const QCommandLineOption project{ {"p", "project-memento"},
                                               QObject::tr("Serialized project-data memento path"),
                                               QObject::tr("path") };
        const QCommandLineOption task{ {"t", "task-memento"},
                                            QObject::tr("Serialized task memento path"),
                                            QObject::tr("path") };

        const QCommandLineOption outputProject{ {"o", "output-diff"},
                                                     QObject::tr("Output project memento-diff"),
                                                     QObject::tr("path")};
        const QCommandLineOption outputTask{ {"m", "task-diff"},
                                            QObject::tr("Optional: Output task memento-diff"),
                                            QObject::tr("path")};
        const QCommandLineOption outputState{ {"s", "task-state"},
                                                   QObject::tr("Optional: Output task state JSON"),
                                                   QObject::tr("path") };
        const QCommandLineOption workingDirectory{ {"w", "working-directory"},
                                                        QObject::tr("Optional: Execution working directory (defaults to project memento directory)"),
                                                        QObject::tr("path")};

        const QList<QCommandLineOption> list()
        {
            return {project, task, outputProject, outputTask, outputState, workingDirectory};
        }
    };

    const QString normalizedFilePath(const QString& filepath) {
        return QDir::cleanPath(QFileInfo(filepath).absoluteFilePath());
    }

    struct MementoExecutionPaths
    {
        QString inputProject;
        QString inputTask;
        QString outputProject;
        QString outputTask;
        QString outputState;
        QString workingDirectory;

        static MementoExecutionPaths fromParser(
            const QCommandLineParser& parser,
            const MementoExecutionOptions& options)
        {
            const QString projectFile = normalizedFilePath(parser.value(options.project));

            MementoExecutionPaths retval;

            retval.inputProject = projectFile;
            retval.inputTask = normalizedFilePath(parser.value(options.task));

            retval.outputProject = normalizedFilePath(parser.value(options.outputProject));

            retval.outputTask = parser.isSet(options.outputTask)
                                    ? normalizedFilePath(parser.value(options.outputTask))
                                    : QString();

            retval.outputState = parser.isSet(options.outputState)
                                     ? normalizedFilePath(parser.value(options.outputState))
                                     : QString();

            retval.workingDirectory = parser.isSet(options.workingDirectory)
                                          ? normalizedFilePath(parser.value(options.workingDirectory))
                                          : QFileInfo{projectFile}.absolutePath();
            return retval;
        }

        bool sanityCheck()
        {
            auto outputPaths = { outputProject, outputTask, outputState };
            auto inputPaths = { inputProject, inputTask };

            for (QString const& outputPath : outputPaths)
            {
                // check if output would overwrite an input file
                for (QString const& inputPath : inputPaths)
                {
                    if (outputPath == inputPath)
                    {
                        gtError() << QObject::tr("Output file must not overwrite the input file '%1'!")
                        .arg(inputPath);
                        return false;
                    }
                }

                // check if output would overwrite another output file
                auto count = std::accumulate(outputPaths.begin(), outputPaths.end(), unsigned{0}, [&outputPath ](unsigned count, QString const& nextOutput){
                    return count + unsigned{outputPath == nextOutput};
                });
                if (!outputPath.isEmpty() && count > 1)
                {
                    gtError() << QObject::tr("Output files must not overwrite each other (file: '%1')!")
                    .arg(outputPath);
                    return false;
                }
            }

            return true;
        };

        bool checkInputExists()
        {
            if (!QFileInfo(inputProject).isFile())
            {
                gtError() << QObject::tr("Project Memento does not exist: %1")
                .arg(inputProject);
                return false;
            }
            if (!QFileInfo(inputTask).isFile())
            {
                gtError() << QObject::tr("Task Memento does not exist: %1")
                .arg(inputTask);
                return false;
            }
            if (!QFileInfo(workingDirectory).isDir())
            {
                gtError() << QObject::tr("Working directory does not exist: %1")
                .arg(workingDirectory);
                return false;
            }

            return true;
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

        if (!gt::xml::writeDomDocumentToDevice(file, diff.doc()))
        {
            gtError()
                << QObject::tr("Cannot write complete output diff '%1': %2")
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

    bool writeTaskState(QString const& fileName, QJsonObject const& taskstate)
    {
        if (fileName.isEmpty())
        {
            return true;
        }

        QJsonDocument statusOutputDoc(taskstate);

        QSaveFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            gtError() << QObject::tr("Cannot write output task state json '%1': %2")
                             .arg(fileName, file.errorString());
            return false;
        }

        if (file.write(statusOutputDoc.toJson(QJsonDocument::Indented)) == -1)
        {
            gtError() << QObject::tr("Cannot write complete output task state json '%1': %2")
                             .arg(fileName, file.errorString());
            file.cancelWriting();
            return false;
        }

        if (!file.commit())
        {
            gtError() << QObject::tr("Cannot publish output task state json '%1': %2")
                             .arg(fileName, file.errorString());
            return false;
        }

        return true;
    }

    bool removeExistingOutput(QString const& fileName)
    {
        QFile outputFile(fileName);
        if (!outputFile.exists())
        {
            return true;
        }

        if (!outputFile.remove())
        {
            gtError() << QObject::tr("Cannot remove existing output diff '%1': %2")
                             .arg(fileName, outputFile.errorString());
            return false;
        }

        return true;
    }

} // namespace


QList<GtCommandLineOption>
gt::console::runTaskFromMementoOptions()
{
    MementoExecutionOptions options;
    QList<GtCommandLineOption> theList;

    for(auto& option: options.list())
    {
        theList.append(GtCommandLineOption{option.names(), option.description()});
    }
    return theList;
}




int
gt::console::runTaskFromMemento(QStringList const& args)
{
    QCommandLineParser parser;

    parser.setApplicationDescription(
        QObject::tr("Execute a task from project and task Mementos"));
    parser.addHelpOption();

    MementoExecutionOptions options;
    parser.addOptions(options.list());

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

    const std::array requiredOptions{&options.project, &options.task, &options.outputProject};
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


    auto runPaths = MementoExecutionPaths::fromParser(parser, options);


    if (!runPaths.sanityCheck())
    {
        return 2;
    }

    if(!runPaths.checkInputExists())
    {
        return 3;
    }


    auto projectData = restoreProjectData(runPaths.inputProject);
    auto task = restoreTask(runPaths.inputTask);
    if (!projectData || !task)
    {
        return 4;
    }

    const GtObjectMemento initialProjectMemento = projectData->toMemento(true);
    const GtObjectMemento initialTaskMemento = task->toMemento(true);
    QJsonObject taskStateOutput;

    MementoExecutionProject project(runPaths.workingDirectory);
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

    if (!removeExistingOutput(runPaths.outputProject))
    {
        return 6;
    }
    if (!removeExistingOutput(runPaths.outputTask))
    {
        return 6;
    }
    if (!removeExistingOutput(runPaths.outputState))
    {
        return 6;
    }

    const QString previousWorkingDirectory = QDir::currentPath();
    if (!QDir::setCurrent(runPaths.workingDirectory))
    {
        gtError() << QObject::tr("Cannot use working directory: %1")
                         .arg(runPaths.workingDirectory);
        return 3;
    }

    GtCoreProcessExecutor executor;
    executor.setSource(&project);
    executor.setCustomProjectPath(runPaths.workingDirectory);
    const auto state = executor.startTask(executionTask);

    if (!QDir::setCurrent(previousWorkingDirectory))
    {
        gtWarning() << QObject::tr("Cannot restore working directory: %1")
                           .arg(previousWorkingDirectory);
    }

    const auto taskState = executionTask->currentState();

    taskStateOutput["taskState"] =  QString::fromLatin1(
        QMetaEnum::fromType<GtProcessComponent::STATE>().valueToKey(taskState)
    );
    if(!writeTaskState(runPaths.outputState, taskStateOutput))
    {
        gtError() <<  QObject::tr("Cannot write task state json: %1")
                           .arg(runPaths.outputState);
        return 6;
    }

    if (state != GtCoreProcessExecutor::TaskExecState::Started ||
        (taskState != GtProcessComponent::FINISHED &&
         taskState != GtProcessComponent::WARN_FINISHED))
    {
        gtError() << QObject::tr("Task execution failed");
        return 5;
    }

    GtObjectMemento resultProjectMemento = project.toProjectDataMemento();
    resultProjectMemento.setIdent(initialProjectMemento.ident());
    GtObjectMementoDiff diff(initialProjectMemento, resultProjectMemento);
    if (!writeDiff(runPaths.outputProject, diff))
    {
        return 6;
    }

    GtObjectMemento resultTaskMemento = executionTask->toMemento(true);
    resultTaskMemento.setIdent(initialTaskMemento.ident());
    GtObjectMementoDiff taskdiff(initialTaskMemento, resultTaskMemento);
    if (!runPaths.outputTask.isEmpty() && !writeDiff(runPaths.outputTask, taskdiff))
    {
        return 6;
    }


    return 0;
}
