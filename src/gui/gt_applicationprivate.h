/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_applicationprivate.h
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_APPLICATIONPRIVATE_H
#define GT_APPLICATIONPRIVATE_H

#include <QObject>
#include <QPointer>

#include "gt_objectmemento.h"

/**
 * @brief The GtApplicationPrivate class
 */
class GtApplicationPrivate : public QObject
{
public:
    /**
     * @brief GtApplicationPrivate
     */
    explicit GtApplicationPrivate(QObject* parent = nullptr);

    /// Command object memento
    GtObjectMemento m_commandMemento;

    /// Root object for command
    QPointer<GtObject> m_commandRoot;

    /// Command id
    QString m_commandId;

    /// Command identification string
    QString m_commandUuid;

};

#endif // GT_APPLICATIONPRIVATE_H
