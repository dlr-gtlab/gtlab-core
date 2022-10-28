#include "test_preferencepage.h"

#include <QLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include "gt_settings.h"

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
    s.setSetting("datamodel/testtext", m_label->text());
}

void
TestPreferencePage::loadSettings(const GtSettings & s)
{
    m_label->setText(s.getSetting("datamodel/testtext").toString());
}
