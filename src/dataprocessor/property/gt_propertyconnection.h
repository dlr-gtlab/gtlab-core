/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyconnection.h
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYCONNECTION_H
#define GTPROPERTYCONNECTION_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

#include <QPointer>

class GtAbstractProperty;

/**
 * @brief The GtPropertyConnection class
 */
class GT_DATAMODEL_EXPORT GtPropertyConnection : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QString sourceUuid READ sourceUuid WRITE setSourceUuid)
    Q_PROPERTY(QString targetUuid READ targetUuid WRITE setTargetUuid)
    Q_PROPERTY(QString sourceProp READ sourceProp WRITE setSourceProp)
    Q_PROPERTY(QString targetProp READ targetProp WRITE setTargetProp)

public:
    /**
     * @brief GtPropertyConnection
     */
    Q_INVOKABLE GtPropertyConnection();

    /**
     * @brief Returns UUID of source object.
     * @return UUID of source object.
     */
    QString sourceUuid() const;

    /**
     * @brief Sets UUID of source object.
     * @param New UUID of source object.
     */
    void setSourceUuid(const QString& sourceUuid);

    /**
     * @brief Returns UUID of target object.
     * @return UUID of target object.
     */
    QString targetUuid() const;

    /**
     * @brief Sets UUID of target object.
     * @param New UUID of target object.
     */
    void setTargetUuid(const QString& targetUuid);

    /**
     * @brief Returns identification string of source object property.
     * @return Identification string of source object property.
     */
    QString sourceProp() const;

    /**
     * @brief Sets identification string of source object property.
     * @param Identification string of source object property.
     */
    void setSourceProp(const QString& sourceProp);

    /**
     * @brief Returns identification string of target object property.
     * @return Identification string of target object property.
     */
    QString targetProp() const;

    /**
     * @brief Sets identification string of target object property.
     * @param Identification string of target object property.
     */
    void setTargetProp(const QString& targetProp);

    /**
     * @brief Creates connection based on source and target definitions.
     */
    void makeConnection();

    /**
     * @brief Returns value of source property inf orm of QVariant.
     * @return Value from source.
     */
    QVariant valueFromSource() const;

    /**
     * @brief Returns true if connection is already connected.
     * @return Whether connetion is connected or not.
     */
    bool isConnected();

private:
    /// Source object uuid.
    QString m_sourceUuid;

    /// Target object uuid.
    QString m_targetUuid;

    /// Source object property identification string.
    QString m_sourceProp;

    /// Target object property identification string.
    QString m_targetProp;

    /// Pointer to source property.
    QPointer<GtAbstractProperty> m_sourcePropObj;

    /// Pointer to target property.
    QPointer<GtAbstractProperty> m_targetPropObj;

private slots:
    /**
     * @brief Called on change of source property.
     */
    void onSourcePropertyChange();

signals:
    /**
     * @brief Emitted to trigger value transfer from source to target object.
     */
    void triggerValueTransfer();

};

#endif // GTPROPERTYCONNECTION_H
