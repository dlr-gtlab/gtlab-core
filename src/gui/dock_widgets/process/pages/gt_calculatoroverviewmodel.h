/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatoroverviewmodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOROVERVIEWMODEL_H
#define GTCALCULATOROVERVIEWMODEL_H

#include "gt_processoverviewmodel.h"

#include <memory>

/**
 * @brief The GtCalculatorOverviewModel class
 */
class GtCalculatorOverviewModel : public GtProcessOverviewModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent
     */
    explicit GtCalculatorOverviewModel(QObject* parent = nullptr);

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

#endif // GTCALCULATOROVERVIEWMODEL_H
