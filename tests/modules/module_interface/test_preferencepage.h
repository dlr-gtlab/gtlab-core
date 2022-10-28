#ifndef TESTPREFERENCEPAGE_H
#define TESTPREFERENCEPAGE_H

#include "gt_preferencespage.h"

#include <QWidget>
#include <QCheckBox>

class TestPreferencePage : public GtPreferencesPage
{
    Q_OBJECT
public:
    explicit TestPreferencePage();

    void saveSettings(GtSettings &) const override;
    void loadSettings(const GtSettings &) override;

private:
    class QLineEdit* m_label;
};

#endif // TESTPREFERENCEPAGE_H
