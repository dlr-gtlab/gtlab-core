/* GTlab - Gas Turbine laboratory
 * Source File: gt_environmentdialog.h
 * copyright 2009-2019 by DLR
 *
 *  Created on: 26.07.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ENVIRONMENTDIALOG_H
#define GT_ENVIRONMENTDIALOG_H

#include "gt_mdi_exports.h"

#include <QDialog>

class GtEnvironmentModel;

/**
 * @brief The GtEnvironmentDialog class
 */
class GT_MDI_EXPORT GtEnvironmentDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param envVars Specific environment variables
     * @param winTitle Window title
     * @param infoText Information text
     * @param parent Parent Widget
     */
    GtEnvironmentDialog(const QStringList& envVars,
                        const QString& winTitle,
                        const QString& infoText,
                        QWidget* parent = Q_NULLPTR);

private:
    /// Environment model
    GtEnvironmentModel* m_envModel;

private slots:
    /**
     * @brief Saves environment settings and accepts dialog.
     */
    void save();

};

#endif // GT_ENVIRONMENTDIALOG_H
