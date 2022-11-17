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
    QString m_message;

    QTextEdit* m_textEdit;

public slots:
    void onTextChanged();

signals:
    void textChanged(QString newText);
};

#endif // GTGENERATEBACKUPDIALOG_H
