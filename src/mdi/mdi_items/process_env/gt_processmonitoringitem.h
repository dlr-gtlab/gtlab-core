/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringitem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMONITORINGITEM_H
#define GTPROCESSMONITORINGITEM_H

#include "gt_object.h"

#include <QPointer>
#include <QColor>

class GtProcessComponent;
class GtAbstractProperty;
class GtProcessMonitoringModel;

/**
 * @brief The GtProcessMonitoringItem class
 */
class GtProcessMonitoringItem : public GtObject
{
    Q_OBJECT

    friend class GtProcessMonitoringModel;

public:
    enum ItemType {
        PROCESS_COMPONENT = 0,
        MONITORING_PROPERTY
    };

    /**
     * @brief Returns type of process monitoring item.
     * @return Type of process monitoring item.
     */
    GtProcessMonitoringItem::ItemType itemType();

    /**
     * @brief  Returns role dependent data for given column.
     * @param Column.
     * @param Data role.
     * @return Data based on given role.
     */
    QVariant data(int column, int role = Qt::DisplayRole);

    /**
     * @brief setData
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(int column, const QVariant& value,
                         int role = Qt::EditRole);

    /**
     * @brief Returns component UUID. If item is a property empty string is
     * returned.
     * @return Component UUID.
     */
    QString componentUuid();

    /**
     * @brief Returns property identification string. If item is a process
     * component empty string is returned.
     * @return Property identification string.
     */
    QString propertyId();

    /**
     * @brief Returns list of selected monitoring items recursively.
     * @return  List of selected monitoring items.
     */
    QList<GtProcessMonitoringItem*> selectedItems();

    /**
     * @brief Returns item specific color for visualization.
     * @return  Item specific color.
     */
    const QColor& color();

private:
    /// Pointer to process component
    QPointer<GtProcessComponent> m_component;

    /// Pointer to property
    QPointer<GtAbstractProperty> m_property;

    /// Type of process monitoring item
    GtProcessMonitoringItem::ItemType m_type;

    /// Check state
    bool m_checked;

    /// Color for visualization
    QColor m_color;

    /**
     * @brief Constructor.
     * @param Process component.
     */
    GtProcessMonitoringItem(GtProcessComponent* comp);

    /**
     * @brief Constructor.
     * @param Monitoring property.
     */
    GtProcessMonitoringItem(GtAbstractProperty* prop);

private slots:
    /**
     * @brief onObjectDataChange
     */
    void onObjectDataChange();

};

#endif // GTPROCESSMONITORINGITEM_H
