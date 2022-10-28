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
    layout()->addWidget(m_label);
}

void
TestPreferencePage::saveSettings(GtSettings & s) const
{
    s.setSetting(moduleSettingPath(GT_MODULENAME(), "testtext"),
                 m_label->text());
}

void
TestPreferencePage::loadSettings(const GtSettings & s)
{
    m_label->setText(
        s.getSetting(moduleSettingPath(GT_MODULENAME(), "testtext")).toString());
}
