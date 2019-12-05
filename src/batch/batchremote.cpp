/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 17.03.2016
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "batchremote.h"

#include <iostream>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "gt_objectmemento.h"
#include "gt_coreapplication.h"
#include "gt_objectfactory.h"
#include "gt_processfactory.h"
#include "gt_task.h"
#include "gt_coreprocessexecutor.h"
#include "gt_objectgroup.h"

bool
BatchRemote::checkInput(QString inputName, QString version, bool silent,
                        bool checkVersion)
{

    if (!silent)
    {
        qDebug() << "Start Check!";
    }

    QFile currentFile(inputName);

    if (!currentFile.exists())
    {
        qDebug() << "File does not exist";
        std::cout << "File does not exist" << std::endl;
        return false;
    }
    else
    {
        if (!silent)
        {
            qDebug() << "There is a file";
            std::cout << "There is a file" << std::endl;
        }

        QDomDocument document;

        QString errorStr;
        int errorLine;
        int errorColumn;

        if (!document.setContent(&currentFile, true, &errorStr,
                                 &errorLine, &errorColumn))
        {
            qDebug() << "XML ERROR!" << " " << "line" << ": "
                     << errorLine << " " << "column" << ": "
                     << errorColumn << " -> " << errorStr;
            return false;
        }
        else
        {
            if (!silent)
            {
                qDebug() << "The file is ok";
                std::cout << "The file is ok" << std::endl;
            }
        }

        QDomElement root = document.documentElement();

        if (root.isNull())
        {
            qDebug() << "ERROR: Invalid .gtlab file!";
            std::cout << "ERROR: Invalid .gtlab file!" << std::endl;
            return false;
        }

        if (root.tagName() != QStringLiteral("GTLAB"))
        {
            qDebug() << "ERROR: Invalid .gtlab file!";
            std::cout << "ERROR: Invalid .gtlab file!" << std::endl;
            return false;
        }

        if (checkVersion)
        {
            // GTlab-Version Check
            if (root.attribute("version") != version)
            {
                qDebug() << "Error: Wrong Version of GTlab!";
                return false;
            }

            // Modules-Version Check
            QDomElement modulesRoot =
                    root.firstChildElement(QStringLiteral("MODULES"));

            if (modulesRoot.isNull())
            {
                qDebug() << "ERROR: Invalid .gtlab file!";
                std::cout << "ERROR: Invalid .gtlab file!" << std::endl;
                return false;
            }

            if (modulesRoot.hasChildNodes())
            {
                QDomElement modulesElement = modulesRoot.firstChildElement();
                //root.firstChildElement();
                while (!modulesElement.isNull())
                {
                    QString currentModule = modulesElement.attribute("name");
                    QString currentModuleVersion =
                            modulesElement.attribute("version");


                    // check for module existance
                    if (!gtApp->moduleIds().contains(currentModule))
                    {
                        qDebug() << "ERROR: Module-Id not existing!";
                        return false;
                    }

                    // check for module version
                    int mv = gtApp->moduleVersion(currentModule);
                    if (mv != currentModuleVersion.toInt())
                    {
                        qDebug() << "ERROR: Module-Version not existing!";
                        return false;
                    }

                    modulesElement = modulesElement.nextSiblingElement();
                }
            }
            else
            {
                qDebug() << "No Modules -> No check possible!";
                return false;
            }

        }
        // data
        QDomElement dataRoot =
                root.firstChildElement(QStringLiteral("DATA"));

        QDomElement dataElement = dataRoot.firstChildElement();

        if (!silent)
        {
            std::cout << "Restoring Data-Elements...";
        }

        while (!dataElement.isNull())
        {
            GtObjectMemento memento(dataElement);

            if (!silent)
            {
                qDebug() << "root = " << dataElement.attribute("class");
            }

            bool restorable = false;
            restorable = memento.isRestorable(gtObjectFactory);

            if (restorable)
            {
                if (!silent)
                {
                    qDebug() << "   |-> ok!";
                }
            }

            else
            {
                return false;
            }

            dataElement = dataElement.nextSiblingElement();
        }

        if (!silent)
        {
            std::cout << "done!" << std::endl;
        }

        // process
        if (!silent)
        {
            std::cout << "Restoring Process-Elements...";
        }

        QDomElement processRoot =
                root.firstChildElement(QStringLiteral("PROCESS"));

        QDomElement processElement = processRoot.firstChildElement();

        while (!processElement.isNull())
        {
            GtObjectMemento memento(processElement);

            if (!silent)
            {
                qDebug() << "root = " << processElement.attribute("name");
            }

            bool restorable = false;
            restorable = memento.isRestorable(gtProcessFactory);

            if (restorable)
            {
                if (!silent)
                {
                    qDebug() << "   |-> ok!";
                }
            }

            else
            {
                return false;
            }

            processElement = processElement.nextSiblingElement();
        }

        if (!silent)
        {
            std::cout << "done!" << std::endl;
        }
    }

    return true;
}

