/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectspecwidget.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTSPECWIDGET_H
#define GTPROJECTSPECWIDGET_H

#include "gt_mdi_exports.h"

#include <QWidget>

class QLineEdit;
class QLabel;

/**
 * @brief The GtProjectSpecWidget class
 */
class GT_MDI_EXPORT GtProjectSpecWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    GtProjectSpecWidget(QWidget* parent = Q_NULLPTR);

    /**
     * @brief nameLine
     * @return
     */
    QLineEdit* nameLine();

    /**
     * @brief pathLine
     * @return
     */
    QLineEdit* pathLine();

    /**
     * @brief isValid
     * @return
     */
    bool isValid();

    /**
     * @brief setProjectInformation
     * @param initName
     * @param initPath
     */
    void setProjectInformation(const QString& initName,
                               const QString& initPath);

private:
    /// Dield for project name
    QLineEdit* m_nameLine;

    /// Dield for project path
    QLineEdit* m_pathLine;

    /// Name check pixmap
    QLabel* m_nameCheck;

    /// Path check pixmap
    QLabel* m_pathCheck;

    /// Temporary project name
    QString m_tmpName;

    /// Initialization path
    QString m_initPath;

    /// Validation state
    bool m_valid;

private slots:

    /**
     * @brief Updates all check states
     */
    bool updateStates();

    /**
     * @brief onNameChange
     * @param val
     */
    void onNameChange(const QString& val);

    /**
     * @brief browseDirectory
     */
    void browseDirectory();

signals:
    /**
     * @brief statesUpdated
     */
    void statesUpdated();

    /**
     * @brief validated - signal emitted after every update
     * @param state - true if name and directory are valid, else false
     */
    void validated(bool state);

};

#endif // GTPROJECTSPECWIDGET_H
