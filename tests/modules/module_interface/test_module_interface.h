/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_module_interface
 * 
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TEST_MODULE_INTERFACE_H
#define TEST_MODULE_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestDatamodelInterface class
 */
class TestModuleInterface : public QObject,
		public GtModuleInterface
 {
    Q_OBJECT
    GT_MODULE("test_module_interface.json")

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

    QList<gt::VersionUpgradeRoutine> upgradeRoutines() const override;

    QList<gt::SharedFunction> sharedFunctions() const override;

    QList<GtCommandLineFunction> commandLineFunctions() const override;

    MetaInformation metaInformation() const override;

    void init() override;
};

#endif // TEST_MODULE_INTERFACE_H
