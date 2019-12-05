/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkforupdatesdialog.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CHECKFORUPDATESDIALOG_H
#define GT_CHECKFORUPDATESDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;

/**
 * @brief The GtCheckForUpdatesDialog class
 */
class GtCheckForUpdatesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget
     */
    GtCheckForUpdatesDialog(QWidget* parent = Q_NULLPTR);

private:
    /// Progress label
    QLabel* m_progressLabel;

    /// Update text label
    QLabel* m_txtLabel;

    /// Check for updates button
    QPushButton* m_checkButton;

    /// Run update button
    QPushButton* m_updateButton;

private slots:
    /**
     * @brief checkForUpdate
     */
    void checkForUpdate();

    /**
     * @brief updateAvailable
     */
    void updateAvailable();

    /**
     * @brief noUpdateAvailable
     * @param str
     */
    void noUpdateAvailable(int errorCode, const QString& str);

};

#endif // GT_CHECKFORUPDATESDIALOG_H
