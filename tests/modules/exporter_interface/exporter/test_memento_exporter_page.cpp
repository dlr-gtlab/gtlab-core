/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_exporter_page.h"

#include <QCheckBox>
#include <QVBoxLayout>

TestMementoExporterPage::TestMementoExporterPage(GtObject* obj,
                                                 TestMementoExporter* exporter,
                                                 QWidget* parent) :
    GtAbstractExporterPage(obj),
    m_exporter(exporter)
{
    if (parent)
    {
        setParent(parent);
    }

    setTitle(tr("Memento Export Options"));

    m_preserveUuids = new QCheckBox(tr("Preserve UUIDs"), this);
    m_preserveUuids->setChecked(exporter && exporter->preserveUuids());

    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_preserveUuids);

    connect(m_preserveUuids, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_exporter)
        {
            m_exporter->setPreserveUuids(checked);
        }
    });
}
