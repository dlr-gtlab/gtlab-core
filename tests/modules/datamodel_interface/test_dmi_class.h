/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_class.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TESTDMICLASS_H
#define TESTDMICLASS_H

#include "gt_object.h"

/**
 * @brief The TestDmiClass class
 */
class TestDmiClass : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestDmiClass();

};

#endif // TESTDMICLASS_H
