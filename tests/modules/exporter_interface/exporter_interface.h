/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_EXPORTER_INTERFACE_H
#define TEST_EXPORTER_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_exporterinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestExporterInterface class
 *
 * Test module for the exporter interface. Does not own a data model,
 * its exporters target classes of the test data model interface.
 */
class TestExporterInterface : public QObject,
                              public GtModuleInterface,
                              public GtExporterInterface
{
    Q_OBJECT

    GT_MODULE("exporter_interface.json")

    Q_INTERFACES(GtModuleInterface)
    Q_INTERFACES(GtExporterInterface)

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
     * @brief Returns static meta objects of exporter classes.
     * @return List including meta objects of exporter classes.
     */
    QList<QMetaObject> exporter() const override;
};

#endif // TEST_EXPORTER_INTERFACE_H
