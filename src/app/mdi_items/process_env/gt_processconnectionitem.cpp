/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processconnectionitem.cpp
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
#include "gt_stringmonitoringproperty.h"
#include "gt_intmonitoringproperty.h"
#include "gt_icons.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_objectui.h"
#include "gt_processconnectionmodel.h"
#include "gt_stringproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_propertyreference.h"

#include "gt_processconnectionitem.h"

QStringList GtProcessConnectionItem::m_acceptedPropertyTypes =
        QStringList() << GT_CLASSNAME(GtDoubleMonitoringProperty) <<
                         GT_CLASSNAME(GtIntMonitoringProperty) <<
                         GT_CLASSNAME(GtDoubleProperty) <<
                         GT_CLASSNAME(GtIntProperty) <<
                         GT_CLASSNAME(GtObjectLinkProperty) <<
                         GT_CLASSNAME(GtStringProperty) <<
                         GT_CLASSNAME(GtStringMonitoringProperty);

GtProcessConnectionItem::GtProcessConnectionItem(GtProcessComponent* comp) :
    m_component(comp),
    m_type(GtProcessConnectionItem::PROCESS_COMPONENT)
{
}

GtProcessConnectionItem::GtProcessConnectionItem(GtAbstractProperty* prop) :
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

GtProcessConnectionItem::GtProcessConnectionItem(GtPropertyStructContainer& conp) :
    m_container(&conp),
    m_type(GtProcessConnectionItem::PROPERTY_CONTAINER)
{
}

GtProcessConnectionItem::GtProcessConnectionItem(GtPropertyStructContainer &con,
                                                 GtPropertyStructInstance& entry) :
    m_container(&con),
    m_containerEntry(&entry),
    m_type(GtProcessConnectionItem::CONTAINER_ENTRY)
{
}



GtProcessConnectionItem::GtProcessConnectionItem(GtProcessComponent& comp,
                                                 GtPropertyStructContainer& con,
                                                 GtPropertyStructInstance& entry,
                                                 GtAbstractProperty& prop) :
    m_component(&comp),
    m_property(&prop),
    m_container(&con),
    m_containerEntry(&entry),
    m_type(GtProcessConnectionItem::DEFAULT_PROPERTY)
{
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
                    if (!m_component)
                    {
                        return QVariant();
                    }

                    return m_component->objectName();
                }

                if (m_type == GtProcessConnectionItem::PROPERTY_CONTAINER)
                {
                    if (!m_container)
                    {
                        return QVariant();
                    }

                    return m_container->name();
                }

                if (m_type == GtProcessConnectionItem::CONTAINER_ENTRY)
                {
                    if (!m_containerEntry || !m_container)
                    {
                        return QVariant();
                    }

                    auto iter = m_container->findEntry(m_containerEntry->ident());

                    // cppcheck-suppress assertWithSideEffect
                    assert(iter != m_container->end());

                    return QStringLiteral("[%1]").arg(std::distance(m_container->begin(), iter));
                }

                if (!m_property)
                {
                    return QVariant();
                }

                return m_property->objectName();

            }
            break;
        }

        case Qt::DecorationRole:
        {
            if (column == 0)
            {
                if (m_type == GtProcessConnectionItem::PROCESS_COMPONENT)
                {
                    if (!m_component)
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

                        if (eData)
                        {
                            return eData->icon;
                        }

                        return gt::gui::icon::calculator();

                    }
                    else
                    {
                        GtObjectUI* oui = gtApp->defaultObjectUI(m_component);

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
                            GtObjectUI oui2;
                            QVariant value = oui2.data(m_component, role,
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
                    if (!m_property)
                    {
                        if (itemType() == PROPERTY_CONTAINER)
                        {
                            return gt::gui::icon::list();
                        }
                        else if (itemType() == CONTAINER_ENTRY)
                        {
                            return gt::gui::icon::arrowDown();
                        }

                        return QVariant();
                    }

                    if (qobject_cast<GtDoubleProperty*>(m_property))
                    {
                        return gt::gui::icon::letter::d();
                    }

                    if (qobject_cast<GtIntProperty*>(m_property))
                    {
                        return gt::gui::icon::letter::i();
                    }

                    if (qobject_cast<GtObjectLinkProperty*>(m_property))
                    {
                        return gt::gui::icon::letter::o();
                    }

                    if (qobject_cast<GtStringProperty*>(m_property))
                    {
                        return gt::gui::icon::letter::s();
                    }
                }

            }
            break;
        }


    case GtProcessConnectionModel::PropertyRole:
    {
        if (column == 0)
        {
            if (m_type == GtProcessConnectionItem::MONITORING_PROPERTY ||
                    m_type == GtProcessConnectionItem::DEFAULT_PROPERTY)
            {
                if (!m_property)
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
        if (m_component)
        {
            return m_component->uuid();
        }
    }

    return QString();
}

QString
GtProcessConnectionItem::parentComponentUuid()
{
    if (m_component)
    {
        return m_component->uuid();
    }

    // get parent component item
    GtProcessConnectionItem* parentComp =
            qobject_cast<GtProcessConnectionItem*>(parent());

    // check parent component item
    if (!parentComp)
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
        if (m_containerEntry)
        {
            assert(m_property);
            assert(m_container);
            return GtPropertyReference(m_container->ident(),
                                       m_containerEntry->ident(),
                                       m_property->ident()).toString();
        }

        if (m_property)
        {
            return GtPropertyReference(m_property->ident()).toString();
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
        if (m_property)
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
        if (m_property)
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
        return nullptr;
    }

    // check property id
    if (propId.isEmpty())
    {
        return nullptr;
    }

    // check item type
    if (m_type != GtProcessConnectionItem::PROCESS_COMPONENT)
    {
        if (propertyId() != propId)
        {
            return nullptr;
        }

        GtProcessConnectionItem* parentItem =
                qobject_cast<GtProcessConnectionItem*>(parent());

        if (!parentItem)
        {
            return nullptr;
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

        if (retval)
        {
            return retval;
        }
    }

    return nullptr;
}
