/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTPOSTACTION_H
#define GTPOSTACTION_H

#include "QAction"

class GtPostAction : public QAction
{
    Q_OBJECT

public:
    GtPostAction();

    GtPostAction(const QString& str, QObject* o);
};

#endif // GTPOSTACTION_H
