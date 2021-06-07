/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_package
 * copyright 2009-2019 by DLR
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#ifndef TEST_DMI_PACKAGE_H
#define TEST_DMI_PACKAGE_H

#include "test_datamodel_interface_exports.h"

#include "gt_package.h"

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

};

#endif // TEST_DMI_PACKAGE_H
