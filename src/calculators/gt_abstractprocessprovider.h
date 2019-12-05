/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractprocessprovider.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROCESSPROVIDER_H
#define GTABSTRACTPROCESSPROVIDER_H

#include "gt_calculators_exports.h"

#include <QObject>
#include <QPointer>

class GtProcessComponent;
class GtObjectMemento;

/**
 * @brief The GtAbstractProcessProvider class
 */
class GT_CALCULATORS_EXPORT GtAbstractProcessProvider : public QObject
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
