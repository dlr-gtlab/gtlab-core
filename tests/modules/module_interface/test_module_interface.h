/* GTlab - Gas Turbine laboratory
 * Source File: test_module_interface
 * copyright 2009-2019 by DLR
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
    Q_PLUGIN_METADATA(IID "de.dlr.gtlab.GtModuleInterface/0.1"
                      FILE "test_module_interface.json")
					  
    Q_INTERFACES(GtModuleInterface)

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
     * @brief Initializes module. Called on application startup.
     */
    void init() override;

};

#endif // TEST_MODULE_INTERFACE_H
 
