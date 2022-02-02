/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_ext_packageui.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TESTDMIEXTPACKAGEUI_H
#define TESTDMIEXTPACKAGEUI_H

#include "gt_objectui.h"

/**
 * @brief The TestDmiExtPackageUI class
 */
class TestMdiExtPackageUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMdiExtPackageUI();

    /**
     * @brief Test that the package icon will not be changed
     * @param obj
     * @return invalid QIcon
     */
    QIcon icon(GtObject *obj) const override;

    /**
     * @brief Test for open with interface. Adds two entries
     * (Opener Ext 1, Opener Ext 2).
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) override;

    /**
     * @brief Test that this doubleClicked function is not called
     * @param obj
     * @return
     */
    void doubleClicked(GtObject *obj) override;

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
    void testGroupAction(GtObject* obj);

};

#endif // TESTDMIEXTPACKAGEUI_H
