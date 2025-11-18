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

#include "gt_moduleupgradeutilities.h"

namespace {
    constexpr auto CLASS_STRING = "class";
    constexpr auto PROP_STRING = "property";
    constexpr auto OBJ_STRING = "object";
    constexpr auto NAME_STRING = "name";
    constexpr auto TYPE_STRING = "type";
}


QVector<QDomElement> gt::module_upgrade::findElementsByClass(
    const QDomNode& node,
    const QStringList& classNames,
    SearchMode searchMode)
{
    return findElementsByAttribute(node, CLASS_STRING,
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
            if (searchMode == SearchMode::DisallowNested)
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

                    if (searchMode == SearchMode::DisallowNested)
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

QDomElement
gt::module_upgrade::findParentByAttribute(
    const QDomElement& start, const QString& attribute,
    const QStringList& values)
{
    QDomNode p = start.parentNode();

    while (!p.isNull())
    {
        QDomElement parent = p.toElement();
        if (!parent.isNull())
        {
            const QString v = parent.attribute(attribute);
            if (values.contains(v))
                return parent;
        }

        p = p.parentNode();
    }

    return {};
}

bool
gt::module_upgrade::properties::setPropertyTypeAndValue(
    QDomElement& propElement,
    const QString& newType,
    const QString& newValue)
{
    if (propElement.isNull()) return false;

    propElement.setAttribute(TYPE_STRING, newType);
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

QDomElement
gt::module_upgrade::properties::propNode(const QString& name,
                                               const QDomElement& root)
{
    QDomElement propElement = root.firstChildElement(PROP_STRING);
    while (!propElement.isNull())
    {
        if (propElement.attribute(NAME_STRING) == name)
        {
            return propElement;
        }

        propElement = propElement.nextSiblingElement(PROP_STRING);
    }

    return {};
}

bool
gt::module_upgrade::properties::setPropertyTypeAndValue(
    const QDomElement& root, const QString& name,
    const QString& newType, const QString& newValue)
{
    QDomElement prop = propNode(name, root);

    return setPropertyTypeAndValue(prop, newType, newValue);
}

double
gt::module_upgrade::properties::doubleValue(
    const QDomElement& parent, const QString& propName)
{
    QDomElement propElement = propNode(propName, parent);

    QString text = propElement.text();

    bool ok = false;
    double val = text.toDouble(&ok); // returns 0.0 for empty text

    if (text.isEmpty() || !ok)
    {
        gtWarningId("Module-Upgrader")
            << QObject::tr("Cannot read value from an empty text for property %1").arg(propName);
        return 0.0;
    }

    return val;
}

void
gt::module_upgrade::properties::appendProperty(
    QDomElement& parent, const QString& propertyName,
    const QString& propertyType, const QString& value)
{
    QDomDocument doc = parent.ownerDocument();
    QDomElement propElement = doc.createElement(PROP_STRING);
    QDomText valElement = doc.createTextNode(value);

    propElement.setAttribute(NAME_STRING, propertyName);
    propElement.setAttribute(TYPE_STRING, propertyType);
    propElement.appendChild(valElement);
    parent.appendChild(propElement);
}

void
gt::module_upgrade::properties::appendDoubleProperty(
    QDomElement& parent, const QString& propertyName, double val)
{
    appendProperty(parent, propertyName,
                   QStringLiteral("double"), QString::number(val));
}

void
gt::module_upgrade::properties::removeProperty(
    QDomElement& parent, const QString& propertyName)
{
    QDomElement propElement = propNode(propertyName, parent);

    if (!propElement.isNull())
        parent.removeChild(propElement);
}

QString
gt::module_upgrade::createUuid()
{
    return QUuid::createUuid().toString();
}

void
gt::module_upgrade::properties::renameProperty(
    QDomElement const& parent, const QString& oldName, const QString& newName)
{
    QDomElement propElement = propNode(oldName, parent);
    if (!propElement.isNull())
        propElement.setAttribute(NAME_STRING, newName);
}

QDomElement
gt::module_upgrade::appendNewGtlabObject(
    QDomElement& parent,
    const QString& className,
    const QString& objectName)
{
    QDomDocument dom = parent.ownerDocument();
    QDomElement newObject = dom.createElement(OBJ_STRING);
    newObject.setAttribute(CLASS_STRING, className);
    newObject.setAttribute(NAME_STRING, objectName);
    newObject.setAttribute(QStringLiteral("uuid"), createUuid());
    parent.appendChild(newObject);

    return newObject;
}

QDomElement
gt::module_upgrade::addObjectList(QDomElement& parent)
{
    QDomDocument dom = parent.ownerDocument();
    QDomElement childrenListElement = dom.createElement(QStringLiteral("objectlist"));
    parent.appendChild(childrenListElement);

    return childrenListElement;
}

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

            if (prop.tagName() == PROP_STRING)
            {
                QString oldUUID = prop.attribute(NAME_STRING);

                // Search for sub element <property name="$$formerNameKey$$">NewName</property>
                QDomElement nameElem;
                QDomNode sub = prop.firstChild();

                while (!sub.isNull())
                {
                    if (sub.isElement())
                    {
                        QDomElement e = sub.toElement();
                        if (e.attribute(NAME_STRING) == formerNameKey)
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
                    prop.setAttribute(NAME_STRING, newName);

                    // Remove subelement
                    prop.removeChild(nameElem);
                }
            }
        }

        child = child.nextSibling();
    }
}
