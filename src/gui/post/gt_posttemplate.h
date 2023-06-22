/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTPOSTTEMPLATE_H
#define GTPOSTTEMPLATE_H

#include "gt_object.h"
#include "gt_posttemplateitem.h"

/**
 * @brief The GtPostTemplate class
 */
class GtPostTemplate : public GtObject
{
    Q_OBJECT

public:

    Q_INVOKABLE GtPostTemplate();

    /**
     * @brief items
     * @return
     */
    QList<GtPostTemplateItem*> items();

private:

};

#endif // GTPOSTTEMPLATE_H
