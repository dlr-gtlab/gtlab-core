/* GTlab - Gas Turbine laboratory
 * Source File: gt_changelogdialog.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCHANGELOGDIALOG_H
#define GTCHANGELOGDIALOG_H

#include <QDialog>

class QTextEdit;

/**
 * @brief The GtChangelogDialog class
 */
class GtChangelogDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param Parent widget
     */
    explicit GtChangelogDialog(QWidget* parent = nullptr);

    /**
     * @brief Called for popup changelog notification.
     */
    void setToPopup();

private:
    bool readChangelog(QTextEdit* textEdit);

};

#endif // GTCHANGELOGDIALOG_H
