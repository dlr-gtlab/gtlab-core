/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Created on: 17.11.2025
 *  Author: Jens Schmeink (AT-TWK)
 */

#include <QStringList>
#include <QUuid>
#include <QMap>
#include <QObject>

#include <gt_logging.h>
#include "gt_xmlutilities.h"
#include "gt_xmlexpr.h"

#include "gt_moduleupgradeutilities.h"

void
gt::module_upgrade::convertPropertyContainerToMap(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap)
{
    for (auto child = container.firstChild(); !child.isNull(); child = child.nextSibling())
    {
        if (!child.isElement()) continue;

        QDomElement prop = child.toElement();

        if (prop.tagName() != xml::S_PROPERTY_TAG) continue;

        QString oldUUID = prop.attribute(xml::S_NAME_TAG);

        // Search for sub element <property name="$$formerNameKey$$">NewName</property>
        QDomElement nameElem;
        // QDomNode sub = prop.firstChild();

        for (auto sub = prop.firstChild(); !sub.isNull(); sub = child.nextSibling())
        {
            if (!sub.isElement()) continue;

            QDomElement e = sub.toElement();
            if (e.attribute(xml::S_NAME_TAG) == formerNameKey)
            {
                nameElem = e;
                break;
            }
        }

        if (nameElem.isNull()) continue;

        QString newName = nameElem.text().trimmed();

        // keep connection info
        replaceMap.insert(oldUUID, newName);

        // Replace UUID in name attribute
        prop.setAttribute(xml::S_NAME_TAG, newName);

        // Remove subelement
        prop.removeChild(nameElem);
    }
}

bool
gt::module_upgrade::setPropertyTypeAndValue(
    QDomElement& propElement,
    const QString& newType,
    const QString& newValue)
{
    if (propElement.isNull()) return false;

    size_t propChildCount = propElement.childNodes().size();

    if (propChildCount > 1)
    {
        return false;
    }

    QDomNode n = propElement.firstChild();

    // n is the only child and it is a text element
    if (!n.isNull() && n.isText())
    {
        n.setNodeValue(newValue);
    }
    // n is the only child but no text element
    // abort as undefined how to update the value
    else if (!n.isNull())
    {
        return false;
    }
    // n is null
    else
    {
        QDomText t = propElement.ownerDocument().createTextNode(newValue);
        propElement.appendChild(t);
    }

    propElement.setAttribute(xml::S_TYPE_TAG, newType);

    return true;
}

bool
gt::module_upgrade::setPropertyTypeAndValue(
    const QDomElement& root, const QString& name,
    const QString& newType, const QString& newValue)
{
    QDomElement prop = xml::findPropertyElement(root, name);

    return setPropertyTypeAndValue(prop, newType, newValue);
}

