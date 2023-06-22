/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractprocessprovider.h
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROCESSPROVIDER_H
#define GTABSTRACTPROCESSPROVIDER_H

#include "gt_core_exports.h"

#include <QObject>
#include <QPointer>

class GtProcessComponent;
class GtObjectMemento;

/**
 * @brief The GtAbstractProcessProvider class
 */
class GT_CORE_EXPORT GtAbstractProcessProvider : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief componentData
     * @return
     */
    GtObjectMemento componentData();

    /**
     * @brief setComponentData
     * @param memento
     * @return
     */
    bool setComponentData(const GtObjectMemento& memento);

    /**
     * @brief initComponent
     * @param metaObj
     * @return
     */
    bool initComponent(const QMetaObject& metaObj);

    /**
     * @brief initComponent
     * @param metaObj
     * @return
     */
    bool initComponent(const QMetaObject* metaObj);

    /**
     * @brief Returns whether component is already initialiazed.
     * @return Init inidcator.
     */
    bool componentInitialized();

    /**
     * @brief componentClassName
     * @return
     */
    QString componentClassName();

protected:
    /// Process component
    QPointer<GtProcessComponent> m_component;

    /**
     * @brief Constructor.
     */
    GtAbstractProcessProvider();

    /**
     * @brief initComponent
     * @param classname
     * @return
     */
    bool initComponent(const QString& classname);

};

#endif // GTABSTRACTPROCESSPROVIDER_H
