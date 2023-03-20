/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TEST_MDI_EXT_EXTERNALOBJECTUI_H
#define TEST_MDI_EXT_EXTERNALOBJECTUI_H

#include "gt_objectui.h"

class TestMdiExtExternalObjectUI : public GtObjectUI
{
    Q_OBJECT

public:

    Q_INVOKABLE TestMdiExtExternalObjectUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

    static void createSibling(GtObject* obj);

    static void modifyValues(GtObject* obj);

    static void modifyParams(GtObject* obj);

    static void addData(GtObject* obj);

    QStringList openWith(GtObject*obj) override;
};

#endif // TEST_MDI_EXT_EXTERNALOBJECTUI_H
