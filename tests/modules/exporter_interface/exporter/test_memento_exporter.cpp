/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_exporter.h"
#include "test_memento_exporter_page.h"

#include "test_dmi_externalobject.h"

#include "gt_objectmemento.h"
#include "gt_icons.h"
#include "gt_selectfiletoexportpage.h"
#include "gt_logging.h"

#include <QIcon>
#include <QFile>

TestMementoExporter::TestMementoExporter()
{
    setObjectName(tr("Memento Exporter"));
}

QStringList
TestMementoExporter::targets() const
{
    return {GT_CLASSNAME(TestDmiExternalObject), QStringLiteral("TestDmiData")};
}

QIcon
TestMementoExporter::icon() const
{
    return gt::gui::icon::export_();
}

bool
TestMementoExporter::doExport(GtObject* data, QFile& file)
{
    if (!data)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open file!") << file.fileName();
        return false;
    }

    file.write(data->toMemento(m_preserveUuids).toByteArray());

    file.close();

    gtInfo() << "Memento export finished";

    return true;
}

QString
TestMementoExporter::extension() const
{
    return QStringLiteral("XML (*.xml)");
}

QList<GtAbstractExporterPage*>
TestMementoExporter::pages(GtObject* data)
{
    auto selectPage = new GtSelectFileToExportPage(data, this);
    selectPage->setFileExtension(extension());

    return {new TestMementoExporterPage(data, this), selectPage};
}

void
TestMementoExporter::setPreserveUuids(bool preserve)
{
    m_preserveUuids = preserve;
}

bool
TestMementoExporter::preserveUuids() const
{
    return m_preserveUuids;
}
