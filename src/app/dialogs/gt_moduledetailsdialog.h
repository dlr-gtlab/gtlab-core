/* GTlab - Gas Turbine laboratory
 * Source File: gt_moduledetailsdialog.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTMODULEDETAILSDIALOG_H
#define GTMODULEDETAILSDIALOG_H

#include "gt_dialog.h"
#include "gt_textedit.h"

class QGridLayout;
class QTextEdit;

/**
 * @brief The GtModuleDetailsDialog class
 * Dialog to show the details of a single module
 */
class GtModuleDetailsDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtModuleDetailsDialog
     * @param moduleId - id of the module the dialog should be opened for
     * @param parent
     */
    explicit GtModuleDetailsDialog(QString const& moduleId,
                                   QWidget* parent = nullptr);

private:
    /**
     * @brief generalInformation
     * @return widget with a overview of general information like name,
     * descripion, ...
     */
    QWidget* generalInformation();

    /**
     * @brief changeLogWidget
     * @return widget for the changelog overview
     */
    QWidget* changeLogWidget();

    /**
     * @brief readMeWidget
     * @return widget for the readme content
     */
    QWidget* readMeWidget();

    /// id of the module
    QString m_moduleId;

    /**
     * @brief addLine
     * adds a line with the given title and value to the dialog
     * @param title
     * @param value
     */
    void addLine(const QString& title, const QString& value);

    /// main grid layout of the dialog
    QGridLayout* m_lay;

    /// internal counter for the lines of the grid which should be filled next
    int m_rowCounter;

    /**
     * @brief loadInfoFile
     * Reads the content of the module information file and sets
     * the filetype. This helps to render the text in the correct way.
     *
     * @return content of the module information file
     */
    std::tuple<QString, GtTextEdit::contentType> loadInfoFile(
            QString const& filter);
};

#endif // GTMODULEDETAILSDIALOG_H
