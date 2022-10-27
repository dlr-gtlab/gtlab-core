/* GTlab - Gas Turbine laboratory
 * Source File: gt_postmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QDir>
#include <QDomDocument>

#include "gt_icons.h"
#include "gt_project.h"
#include "gt_logging.h"
#include "gt_posttemplatepath.h"
#include "gt_coredatamodel.h"

#include "gt_postmodel.h"

GtPostModel::GtPostModel(QObject* parent) : QAbstractListModel(parent)
{
}

int
GtPostModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_entries.size();
}

QVariant
GtPostModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_entries.size())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            return m_entries[index.row()];
        }

        case Qt::DecorationRole:
        {
            return gt::gui::icon::post();
        }
    }

    return QVariant();
}

bool
GtPostModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() < 0 || index.row() >= m_entries.size())
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            QString newId = value.toString();

            if (newId.isEmpty())
            {
                return false;
            }

            m_entries[index.row()] = newId;

            // ToDo: edit file/filename

            return true;
        }
    }

    return QAbstractListModel::setData(index, value, role);
}

Qt::ItemFlags
GtPostModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    return Qt::ItemIsEditable | defaultFlags;
}

void
GtPostModel::setTemplateData(const QStringList& str)
{
    beginResetModel();
    m_entries = str;
    endResetModel();
}

const QStringList&
GtPostModel::entries()
{
    return m_entries;
}

QModelIndex
GtPostModel::addEntry(const QString& str)
{
    if (str.isEmpty())
    {
        return {};
    }

    if (m_entries.contains(str))
    {
        return {};
    }

    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries << str;
    endInsertRows();

    return index(m_entries.size() - 1);
}

void
GtPostModel::deleteEntry(const QModelIndex& index)
{
    QString filename = indexToFilename(index);

    qDebug() << "filename = " << filename;

    QDir projectDir;

    if (!projectPostDirectory(projectDir))
    {
        return;
    }

    QFile file(projectDir.absoluteFilePath(filename));

    if (!file.exists())
    {
        gtWarning() << tr("Template file does not exist, cannot delete it") <<
                       QStringLiteral("! (") << filename << QStringLiteral(")");

        return;
    }

    if (!file.remove())
    {
        gtWarning() << tr("Template file could not be deleted") <<
                       QStringLiteral("! (") << filename << QStringLiteral(")");

        return;
    }

    QString str = data(index).toString();

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_entries.removeOne(str);
    endRemoveRows();
}

QDomProcessingInstruction
GtPostModel::createDocHeader(QDomDocument& doc)
{
    return doc.createProcessingInstruction(
                QStringLiteral("xml"),
                QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
}

QDomElement
GtPostModel::createRoot(QDomDocument& doc, const QString& name)
{
    QDomElement root = doc.createElement("GTlabPost");
    root.setAttribute("version", "0.1");
    root.setAttribute("name", name);

    return root;
}

QString
GtPostModel::indexToFilename(const QModelIndex& index)
{
    if (!index.isValid())
    {
        qDebug() << "index not valid";
        return QString();
    }

    if (index.model() != this)
    {
        qDebug() << "model is not this";
        return QString();
    }

    return createFilename(index.data().toString());
}

QString
GtPostModel::createFilename(const QString& str)
{
    QString retval = str;
    retval.remove(" ");
    retval = retval.toLower();
    retval = retval + QStringLiteral(".post");

    return retval;
}

void
GtPostModel::refreshData(GtProject* project)
{
    m_project = project;

    if (!m_project)
    {
        setTemplateData(QStringList());
        return;
    }

    QDir projectDir;

    if (!projectPostDirectory(projectDir))
    {
        setTemplateData(QStringList());
        return;
    }

    QStringList entries = projectDir.entryList(QStringList() <<
                                               QStringLiteral("*.post"));

    QStringList templateData;

    foreach (const QString& str, entries)
    {
        QString id = readTemplateId(projectDir.absoluteFilePath(str));

        if (!id.isEmpty())
        {
            templateData << id;
        }
    }

    setTemplateData(templateData);
}

QModelIndex
GtPostModel::newPostTemplate()
{
    if (!m_project)
    {
        return {};
    }

    QDir projectDir;

    if (!projectPostDirectory(projectDir))
    {
        return {};
    }

    QStringList oldEntries = entries();

    QString id = getUniqueName(QStringLiteral("New_Template"), oldEntries);

    QString filename = createFilename(id);

    QFile file(projectDir.absoluteFilePath(filename));

    if (file.exists())
    {
        gtWarning() << tr("template file already exists") <<
                       QStringLiteral("! (") << filename << QStringLiteral(")");
    }

    QDomDocument document;
    QDomProcessingInstruction header = createDocHeader(document);
    document.appendChild(header);

    QDomElement rootElement = createRoot(document, id);

//    QDomElement rootElement =
//            document.createElement(QStringLiteral("GTlabPost"));

//    rootElement.setAttribute(QStringLiteral("name"), id);

    // ToDo: version attribute

    document.appendChild(rootElement);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtWarning() << objectName() << QStringLiteral(": ")
                    << tr("Failed to save template data!")
                    << QStringLiteral(" (") << id << QStringLiteral(")");
        return {};
    }

    QTextStream stream(&file);
    stream << document.toString(5);

    file.close();

    return addEntry(id);
}

GtPostTemplatePath*
GtPostModel::createTemplatePath(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return nullptr;
    }

    if (index.model() != this)
    {
        return nullptr;
    }

    QString filename = indexToFilename(index);
    qDebug() << "index = " << index;
    qDebug() << "filename = " << filename;

    if (filename.isEmpty())
    {
        return nullptr;
    }

    QDir projectDir;

    if (!projectPostDirectory(projectDir))
    {
        return nullptr;
    }

    return new GtPostTemplatePath(projectDir.absoluteFilePath(filename));
}

bool
GtPostModel::renameFile(const QString& oldName, const QString& newName)
{
    QDir projectDir;

    if (!projectPostDirectory(projectDir))
    {
        return false;
    }

    QString filename = createFilename(oldName);
    QString filenameNew = createFilename(newName);

    QFile file(projectDir.absoluteFilePath(filename));

    if (!file.exists())
    {
        gtDebug() << tr("File does not exist") << QStringLiteral(": ")
                  << projectDir.absoluteFilePath(filename);
        return false;
    }

    QString fileNewAndPath = projectDir.absoluteFilePath(filenameNew);

    if (file.rename(fileNewAndPath))
    {
        gtDebug() << tr("Renaming of file was successful") << QStringLiteral(": ")
                  << fileNewAndPath;

        QFile fileNew(fileNewAndPath);

        if (!fileNew.exists())
        {
            gtWarning() << tr("Template file does not exist")
                        << QStringLiteral("! (")
                        << fileNewAndPath << QStringLiteral(")");
        }

        if (!fileNew.open(QIODevice::ReadWrite))
        {
            gtWarning() << objectName() << QStringLiteral(": ")
                        << tr("Failed to edit template data!")
                        << QStringLiteral(" (") << newName << QStringLiteral(")");
            return false;
        }

        QByteArray xmlData(fileNew.readAll());

        QDomDocument document;

        QString errorStr;
        int errorLine;
        int errorColumn;

        if (!document.setContent(xmlData, true, &errorStr,
                                 &errorLine, &errorColumn))
        {
            gtWarning() << tr("Template file does not exist")
                        << QStringLiteral("! (")
                        << fileNewAndPath << QStringLiteral(")");
            return false;
        }

        createDocHeader(document);

        QDomElement root = document.documentElement();

        if (root.isNull())
        {
            // TODO: warning
            return false;
        }

        root.setAttribute(QStringLiteral("name"), newName);

        fileNew.resize(0);
        QTextStream stream(&fileNew);
        stream << document.toString(5);

        fileNew.close();

        return true;
    }

    gtDebug() << tr("Renaming was not successful") << QStringLiteral(": ")
              << fileNewAndPath;

    return false;
}

bool
GtPostModel::projectPostDirectory(QDir& dir)
{
    if (!m_project)
    {
        return false;
    }

    dir = QDir(m_project->path());

    if (!dir.exists())
    {
        return false;
    }

    if (!dir.cd(QStringLiteral("post")))
    {
        if (!dir.mkpath(dir.absolutePath() + QDir::separator()
                               + QStringLiteral("post")))
        {
            gtWarning() <<
                   tr("Could not create post template project directory!");
            return false;
        }

        if (!dir.cd(QStringLiteral("post")))
        {
            gtWarning() << tr("post template project directory corrupted!");
            return false;
        }
    }

    return true;
}

QString
GtPostModel::readTemplateId(const QString& path)
{
    QFile file(path);

    if (!file.exists())
    {
        gtWarning() << tr("template file not found") << QStringLiteral("! (")
                    << path << QStringLiteral(")");
        return QString();
    }

    if (!file.open(QFile::ReadOnly))
    {
        gtWarning() << tr("could not open template file") <<
                       QStringLiteral("! (") << path << QStringLiteral(")");
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        gtWarning() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return QString();
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTlabPost")))
    {
        gtWarning() << tr("ERROR: Invalid GTlab Template view file!");
        return QString();
    }

    // ToDo: version check

    return root.attribute(QStringLiteral("name"));
}
