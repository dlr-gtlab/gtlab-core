/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_process_interface
 *
 * Created on: 20.05.2022
 * Author: J. Schmeink
 */

#ifndef TEST_MODULE_INTERFACE_H
#define TEST_MODULE_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_processinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestDatamodelInterface class
 */
class TestProcessInterface : public QObject,
        public GtModuleInterface, public GtProcessInterface
 {
    Q_OBJECT
    GT_MODULE("test_process_interface.json")

    Q_INTERFACES(GtProcessInterface)

public:

    /**
     * @brief Returns current version number of module
     * @return version number
     */
    GtVersionNumber version() override;

    /**
     * @brief Returns module description
     * @return description
     */
    QString description() const override;

    /**
     * @brief Returns static meta objects of calculator classes.
     * @return list including meta objects
     */
    QList<GtCalculatorData> calculators() override;
};

#endif // TEST_MODULE_INTERFACE_H
 
