/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_dmi_package
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#ifndef TEST_DMI_PACKAGE_H
#define TEST_DMI_PACKAGE_H

#include "test_datamodel_interface_exports.h"

#include "gt_package.h"
#include "gt_objectlinkproperty.h"

/**
 * @brief The TestDmiPackage class
 */
class TEST_DATAMODEL_INTERFACE_EXPORT TestDmiPackage : public GtPackage
{
    Q_OBJECT

public:

    /**
      * @brief Constructor.
      */
    Q_INVOKABLE TestDmiPackage();

private:
    GtObjectLinkProperty m_link;

};

#endif // TEST_DMI_PACKAGE_H
