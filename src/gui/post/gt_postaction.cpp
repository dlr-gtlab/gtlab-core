/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_postaction.h"

GtPostAction::GtPostAction() :
    QAction(QString(), nullptr)
{

}

GtPostAction::GtPostAction(const QString& str, QObject* o) :
    QAction(str, o)
{

}
