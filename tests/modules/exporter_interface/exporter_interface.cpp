/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_exporter.h"

#include "exporter_interface.h"

GtVersionNumber
TestExporterInterface::version()
{
    return GtVersionNumber(1, 0, 0);
}

QString
TestExporterInterface::description() const
{
    return "Test Exporter Interface";
}

QList<QMetaObject>
TestExporterInterface::exporter() const
{
    return {GT_METADATA(TestMementoExporter)};
}
