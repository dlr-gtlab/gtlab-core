/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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

    bool hasValidationRegExp() override;

    QRegExp validatorRegExp() override;
};

#endif // TEST_MDI_EXT_EXTERNALOBJECTUI_H
