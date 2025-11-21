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

    propElement.setAttribute(xml::S_TYPE_TAG, newType);
    QDomNode n = propElement.firstChild();
    if (!n.isNull() && !n.isText())
    {
        // Create new text node after removing old children
        while (propElement.hasChildNodes())
        {
            propElement.removeChild(propElement.firstChild());
        }
        QDomText t = propElement.ownerDocument().createTextNode(newValue);
        propElement.appendChild(t);
    }
    else if (n.isNull())
    {
        QDomText t = propElement.ownerDocument().createTextNode(newValue);
        propElement.appendChild(t);
    }
    else
    {
        n.setNodeValue(newValue);
    }

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

