/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_dmi_package_ext
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#ifndef TEST_DMI_EXT_PACKAGE_H
#define TEST_DMI_EXT_PACKAGE_H

#include "gt_package.h"


/**
 * @brief The TestDmiExtPackage class
 */
class TestMdiExtPackage : public GtPackage
{
    Q_OBJECT

public:

    /**
      * @brief TestDmiExtPackage
      */
    Q_INVOKABLE TestMdiExtPackage();

};

#endif // TEST_DMI_EXT_PACKAGE_H
