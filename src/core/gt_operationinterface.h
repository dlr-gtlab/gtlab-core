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

    /**
     *  Returns executable operation types contributed by this module.
     *
     * Each meta object must describe a reconstructable GtExecutableOperation
     * with an invokable constructor. The normal module loader validates the
     * declaration and registers valid types in GtObjectFactory. Module authors
     * must not perform a separate operation registration step.
     */
    virtual QList<QMetaObject> operations() const { return {}; }
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtOperationInterface,
                    "de.dlr.gtlab.GtOperationInterface/0.1")
QT_END_NAMESPACE

#endif // GTOPERATIONINTERFACE_H
