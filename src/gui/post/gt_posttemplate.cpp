/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_posttemplate.h"

GtPostTemplate::GtPostTemplate()
{
    setObjectName("PostTemplate");
}

QList<GtPostTemplateItem*>
GtPostTemplate::items()
{
    return findDirectChildren<GtPostTemplateItem*>();
}

