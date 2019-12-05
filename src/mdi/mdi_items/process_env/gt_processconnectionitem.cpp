/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionitem.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>
#include <QIcon>

#include "gt_abstractproperty.h"
#include "gt_monitoringproperty.h"
#include "gt_calculator.h"
#include "gt_calculatorfactory.h"
#include "gt_extendedcalculatordata.h"
#include "gt_application.h"
#include "gt_doublemonitoringproperty.h"
#include "gt_intmonitoringproperty.h"
#include "gt_objectui.h"
#include "gt_processconnectionmodel.h"
#include "gt_stringproperty.h"
#include "gt_objectlinkproperty.h"

#include "gt_processconnectionitem.h"

QStringList GtProcessConnectionItem::m_acceptedPropertyTypes =
        QStringList() << GT_CLASSNAME(GtDoubleMonitoringProperty) <<
                         GT_CLASSNAME(GtIntMonitoringProperty) <<
                         GT_CLASSNAME(GtDoubleProperty) <<
                         GT_CLASSNAME(GtIntProperty) <<
                         GT_CLASSNAME(GtObjectLinkProperty) <<
                         GT_CLASSNAME(GtStringProperty);

GtProcessConnectionItem::GtProcessConnectionItem(GtProcessComponent* comp) :
    m_component(comp),
    m_property(Q_NULLPTR),
    m_type(GtProcessConnectionItem::PROCESS_COMPONENT)
{
}

GtProcessConnectionItem::GtProcessConnectionItem(GtAbstractProperty* prop) :
    m_component(Q_NULLPTR),
    m_property(prop)
{
    if (dynamic_cast<GtMonitoringProperty*>(prop))
    {
        m_type = GtProcessConnectionItem::MONITORING_PROPERTY;
    }
    else
    {
        m_type = GtProcessConnectionItem::DEFAULT_PROPERTY;
    }
}

bool
GtProcessConnectionItem::propertyTypeAccepted(GtAbstractProperty* prop)
{
    const QString className = prop->metaObject()->className();

    return m_acceptedPropertyTypes.contains(className);
}

GtProcessConnectionItem::ItemType
GtProcessConnectionItem::itemType()
{
    return m_type;
}

QVariant
GtProcessConnectionItem::data(int column, int role)
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessConnectionItem::PROCESS_COMPONENT)
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
        }

        case Qt::DecorationRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessConnectionItem::PROCESS_COMPONENT)
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

                    if (qobject_cast<GtDoubleProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("doubleIcon_16.png"));
                    }
                    else if (qobject_cast<GtIntProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("intIcon_16.png"));
                    }
                    else if (qobject_cast<GtObjectLinkProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("oIcon_16.png"));
                    }
                    else if (qobject_cast<GtStringProperty*>(m_property))
                    {
                        return gtApp->icon(QStringLiteral("stringIcon_16.png"));
                    }
                }
            }
        }


    case GtProcessConnectionModel::PropertyRole:
    {
        if (column == 0)
        {
            if (m_type == GtProcessConnectionItem::MONITORING_PROPERTY ||
                    m_type == GtProcessConnectionItem::DEFAULT_PROPERTY)
            {
                if (m_property == Q_NULLPTR)
                {
                    return false;
                }

                return propertyTypeAccepted(m_property);
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

QString
GtProcessConnectionItem::componentUuid()
{
    if (m_type == GtProcessConnectionItem::PROCESS_COMPONENT)
    {
        if (m_component != Q_NULLPTR)
        {
            return m_component->uuid();
        }
    }

    return QString();
}

QString
GtProcessConnectionItem::parentComponentUuid()
{
    // get parent component item
    GtProcessConnectionItem* parentComp =
            qobject_cast<GtProcessConnectionItem*>(parent());

    // check parent component item
    if (parentComp == Q_NULLPTR)
    {
        return QString();
    }

    return parentComp->componentUuid();
}

QString
GtProcessConnectionItem::propertyId()
{
    if (m_type == GtProcessConnectionItem::MONITORING_PROPERTY ||
            m_type == GtProcessConnectionItem::DEFAULT_PROPERTY)
    {
        if (m_property != Q_NULLPTR)
        {
            return m_property->ident();
        }
    }

    return QString();
}

QVariant
GtProcessConnectionItem::propertyValue()
{
    if (m_type == GtProcessConnectionItem::MONITORING_PROPERTY ||
            m_type == GtProcessConnectionItem::DEFAULT_PROPERTY)
    {
        if (m_property != Q_NULLPTR)
        {
            return m_property->valueToVariant();
        }
    }

    return QVariant();
}

QString
GtProcessConnectionItem::propertyClassName()
{
    if (m_type == GtProcessConnectionItem::MONITORING_PROPERTY ||
            m_type == GtProcessConnectionItem::DEFAULT_PROPERTY)
    {
        if (m_property != Q_NULLPTR)
        {
            return m_property->metaObject()->className();
            //return GT_CLASSNAME(m_property->metaObject()->c);
        }
    }

    return QString();
}

GtProcessConnectionItem*
GtProcessConnectionItem::itemById(const QString& uuid, const QString& propId)
{
    // check uuid
    if (uuid.isEmpty())
    {
        return Q_NULLPTR;
    }

    // check property id
    if (propId.isEmpty())
    {
        return Q_NULLPTR;
    }

    // check item type
    if (m_type != GtProcessConnectionItem::PROCESS_COMPONENT)
    {
        if (propertyId() != propId)
        {
            return Q_NULLPTR;
        }

        GtProcessConnectionItem* parentItem =
                qobject_cast<GtProcessConnectionItem*>(parent());

        if (parentItem == Q_NULLPTR)
        {
            return Q_NULLPTR;
        }

        if (parentItem->componentUuid() == uuid)
        {
            return this;
        }
    }

    // iterate over children
    foreach (GtProcessConnectionItem* child,
             findDirectChildren<GtProcessConnectionItem*>())
    {
        GtProcessConnectionItem* retval = child->itemById(uuid, propId);

        if (retval != Q_NULLPTR)
        {
            return retval;
        }
    }

    return Q_NULLPTR;
}
