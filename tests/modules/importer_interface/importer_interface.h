/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_IMPORTER_INTERFACE_H
#define TEST_IMPORTER_INTERFACE_H

#include "gt_moduleinterface.h"
#include "gt_importerinterface.h"
#include "gt_versionnumber.h"

/**
 * @brief The TestImporterInterface class
 *
 * Test module for the importer interface. Does not own a data model, its
 * importers restore mementos that have been exported by the exporter
 * interface test module.
 */
class TestImporterInterface : public QObject,
                              public GtModuleInterface,
                              public GtImporterInterface
{
    Q_OBJECT

    GT_MODULE("importer_interface.json")

    Q_INTERFACES(GtModuleInterface)
    Q_INTERFACES(GtImporterInterface)

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
     * @brief Returns static meta objects of importer classes.
     * @return List including meta objects of importer classes.
     */
    QList<QMetaObject> importer() const override;
};

#endif // TEST_IMPORTER_INTERFACE_H
