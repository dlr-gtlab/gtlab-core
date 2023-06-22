/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractexternalizationinterface.h
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTABSTRACTEXTERNALIZATIONINTERFACE_H
#define GTABSTRACTEXTERNALIZATIONINTERFACE_H

#include "gt_datamodel_exports.h"

#include <QObject>

/**
 * @brief The GtAbstractExternalizationInterface class
 */
class GT_DATAMODEL_EXPORT GtAbstractExternalizationInterface : public QObject
{
    Q_OBJECT

public:

    virtual ~GtAbstractExternalizationInterface() = default;

    /**
     * @brief method to run, when project was loaded
     */
    virtual void onProjectLoaded(const QString& projectDir) = 0;

protected:

    /**
     * @brief GtAbstractExternalizationInterface. Will register the
     * interface in the externalization interface
     */
    GtAbstractExternalizationInterface();
};

#endif // GTABSTRACTEXTERNALIZATIONINTERFACE_H
