/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_packageui.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TESTDMIPACKAGEUI_H
#define TESTDMIPACKAGEUI_H

#include "gt_objectui.h"

/**
 * @brief The TestDmiPackageUI class
 */
class TestDmiPackageUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE TestDmiPackageUI();

    /**
     * @brief Test for open with interface. Adds two entries
     * (Opener 1, Opener 2).
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) Q_DECL_OVERRIDE;

public slots:
    /**
     * @brief Test for specific context menu action.
     * @param obj
     */
    void testAction(GtObject* obj);

    /**
     * @brief Test for specific context menu group action.
     * @param obj
     */
    void testGroupAction1(GtObject* obj);


    /**
     * @brief Second test for specific context menu group action.
     * @param obj
     */
    void testGroupAction2(GtObject* obj);

};

#endif // TESTDMIPACKAGEUI_H
