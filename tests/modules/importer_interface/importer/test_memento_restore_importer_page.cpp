/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_restore_importer_page.h"
#include "test_memento_restore_importer.h"

#include <QCheckBox>
#include <QVBoxLayout>

TestMementoRestoreImporterPage::TestMementoRestoreImporterPage(
    GtObject* obj, TestMementoRestoreImporter* importer, QWidget* parent) :
    GtAbstractImporterPage(obj),
    m_importer(importer)
{
    if (parent)
    {
        setParent(parent);
    }

    setTitle(tr("Restore Memento Options"));

    m_keepName = new QCheckBox(tr("Keep object name"), this);
    m_keepName->setChecked(importer && importer->keepObjectName());

    m_changeUuid = new QCheckBox(tr("Change UUID"), this);
    m_changeUuid->setChecked(importer && importer->changeUuid());

    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_keepName);
    layout->addWidget(m_changeUuid);

    connect(m_keepName, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_importer)
        {
            m_importer->setKeepObjectName(checked);
        }
    });

    connect(m_changeUuid, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_importer)
        {
            m_importer->setChangeUuid(checked);
        }
    });
}