bool
BatchRemote::run(QString inputName, QString outputName , QString version)
{
    qDebug() << "Test runs with Input:" << inputName << "and output:"
             << outputName;

    std::cout << "Run Tool!" << std::endl;

    QFile currentFile(inputName);

    if (!currentFile.exists())
    {
        qDebug() << "File does not exist";
        //std::cout << "File does not exist"<<endl;
        return false;
    }

    else
    {
        qDebug() << "There is a file";
        //std::cout << "There is a file" << endl;

        QDomDocument document;

        QString errorStr;
        int errorLine;
        int errorColumn;

        if (!document.setContent(&currentFile, true, &errorStr,
                                 &errorLine, &errorColumn))
        {
            qDebug() << "XML ERROR!" << " " << "line" << ": "
                     << errorLine << " " << "column" << ": "
                     << errorColumn << " -> " << errorStr;
            return false;
        }
        else
        {
            qDebug() << "The file is ok";
            std::cout << "The file is ok" << std::endl;
        }

        QDomElement root = document.documentElement();

        if (root.isNull())
        {
            qDebug() << "ERROR: Invalid .gtlab file!";
            std::cout << "ERROR: Invalid .gtlab file!" << std::endl;
            return false;
        }
        else
        {
            QDomElement modulesRoot =
                    root.firstChildElement(QStringLiteral("MODULES"));

            QDomElement modulesElement = modulesRoot.firstChildElement();
            QList<QString> modulesList;

            while (!modulesElement.isNull())
            {
                modulesList.append(modulesElement.attribute("name"));
                modulesElement = modulesElement.nextSiblingElement();

            }


            QDomElement dataRoot =
                    root.firstChildElement(QStringLiteral("DATA"));

            QDomElement dataElement = dataRoot.firstChildElement();

            //QString processName;
            GtObject* obj = NULL;
            QList<GtObject*> objectList;
            GtObjectGroup* objectGroup = new GtObjectGroup;

            qDebug() << "Restoring DATA";

            while (!dataElement.isNull())
            {

                qDebug() << "Restoring: " << dataElement.attribute("name");
                GtObjectMemento memento(dataElement);
                obj = memento.restore(gtObjectFactory);

                if (obj == NULL)
                {
                    qDebug() << "ERROR: Object" << dataElement.attribute("name")
                             << "not restorable!";
                    return false;
                }

                objectList.append(obj);
                qDebug() << "done!";
                dataElement = dataElement.nextSiblingElement();
            }
            foreach (GtObject* object, objectList)
            {
                objectGroup->appendChild(object);
            }

            qDebug() << "Restoring PROCESS";

            QDomElement processRoot =
                    root.firstChildElement(QStringLiteral("PROCESS"));

            QDomElement processElement = processRoot.firstChildElement();

            qDebug() << "Restoring: " << processElement.attribute("name");
            GtObjectMemento memento(processElement);
            GtTask* process = memento.restore<GtTask*>(gtProcessFactory);

            if (process == NULL)
            {
                qDebug() << "ERROR: Object" << processElement.attribute("name")
                         << "not restorable!";
                return false;
            }

            qDebug() << "done!";


            // EXECUTE PROCESS
            qDebug() << "Process execution!";
            std::cout << "Process execution" << std::endl;

            GtCoreProcessExecutor executor;
            executor.setSource(objectGroup);
            executor.runTask(process);

            if (process->currentState() != GtProcessComponent::FINISHED)
            {
                gtError() << "Calculator run failed!";
                return false;
            }

             // WRITE OUTPUT
            QDomDocument document;
            QDomProcessingInstruction header =
                    document.createProcessingInstruction(
                    QStringLiteral("xml"),
                    QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
            document.appendChild(header);

            QDomElement rootElement =
                document.createElement(QStringLiteral("GTLAB"));

            rootElement.setAttribute(QStringLiteral("version"), version);

            QDomElement modulesRootElement = document.createElement
                                        (QStringLiteral("MODULES"));
            foreach (QString moduleId, modulesList)
            {
                QDomElement moduleElement = document.createElement
                        (QStringLiteral("MODULE"));
                moduleElement.setAttribute(QStringLiteral("name"), moduleId);
                moduleElement.setAttribute(QStringLiteral("version"),
                                           gtApp->moduleVersion(moduleId));
                modulesRootElement.appendChild(moduleElement);
            }

            rootElement.appendChild(modulesRootElement);


            QDomElement dataRootElement = document.createElement
                                      (QStringLiteral("DATA"));

            foreach (GtObject* object, objectList)
            {
                GtObjectMemento memento = object->toMemento();
                dataRootElement.appendChild(memento.documentElement());
            }

            rootElement.appendChild(dataRootElement);

            document.appendChild(rootElement);

            QStringList outputDirList = outputName.split("/");
            outputDirList.removeLast();
            QString outputDir = outputDirList.join("/");

            QFile outputFile(outputDir + "/" + "workingoutput.xml");

            if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                return false;
            }

            qDebug() << "writing to output file...";
            qDebug() << outputFile.fileName();
            qDebug() << outputDir;

            QTextStream stream(&outputFile);
            stream << document.toString(5);
            outputFile.close();

            QFile oldFile(outputName);
            if (oldFile.exists())
            {
                qDebug() << "Deleting old file with the identical file name!";
                oldFile.remove();
            }

            if (!QDir().rename(outputDir + "/" + "workingoutput.xml",
                               outputName))
            {
                qDebug() << "Bad Name for output!";
                qDebug() << "Cannot rename from "
                         << QString(outputDir + "workingoutput.xml")
                         << "to" << outputName;
            }

        }
    }

    return true;
}
