/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringitem.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>
#include <QIcon>

#include "gt_calculator.h"
#include "gt_abstractproperty.h"
#include "gt_calculatorfactory.h"
#include "gt_extendedcalculatordata.h"
#include "gt_application.h"
#include "gt_doublemonitoringproperty.h"
#include "gt_intmonitoringproperty.h"
#include "gt_objectui.h"
#include "gt_processmonitoringmodel.h"

#include "gt_processmonitoringitem.h"

GtProcessMonitoringItem::GtProcessMonitoringItem(GtProcessComponent* comp) :
    m_component(comp),
    m_property(Q_NULLPTR),
    m_type(GtProcessMonitoringItem::PROCESS_COMPONENT),
    m_checked(false)
{
    // make connections
    if (comp != Q_NULLPTR)
    {
        connect(comp, SIGNAL(dataChanged(GtObject*)),
                SLOT(onObjectDataChange()));
        connect(comp, SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
                SLOT(onObjectDataChange()));
    }
}

GtProcessMonitoringItem::GtProcessMonitoringItem(GtAbstractProperty* prop) :
    m_component(Q_NULLPTR),
    m_property(prop),
    m_type(GtProcessMonitoringItem::MONITORING_PROPERTY),
    m_checked(false),
    m_color(QColor::fromHsv(qrand() % 256, 255, 190))
{

}

void
GtProcessMonitoringItem::onObjectDataChange()
{
    emit dataChanged(this);
}

GtProcessMonitoringItem::ItemType
GtProcessMonitoringItem::itemType()
{
    return m_type;
}

QVariant
GtProcessMonitoringItem::data(int column, int role)
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessMonitoringItem::PROCESS_COMPONENT)
                {
                    if (m_component == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    return m_component->objectName();
                }
                else
                {
                    if (m_property == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    return m_property->objectName();
                }
            }
            else if (column == 1)
            {
                if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
                {
                    if (m_property == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    return m_property->valueToVariant();
                }
            }
            else if (column == 2)
            {
                if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
                {
                    if (m_property == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    return m_property->siUnit();
                }
            }
        }

        case Qt::DecorationRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessMonitoringItem::PROCESS_COMPONENT)
                {
                    if (m_component == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    if (qobject_cast<GtCalculator*>(m_component))
                    {
                        QString className =
                            m_component->metaObject()->className();
                        GtCalculatorData calcData =
                            gtCalculatorFactory->calculatorData(className);

                        GtExtendedCalculatorDataImpl* eData =
                            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                                calcData.get());

                        if (eData != Q_NULLPTR)
                        {
                            return eData->icon;
                        }
                        else
                        {
                            return gtApp->icon(
                                       QStringLiteral("calculatorIcon_16.png"));
                        }
                    }
                    else
                    {
                        GtObjectUI* oui = gtApp->objectUI(m_component);

                        if (oui)
                        {
                            QVariant value = oui->data(m_component, role,
                                                       column);

                            if (value.isValid())
                            {
                                return value;
                            }
                        }
                        else
                        {
                            GtObjectUI oui;
                            QVariant value = oui.data(m_component, role,
                                                      column);

                            if (value.isValid())
                            {
                                return value;
                            }
                        }
                    }
                }
                else
                {
                    if (m_property == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    if (qobject_cast<GtDoubleMonitoringProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("doubleIcon_16.png"));
                    }
                    else if (qobject_cast<GtIntProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("intIcon_16.png"));
                    }
                }
            }
            else if (column == 3)
            {
                if (m_type == GtProcessMonitoringItem::PROCESS_COMPONENT)
                {
                    if (m_component == Q_NULLPTR)
                    {
                        return QVariant();
                    }

                    switch (m_component->currentState())
                    {
                        case GtProcessComponent::RUNNING:
                            return gtApp->icon(
                                       QStringLiteral("inProgressIcon_16.png"));

                        case GtProcessComponent::QUEUED:
                            return gtApp->icon(
                                       QStringLiteral("sleepIcon_16.png"));

                        case GtProcessComponent::FAILED:
                            return gtApp->icon(
                                       QStringLiteral("processFailedIcon_16.png"));

                        case GtProcessComponent::FINISHED:
                            return gtApp->icon(
                                       QStringLiteral("checkIcon_16.png"));

                        case GtProcessComponent::CONNECTING:
                            return gtApp->icon(
                                       QStringLiteral("networkIcon_16.png"));

                        default:
                            break;
                    }
                }
            }
        }

        case Qt::CheckStateRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
                {
                    if (m_checked)
                    {
                        return Qt::Checked;
                    }

                    return Qt::Unchecked;
                }
                else
                {
                    return QVariant();
                }
            }
        }

        case GtProcessMonitoringModel::PropertyRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    return QVariant();
}

bool
GtProcessMonitoringItem::setData(int column, const QVariant& value, int role)
{
    switch (role)
    {
        case Qt::CheckStateRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
                {
                    m_checked = value.toBool();

                    return true;
                }
            }
        }
    }

    return false;
}

QString
GtProcessMonitoringItem::componentUuid()
{
    if (m_type == GtProcessMonitoringItem::PROCESS_COMPONENT)
    {
        if (m_component != Q_NULLPTR)
        {
            return m_component->uuid();
        }
    }

    return QString();
}

QString
GtProcessMonitoringItem::propertyId()
{
    if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY)
    {
        if (m_property != Q_NULLPTR)
        {
            return m_property->ident();
        }
    }

    return QString();
}

QList<GtProcessMonitoringItem*>
GtProcessMonitoringItem::selectedItems()
{
    // create empty list
    QList<GtProcessMonitoringItem*> retval;

    // check whether item is property type and selected
    if (m_type == GtProcessMonitoringItem::MONITORING_PROPERTY && m_checked)
    {
        retval << this;
    }

    // iterate over children
    foreach (GtProcessMonitoringItem* child,
             findDirectChildren<GtProcessMonitoringItem*>())
    {
        // append selected items of child item
        retval.append(child->selectedItems());
    }

    // return list of selected items
    return retval;
}

const QColor&
GtProcessMonitoringItem::color()
{
    return m_color;
}
