/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_mdi_package
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TEST_MDI_PACKAGE_H
#define TEST_MDI_PACKAGE_H

#include "test_mdi_interface_exports.h"

#include "gt_intproperty.h"
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

    int mementoExternalValue() const;

private:
    GtIntProperty m_mementoExternalValue;
};

#endif // TEST_MDI_PACKAGE_H
