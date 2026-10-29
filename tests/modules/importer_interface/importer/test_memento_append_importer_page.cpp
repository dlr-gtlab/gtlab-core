/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_append_importer_page.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

TestMementoAppendImporterPage::TestMementoAppendImporterPage(
        GtObject* obj,
        TestMementoAppendImporter* importer,
        QWidget* parent) :
    GtAbstractImporterPage(obj),
    m_importer(importer)
{
    if (parent)
    {
        setParent(parent);
    }

    setTitle(tr("Append Memento Options"));

    m_label = new QLabel(tr("Suffix"), this);
    m_suffix = new QLineEdit(importer ? importer->suffix() : QString(), this);

    auto layout = new QHBoxLayout;
    layout->addWidget(m_label);
    layout->addWidget(m_suffix);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);

    connect(m_suffix, &QLineEdit::textChanged, this, [this](const QString& text)
    {
        if (m_importer)
        {
            m_importer->setSuffix(text);
        }
    });
}
