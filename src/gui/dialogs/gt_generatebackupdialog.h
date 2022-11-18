/* GTlab - Gas Turbine laboratory
 * Source File: gt_generatebackupdialog.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Jens Schmeinkr (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTGENERATEBACKUPDIALOG_H
#define GTGENERATEBACKUPDIALOG_H

#include "gt_dialog.h"

class GtProject;
class QTextEdit;

/**
 * @brief The GtGenerateBackUpDialog class
 * Dialog class for the communication with the user to add a message
 * to a backup of a project and confirm its generation
 */
class GtGenerateBackUpDialog : public GtDialog
{
    Q_OBJECT
public:
    /**
     * @brief GtGenerateBackUpDialog
     * Constructor to define the layout
     * @param parent
     */
    GtGenerateBackUpDialog(QWidget* parent = nullptr);

    /**
     * @brief message
     * @return the message written in the text edit
     */
    QString message() const;
private:
    /// resulting message from dialog
    QString m_message;

    /// main text edit
    QTextEdit* m_textEdit;

public slots:
    /// reaction on a change in the main text edit: signal with the new text
    /// is emited
    void onTextChanged();

signals:
    /// signal emited when the text of the main text edit is changed
    void textChanged(QString newText);
};

#endif // GTGENERATEBACKUPDIALOG_H
