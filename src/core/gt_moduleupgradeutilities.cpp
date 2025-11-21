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
gt::module_upgrade::normalizePropertyContainerId(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap)
{
    QDomNode child = container.firstChild();

    while (!child.isNull())
    {
        if (child.isElement())
        {
            QDomElement prop = child.toElement();

            if (prop.tagName() == xml::S_PROPERTY_TAG)
            {
                QString oldUUID = prop.attribute(xml::S_NAME_TAG);

                // Search for sub element <property name="$$formerNameKey$$">NewName</property>
                QDomElement nameElem;
                QDomNode sub = prop.firstChild();

                while (!sub.isNull())
                {
                    if (sub.isElement())
                    {
                        QDomElement e = sub.toElement();
                        if (e.attribute(xml::S_NAME_TAG) == formerNameKey)
                        {
                            nameElem = e;
                            break;
                        }
                    }
                    sub = sub.nextSibling();
                }

                if (!nameElem.isNull())
                {
                    QString newName = nameElem.text().trimmed();

                    // keep connection info
                    replaceMap.insert(oldUUID, newName);

                    // Replace UUID in name attribute
                    prop.setAttribute(xml::S_NAME_TAG, newName);

                    // Remove subelement
                    prop.removeChild(nameElem);
                }
            }
        }

        child = child.nextSibling();
    }
}

QVector<QDomElement>
gt::module_upgrade::findElementsByClass(
    const QDomNode& node,
    const QStringList& classNames,
    SearchMode searchMode)
{
    return findElementsByAttribute(node, xml::S_CLASS_TAG,
                                   classNames, searchMode);
}

QVector<QDomElement>
gt::module_upgrade::findElementsByAttribute(
    const QDomNode& node,
    const QString& attribute,
    const QStringList& values,
    SearchMode searchMode)
{
    QVector<QDomElement> results;

    // *** Root check ***
    if (node.isElement())
    {
        QDomElement rootElem = node.toElement();

        const QString attrValue = rootElem.attribute(attribute);
        if (values.contains(attrValue))
        {
            results.append(rootElem);

            // Root matches & nested not allowed → done.
            if (searchMode == SearchMode::NonRecursive)
            {
                return results;
            }
        }
    }

    // internal recursive function
    auto recurse = [&](const QDomNode& n, auto&& recurseRef) -> void
    {
        for (QDomNode child = n.firstChild(); !child.isNull(); child = child.nextSibling())
        {
            if (child.isElement())
            {
                QDomElement elem = child.toElement();
                const QString attrValue = elem.attribute(attribute);

                if (values.contains(attrValue))
                {
                    results.append(elem);

                    if (searchMode == SearchMode::NonRecursive)
                    {
                        continue;  // IMPORTANT: skip children
                    }
                }
            }

            recurseRef(child, recurseRef);
        }
    };

    recurse(node, recurse);
    return results;
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

// double
// gt::module_upgrade::doubleValue(
//     const QDomElement& parent, const QString& propName)
// {
//     QDomElement propElement = xml::findPropertyElement(parent, propName);

//     QString text = propElement.text();

//     bool ok = false;
//     double val = text.toDouble(&ok);

//     if (text.isEmpty() || !ok)
//     {
//         gtWarningId("Module-Upgrader")
//             << QObject::tr("Cannot read value from an empty text for property %1").arg(propName);
//         return 0.0;
//     }

//     return val;
// }
