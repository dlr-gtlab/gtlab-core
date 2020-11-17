/* GTlab - Gas Turbine laboratory
 * Source File: test_datamodel_interface
 * copyright 2009-2019 by DLR
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#ifndef TEST_DATAMODEL_INTERFACE_H
#define TEST_DATAMODEL_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_datamodelinterface.h"
#include "gt_mdiinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestDatamodelInterface class
 */
class TestDatamodelInterface : public QObject,
		public GtModuleInterface,
        public GtDatamodelInterface,
        public GtMdiInterface
 {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.dlr.gtlab.GtModuleInterface/0.1"
                      FILE "test_datamodel_interface.json")
					  
    Q_INTERFACES(GtModuleInterface)
	Q_INTERFACES(GtDatamodelInterface)
    Q_INTERFACES(GtMdiInterface)

public:

	/**
     * @brief Returns current version number of module
     * @return version number
     */
    GtVersionNumber version() Q_DECL_OVERRIDE;

    /**
     * @brief Returns module identification string.
     * @return identification string
     */
    QString ident() const Q_DECL_OVERRIDE;

    /**
     * @brief Returns module description
     * @return description
     */
    QString description() const Q_DECL_OVERRIDE;
	 
    /**
     * @brief Returns static meta objects of datamodel classes.
     * @return list including meta objects
     */
    QList<QMetaObject> data() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of datamodel classes.
     * @return list including meta objects
     */
    QMetaObject package() Q_DECL_OVERRIDE;

    /**
     * @brief Returns true if module is a stand alone module with own
     * data model structure. Otherwise module only extends the overall
     * application with additional functionalities like classes, calculators
     * or graphical user interfaces.
     * @return Stand alone indicator.
     */
    bool standAlone() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of mdi item classes.
     * @return list including meta objects
     */
    QList<QMetaObject> mdiItems() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of dockwidget classes.
     * @return list including meta objects
     */
    QList<QMetaObject> dockWidgets() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of object user interface classes
     * combined with the name of the target class.
     * @return
     */
    QMap<const char*, QMetaObject> uiItems() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of post item classes.
     * @return list including meta objects
     */
    QList<QMetaObject> postItems() Q_DECL_OVERRIDE;

    /**
     * @brief Returns static meta objects of post plot classes.
     * @return list including meta objects
     */
    QList<QMetaObject> postPlots() Q_DECL_OVERRIDE;

};

#endif // TEST_DATAMODEL_INTERFACE_H
 
