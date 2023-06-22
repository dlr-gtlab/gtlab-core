/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 7.10.2022
 *  Author: Martin Siggel (AT-TWK)
 *  E-Mail: martin.siggel@dlr.de
 */

#ifndef TEST_ISSUE358_H
#define TEST_ISSUE358_H

#include <QObject>

#include <gt_object.h>
#include <gt_doubleproperty.h>

/**
 * Just a helper object required to
 * reproduce issue 358
 */
class TestObject358 : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TestObject358();

    bool parentExistedOnValueChanged() const
    {
        return parentExist;
    }

    void setValue(double v)
    {
        dp.setVal(v);
    }

    double value() const
    {
        return dp.getVal();
    }

    bool valueHasChanged{false};

private:
    GtDoubleProperty dp;
    bool parentExist{false};

};

#endif // TEST_ISSUE358_H
