/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_postmodel.h
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPOSTMODEL_H
#define GTPOSTMODEL_H

#include <QAbstractListModel>
#include <QPointer>

#include "gt_gui_exports.h"

class QDir;
class GtProject;
class GtPostTemplatePath;
class QDomDocument;
class QDomProcessingInstruction;
class QDomElement;

/**
 * @brief The GtPostModel class
 */
class GT_GUI_EXPORT GtPostModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param Parent object
     */
    explicit GtPostModel(QObject* parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index,
                 const QVariant &value, int role) override;

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief Returns post template entry list.
     * @return
     */
    const QStringList& entries();

    /**
     * @brief indexToFilename
     * @param index
     * @return
     */
    QString indexToFilename(const QModelIndex& index);

    /**
     * @brief filename
     * @param str
     * @return
     */
    QString createFilename(const QString& str);

    /**
     * @brief Updates data based on post templates stored inside project
     * directory.
     * @param Current project.
     */
    void refreshData(GtProject* project);

    /**
     * @brief Creates a new post template.
     * @return Template data model index.
     */
    QModelIndex newPostTemplate();

    /**
     * @brief Creates a template path based on given model index.
     * GtPostTemplatePath does not have ownership of the created template path.
     * @param Post model index. NULL if post template path could not be created.
     * @return New post template path.
     */
    GtPostTemplatePath* createTemplatePath(const QModelIndex& index);

    /**
     * @brief renameFile
     * @param oldName
     * @param newName
     * @return
     */
    bool renameFile(const QString& oldName, const QString& newName);

    /**
     * @brief createDocHeader
     * @param doc
     * @return
     */
    static QDomProcessingInstruction createDocHeader(QDomDocument& doc);

    /**
     * @brief createRoot
     * @param doc
     * @param name
     * @return
     */
    static QDomElement createRoot(QDomDocument& doc, const QString& name);

    /**
     * @brief deleteEntry
     * @param index
     */
    void deleteEntry(const QModelIndex& index);

private:
    /// Post template entries
    QStringList m_entries;

    /// Current project
    QPointer<GtProject> m_project;

    /**
     * @brief Fills the directory referenz with project specific post
     * processing path
     * @param dir - Reference to directory object.
     * @return Whether directory was filled or not.
     */
    bool projectPostDirectory(QDir& dir);

    /**
     * @brief Reads template id for given post template file path. Returns
     * empty id if file corrupted.
     * @param Post template file path.
     * @return Template id.
     */
    QString readTemplateId(const QString& path);

    /**
     * @brief setTemplateData
     * @param str
     */
    void setTemplateData(const QStringList& str);

    /**
     * @brief Adds a new entry to the template data.
     * @param Entry identification string.
     * @return Template data model index.
     */
    QModelIndex addEntry(const QString& str);
};

#endif // GTPOSTMODEL_H
