/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_datamodel_interface
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#ifndef TEST_DATAMODEL_INTERFACE_H
#define TEST_DATAMODEL_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_datamodelinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestDatamodelInterface class
 */
class TestDatamodelInterface : public QObject,
        public GtModuleInterface,
        public GtDatamodelInterface
 {
    Q_OBJECT
    GT_MODULE("test_datamodel_interface.json")

    Q_INTERFACES(GtModuleInterface)
    Q_INTERFACES(GtDatamodelInterface)


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
     * @brief Returns static meta objects of datamodel classes.
     * @return list including meta objects
     */
    QList<QMetaObject> data() override;

    /**
     * @brief Returns static meta objects of datamodel classes.
     * @return list including meta objects
     */
    QMetaObject package() override;

    /**
     * @brief Returns true if module is a stand alone module with own
     * data model structure. Otherwise module only extends the overall
     * application with additional functionalities like classes, calculators
     * or graphical user interfaces.
     * @return Stand alone indicator.
     */
    bool standAlone() override;

    /* Return a custom shared function */
    QList<gt::SharedFunction> sharedFunctions() const override;
};

#endif // TEST_DATAMODEL_INTERFACE_H

