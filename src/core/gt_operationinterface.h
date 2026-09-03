/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTOPERATIONINTERFACE_H
#define GTOPERATIONINTERFACE_H

#include "gt_core_exports.h"

#include <QMetaObject>
#include <QtPlugin>

/**
 * @brief Optional module declaration for executable operation types.
 */
class GT_CORE_EXPORT GtOperationInterface
{
public:
    virtual ~GtOperationInterface() = default;

    virtual QList<QMetaObject> operations() const { return {}; }
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtOperationInterface,
                    "de.dlr.gtlab.GtOperationInterface/0.1")
QT_END_NAMESPACE

#endif // GTOPERATIONINTERFACE_H
