/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionitem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCONNECTIONITEM_H
#define GTPROCESSCONNECTIONITEM_H

#include "gt_object.h"

#include <QPointer>
#include <QStringList>

class GtProcessComponent;
class GtAbstractProperty;
class GtProcessConnectionModel;
class GtPropertyStructInstance;

/**
 * @brief The GtProcessConnectionItem class
 */
class GtProcessConnectionItem : public GtObject
{
    Q_OBJECT

    friend class GtProcessConnectionModel;

public:
    enum ItemType {
        PROCESS_COMPONENT = 0,
        MONITORING_PROPERTY,
        DEFAULT_PROPERTY,
        PROPERTY_CONTAINER,
        CONTAINER_ENTRY
    };

    /**
     * @brief Returns type of process connection item.
     * @return Type of process connection item.
     */
    GtProcessConnectionItem::ItemType itemType();

    /**
     * @brief  Returns role dependent data for given column.
     * @param Column.
     * @param Data role.
     * @return Data based on given role.
     */
    QVariant data(int column, int role = Qt::DisplayRole);

    /**
     * @brief Returns component UUID. If item is a property empty string is
     * returned.
     * @return Component UUID.
     */
    QString componentUuid();

    /**
     * @brief Returns component UUID of parent item.
     * If item is a property empty string is returned.
     * @return Parent component UUID.
     */
    QString parentComponentUuid();

    /**
     * @brief Returns property identification string. If item is a process
     * component empty string is returned.
     * @return Property identification string.
     */
    QString propertyId();

    /**
     * @brief Returns property value. If item is a process component invalid
     * QVariant is returned.
     * @return Property value.
     */
    QVariant propertyValue();

    /**
     * @brief Returns property classname. If item is a process component empty
     * QString is returned.
     * @return Property value.
     */
    QString propertyClassName();

    /**
     * @brief Returns process connection item corresponding to given
     * identification strings.
     * @param UUID of component.
     * @param Property identification string.
     * @return Process connection item.
     */
    GtProcessConnectionItem* itemById(const QString& uuid,
                                      const QString& propId);

private:
    /// Pointer to process component
    QPointer<GtProcessComponent> m_component {nullptr};

    /// Pointer to property
    QPointer<GtAbstractProperty> m_property {nullptr};

    /// Pointer to property
    QPointer<GtPropertyStructContainer> m_container {nullptr};

    QPointer<GtPropertyStructInstance> m_containerEntry {nullptr};

    /// Type of process monitoring item
    GtProcessConnectionItem::ItemType m_type;

    /// List of all accepted property types
    static QStringList m_acceptedPropertyTypes;

    /**
     * @brief Constructor.
     * @param Process component.
     */
    explicit GtProcessConnectionItem(GtProcessComponent* comp);

    /**
     * @brief Constructor.
     * @param Process component property.
     */
    explicit GtProcessConnectionItem(GtAbstractProperty* prop);

    /**
     * @brief Constructor.
     * @param Process component property.
     */
    explicit GtProcessConnectionItem(GtPropertyStructContainer& con);

    /**
     * @brief Constructor.
     * @param Process component property.
     */
    explicit GtProcessConnectionItem(GtPropertyStructContainer& con,
                                     GtPropertyStructInstance& entry);

    /**
     * @brief Constructor.
     * @param Process component property.
     */
    explicit GtProcessConnectionItem(GtProcessComponent& comp,
                                     GtPropertyStructContainer& con,
                                     GtPropertyStructInstance& entry,
                                     GtAbstractProperty& prop);

    /**
     * @brief Returns true if property type is accepted, otherwise returns
     * falseö
     * @param Process component property.
     * @return Whether property is accepted or not.
     */
    static bool propertyTypeAccepted(GtAbstractProperty* prop);

};

#endif // GTPROCESSCONNECTIONITEM_H
