/* GTlab - Gas Turbine laboratory
 * Source File: test_process_interface
 * copyright 2009-2022 by DLR
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
    Q_PLUGIN_METADATA(IID "de.dlr.gtlab.GtProcessInterface/0.1"
                      FILE "test_process_interface.json")
					  
    Q_INTERFACES(GtModuleInterface)
    Q_INTERFACES(GtProcessInterface)

public:

	/**
     * @brief Returns current version number of module
     * @return version number
     */
    GtVersionNumber version() override;

    /**
     * @brief Returns module identification string.
     * @return identification string
     */
    QString ident() const override;

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

    /**
     * @brief Returns static meta objects of task classes.
     * @return list including meta objects
     */
    QList<GtTaskData> tasks() override;
};

#endif // TEST_MODULE_INTERFACE_H
 