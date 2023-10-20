/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_mdi_dm_class
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TESTMDIDMCLASS_H
#define TESTMDIDMCLASS_H

#include "gt_object.h"

/**
 * @brief The TestMdiDmClass class
 */
class TestMdiData : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMdiData();

};

#endif // TESTMDIDMCLASS_H
