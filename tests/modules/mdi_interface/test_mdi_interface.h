/* GTlab - Gas Turbine laboratory
 * Source File: test_mdi_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TEST_DATAMODEL_INTERFACE_H
#define TEST_DATAMODEL_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_datamodelinterface.h"
#include "gt_mdiinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestMdiInterface class
 */
class TestMdiInterface : public QObject,
        public GtModuleInterface,
        public GtDatamodelInterface,
        public GtMdiInterface
 {
    Q_OBJECT

    GT_MODULE("test_mdi_interface.json")

    Q_INTERFACES(GtDatamodelInterface)
    Q_INTERFACES(GtMdiInterface)


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

    /**
     * @brief Returns static meta objects of mdi item classes.
     * @return list including meta objects
     */
    QList<QMetaObject> mdiItems() override;

    /**
     * @brief Returns static meta objects of dockwidget classes.
     * @return list including meta objects
     */
    QList<QMetaObject> dockWidgets() override;

    /**
     * @brief Returns static meta objects of object user interface classes
     * combined with the name of the target class.
     * @return
     */
    QMap<const char*, QMetaObject> uiItems() override;

};

#endif // TEST_DATAMODEL_INTERFACE_H
 
