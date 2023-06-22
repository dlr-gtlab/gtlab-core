/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef TEST_GT_ENUMPROPERTY_H
#define TEST_GT_ENUMPROPERTY_H

#include <QObject>

class EnumContainer : public QObject
{
    Q_OBJECT
public:
    enum TestEnum {
        A = -1,
        B = 3,
        C
    };
    Q_ENUM(TestEnum)
};

#endif // TEST_GT_ENUMPROPERTY_H
