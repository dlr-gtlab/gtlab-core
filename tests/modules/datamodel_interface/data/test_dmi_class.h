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
#include "gt_doubleproperty.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_propertystructcontainer.h"

/**
 * @brief The TestDmiClass class
 */
class TestDmClass : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestDmClass();

private:
    GtDoubleProperty m_value;
    GtPropertyStructContainer m_container;
    GtModeProperty m_mode;
    GtModeTypeProperty m_propTypeA, m_propTypeB;
};

#endif // TESTDMICLASS_H
