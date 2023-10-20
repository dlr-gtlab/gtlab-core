/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_mdi_package
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TEST_DMI_PACKAGE_H
#define TEST_DMI_PACKAGE_H

#include "test_mdi_interface_exports.h"

#include "gt_package.h"

/**
 * @brief The TestMdiDmPackage class
 */
class TEST_MDI_INTERFACE_EXPORT TestMdiPackage : public GtPackage
{
    Q_OBJECT

public:

    /**
      * @brief Constructor.
      */
    Q_INVOKABLE TestMdiPackage();

};

#endif // TEST_DMI_PACKAGE_H
