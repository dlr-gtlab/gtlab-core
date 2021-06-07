/* GTlab - Gas Turbine laboratory
 * Source File: test_mdi_dm_class
 * copyright 2009-2019 by DLR
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
