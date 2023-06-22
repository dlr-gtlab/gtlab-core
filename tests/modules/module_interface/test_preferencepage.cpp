/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 *  E-Mail: martin.siggel@dlr.de
 */

#include "test_preferencepage.h"

#include <QLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include "gt_settings.h"

#include "gt_moduleinterface.h"

TestPreferencePage::TestPreferencePage()
    : GtPreferencesPage("Module Interface")
{
    setTitleShort("Mod.Interface");
    m_label = new QLineEdit;
    auto layout = new QVBoxLayout;
    layout->addWidget(m_label);
    layout->addStretch();
    setLayout(layout);
}

void
TestPreferencePage::saveSettings(GtSettings& s) const
{
    assert(s.hasSetting(gt::makeModuleSettingsPath("testtext")));
    s.setModuleSetting("testtext", m_label->text());
}

void
TestPreferencePage::loadSettings(const GtSettings& s)
{
    m_label->setText(s.getModuleSetting("testtext").toString());
}
