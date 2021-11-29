/* GTlab - Gas Turbine laboratory
 * Source File: test_mdi_packageui
 * copyright 2009-2020 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TESTDMIPACKAGEUI_H
#define TESTDMIPACKAGEUI_H

#include "gt_objectui.h"

/**
 * @brief The TestDmiPackageUI class
 */
class TestMdiPackageUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE TestMdiPackageUI();

    /**
     * @brief Icon Test
     * @param obj
     * @return globalIcon.png
     */
    QIcon icon(GtObject *obj) const Q_DECL_OVERRIDE;

    /**
     * @brief Test for open with interface. Adds two entries
     * (Opener 1, Opener 2).
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) Q_DECL_OVERRIDE;

    /**
     * @brief Test for doubleclicked.
     * @param obj
     * @return
     */
    void doubleClicked(GtObject *obj) Q_DECL_OVERRIDE;

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