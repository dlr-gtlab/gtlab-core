/* GTlab - Gas Turbine laboratory
 * Source File: gt_applicationprivate.h
 * copyright 2009-2017 by DLR
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
