/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 *  E-Mail: martin.siggel@dlr.de
 */

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
