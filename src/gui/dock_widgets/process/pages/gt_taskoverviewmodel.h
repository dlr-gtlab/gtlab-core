/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_taskoverviewmodel.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_TASKOVERVIEWMODEL_H
#define GT_TASKOVERVIEWMODEL_H

#include "gt_processoverviewmodel.h"

/**
 * @brief The GtTaskOverviewModel class
 */
class GtTaskOverviewModel : public GtProcessOverviewModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent
     */
    explicit GtTaskOverviewModel(QObject* parent = nullptr);

    /**
     * @brief Virtual function to implement specific model setup.
     */
    void setUpModel() override;

protected:
    /**
     * @brief Returns identification string of given process item.
     * @param item Process item.
     * @return Identification string of process item.
     */
    QString id(GtAbstractProcessItem* item) const override;

    /**
     * @brief Returns version string of given process item.
     * @param item Process item.
     * @return Version string of process item.
     */
    QString version(GtAbstractProcessItem* item) const override;

    /**
     * @brief Returns icon of given process item.
     * @param item Process item.
     * @return Icon of process item.
     */
    QIcon icon(GtAbstractProcessItem* item) const override;

    /**
     * @brief Returns description of given process item.
     * @param item Process item.
     * @return description of process item.
     */
    QString description(GtAbstractProcessItem* item) const override;

};

#endif // GT_TASKOVERVIEWMODEL_H
