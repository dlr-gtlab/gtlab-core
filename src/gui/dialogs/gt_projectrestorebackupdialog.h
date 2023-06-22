/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectrestorebackupdialog.h
 *
 *  Created on: 18.11.2022
 *  Author: Jens Schmeinkr (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTPROJECTRESTOREBACKUPDIALOG_H
#define GTPROJECTRESTOREBACKUPDIALOG_H

#include "gt_dialog.h"
#include <QDir>

class QListWidget;
class QListWidgetItem;
class GtTextEdit;

/**
 * @brief The GtProjectRestoreBackupDialog class
 * Dialog for the selection of a backup to use for the restore
 */
class GtProjectRestoreBackupDialog : public GtDialog
{
    Q_OBJECT
public:
    /**
     * @brief GtProjectRestoreBackupDialog
     * @param validBackupDirs - list of valid directories to offer as options
     * @param parent object
     *
     * Constructor to define the layout of the dialog
     */
    explicit GtProjectRestoreBackupDialog(QList<QDir> const& validBackupDirs,
                                          QWidget* parent = nullptr);

    /**
     * @brief selectedFolderName
     * @return the name of the selected folder
     */
    QString selectedFolderName() const;
public slots:
    /**
     * @brief onElementSelection
     * @param itemText to use as selection
     */
    void onElementSelection(QString itemText);

    /**
     * @brief onItemDoubleClicked
     * Reaction on a double click on an element: select item and close dialog
     * @param i - double clicked element
     */
    void onItemDoubleClicked(QListWidgetItem* i);

private:
    QListWidget* m_listW;

    GtTextEdit* m_messageWidget;

    QPushButton* m_okBtn;

    QString m_selectedFolderName;

    QList<QDir> m_folders;

    /**
     * @brief folderByName
     * Searches for a folder in the m_folders list by name
     * @param name
     * @return one of the folders of the given list selected by its name
     */
    QDir folderByName(QString const& name) const;
};

#endif // GTPROJECTRESTOREBACKUPDIALOG_H
