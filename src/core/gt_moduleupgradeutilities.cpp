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

bool
gt::module_upgrade::convertPropertyContainerToMap(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap)
{
    if (container.isNull()) return false;

    if (container.tagName() != xml::S_PROPERTYCONT_TAG) return false;

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

    return true;
}



