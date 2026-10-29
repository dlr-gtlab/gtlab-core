/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_restore_importer.h"
#include "test_memento_append_importer.h"

#include "importer_interface.h"

GtVersionNumber
TestImporterInterface::version()
{
    return GtVersionNumber(1, 0, 0);
}

QString
TestImporterInterface::description() const
{
    return "Test Importer Interface";
}

QList<QMetaObject>
TestImporterInterface::importer() const
{
    return {GT_METADATA(TestMementoRestoreImporter),
            GT_METADATA(TestMementoAppendImporter)};
}
