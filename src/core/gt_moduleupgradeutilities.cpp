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

#include "gt_moduleupgradeutilities.h"

namespace {
    static QString CLASS_STRING = QStringLiteral("class");
    static QString PROP_STRING = QStringLiteral("property");
    static QString OBJ_STRING = QStringLiteral("object");
    static QString NAME_STRING = QStringLiteral("name");
    static QString TYPE_STRING = QStringLiteral("type");
}


QList<QDomElement>
gt::module_upgrade_utils::findElementsByClass(
    const QDomNode& node,
    const QStringList& classNames,
    bool allowNestedClassElements)
{
    return findElementsByAttribute(node, CLASS_STRING,
                                   classNames, allowNestedClassElements);
}

QList<QDomElement>
gt::module_upgrade_utils::findElementsByAttribute(
    const QDomNode& node,
    const QString& attribute,
    const QStringList& values,
    bool allowNestedMatches)
{
    QList<QDomElement> results;

    // internal recursive function
    std::function<void(const QDomNode&)> recurse =
        [&](const QDomNode& n)
    {
        QDomNode child = n.firstChild();

        while (!child.isNull())
        {
            if (child.isElement())
            {
                QDomElement elem = child.toElement();
                if (!elem.isNull())
                {
                    const QString attrValue = elem.attribute(attribute);

                    // attribute found → save
                    if (values.contains(attrValue))
                    {
                        results.append(elem);

                        // if nested search is disabled
                        if (!allowNestedMatches)
                        {
                            child = child.nextSibling();
                            continue;
                        }
                    }
                }
            }

            // go deeper
            recurse(child);
            child = child.nextSibling();
        }
    };

    recurse(node);
    return results;
}

QDomElement
gt::module_upgrade_utils::findParentByAttribute(
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
gt::module_upgrade_utils::properties::updateTypeAndValue(
    QDomElement& propElement,
    const QString& newType,
    const QString& newValue)
{
    if (propElement.isNull()) return false;

    propElement.setAttribute(TYPE_STRING, newType);
    propElement.firstChild().setNodeValue(newValue);

    return true;
}

QDomElement
gt::module_upgrade_utils::properties::propNode(const QString& name,
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
gt::module_upgrade_utils::properties::updateTypeAndValue(
    const QString& name, const QDomElement& root,
    const QString& newType, const QString& newValue)
{
    QDomElement prop = propNode(name, root);

    return updateTypeAndValue(prop, newType, newValue);
}

double
gt::module_upgrade_utils::properties::doubleValue(
    const QDomElement& parent, const QString& propName)
{
    QDomElement propElement = propNode(propName, parent);

    QString text = propElement.text();

    return text.toDouble();
}

void
gt::module_upgrade_utils::properties::appendProperty(
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
gt::module_upgrade_utils::properties::appendDoubleProperty(
    QDomElement& parent, const QString& propertyName, double val)
{
    appendProperty(parent, propertyName, "double", QString::number(val));
}

void
gt::module_upgrade_utils::properties::removeProperty(
    QDomElement& parent, const QString& propertyName)
{
    QDomElement propElement = propNode(propertyName, parent);

    parent.removeChild(propElement);
}

QString
gt::module_upgrade_utils::newUuid()
{
    return QUuid::createUuid().toString();
}

void
gt::module_upgrade_utils::properties::renameProperty(
    const QDomElement& parent, const QString& oldName, const QString& newName)
{
    QDomElement propElement = propNode(oldName, parent);
    propElement.setAttribute(NAME_STRING, newName);
}

QDomElement
gt::module_upgrade_utils::appendNewGtlabObject(
    QDomElement& parent,
    const QString& className,
    const QString& objectName)
{
    QDomDocument dom = parent.ownerDocument();
    QDomElement newObject = dom.createElement(OBJ_STRING);
    newObject.setAttribute(CLASS_STRING, className);
    newObject.setAttribute(NAME_STRING, objectName);
    newObject.setAttribute("uuid", newUuid());
    parent.appendChild(newObject);

    return newObject;
}

QDomElement
gt::module_upgrade_utils::addObjectList(QDomElement& parent)
{
    QDomDocument dom = parent.ownerDocument();
    QDomElement childrenListElement = dom.createElement("objectlist");
    parent.appendChild(childrenListElement);

    return childrenListElement;
}
