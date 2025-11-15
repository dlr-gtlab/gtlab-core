/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_xmlutilities.cpp
 *
 *  Created on: 04.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QDomElement>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

#include "gt_xmlutilities.h"
#include "gt_xmlexpr.h"
#include "gt_objectio.h"

#include "gt_logging.h"


namespace
{

template <typename Predicate>
void
findElements(
    const QDomElement& elem, Predicate&& func,
    QList<QDomElement>& foundElements)
{
    if (func(elem))
    {
        foundElements.append(elem);
    }

    QDomElement child = elem.firstChildElement();
    while(!child.isNull())
    {
        findElements(child, func, foundElements);
        child = child.nextSiblingElement();
    }
}

} // namespace

QList<QDomElement>
gt::xml::findObjectElementsByClassName(
    const QDomElement& root, const QString& className)
{
    QList<QDomElement> result;
    findElements(root, [&](const QDomElement& elem) {
        return elem.tagName() == gt::xml::S_OBJECT_TAG &&
               elem.attributes().contains(gt::xml::S_CLASS_TAG) &&
               elem.attribute(gt::xml::S_CLASS_TAG) == className;
    }, result);
    return result;
}

bool
gt::xml::writeDomElementOrderedAttribute(const QDomElement& element,
                                         QXmlStreamWriter& writer)
{
    static QRegularExpression carriageReturns("[\\r]");

    writer.writeStartElement(element.tagName());

    // attributes
    QStringList attr_ids;
    QDomNamedNodeMap attr_nodes = element.attributes();

    for (int i = 0; i < attr_nodes.size(); ++i)
    {
        QDomNode attr_node = attr_nodes.item(i);
        QDomAttr attr = attr_node.toAttr();
        attr_ids << attr.name();
    }

    attr_ids.sort();

    foreach (const QString& attr_id, attr_ids)
    {
        writer.writeAttribute(attr_id, element.attribute(attr_id));
    }

    if (element.hasChildNodes())
    {
        QDomNode c_node = element.firstChild();

        while (!c_node.isNull())
        {
            if (c_node.nodeType() == QDomNode::TextNode)
            {
                // remove carriage returns to avoid double definitions
                writer.writeCharacters(
                            c_node.toText().data().remove(carriageReturns));
            }
            else if (c_node.nodeType() == QDomNode::ElementNode)
            {
                if (!gt::xml::writeDomElementOrderedAttribute(
                            c_node.toElement(), writer))
                {
                    return false;
                }
            }

            c_node = c_node.nextSibling();
        }
    }

    writer.writeEndElement();

    return true;
}

bool
gt::xml::writeDomDocumentToFile(const QString& filePath,
                                const QDomDocument& doc,
                                bool attrOrdered)
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    bool success = writeDomDocumentToDevice(file, doc, attrOrdered);

    file.close();
    return success;
}

bool
gt::xml::writeDomDocumentToDevice(QIODevice& device,
                                const QDomDocument& doc,
                                bool attrOrdered)
{


    if (attrOrdered)
    {
        QXmlStreamWriter str_w(&device);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        str_w.setCodec("UTF-8");
#endif

        str_w.setAutoFormatting(true);

        str_w.writeStartDocument(QStringLiteral("1.0"));

        QDomElement rootElement = doc.documentElement();

        if (!rootElement.isNull())
        {
            if (!gt::xml::writeDomElementOrderedAttribute(rootElement, str_w))
            {
                return false;
            }
        }

        str_w.writeEndDocument();
    }
    else
    {
        QTextStream stream(&device);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        stream.setCodec("UTF-8");
#endif
        stream << doc.toString(4);
    }

    return true;
}

QDomElement
gt::xml::createPropertyElement(QDomDocument &doc, const QString &propertyId,
                            const QString &propertyType, const QString &value)
{
    auto prop = doc.createElement(gt::xml::S_PROPERTY_TAG);
    prop.setAttribute("name", propertyId);
    prop.setAttribute("type", propertyType);

    if (!value.isEmpty())
    {
        prop.appendChild(doc.createTextNode(value));
    }

    return prop;
}

QList<QDomElement> gt::xml::propertyElements(const QDomElement& root)
{
    QList<QDomElement> result;
    findElements(root, [&](const QDomElement& elem) {
            return elem.tagName() == gt::xml::S_PROPERTY_TAG;
        }, result);

    return result;
}

QDomElement gt::xml::findPropertyElement(const QDomElement &root,
                             const QString& id)
{
    QList<QDomElement> result;
    findElements(root, [&](const QDomElement& elem) {
            return elem.tagName() == gt::xml::S_PROPERTY_TAG &&
                   elem.attribute(gt::xml::S_NAME_TAG) == id;
        }, result);

    if (result.isEmpty())
    {
        return QDomElement();
    }

    if (result.size() > 1)
    {
        gtWarning() <<GtObject::tr("Found multiple properties with the same id '%1'").arg(id);
        return result.at(0);
    }

    // result.size() == 0
    return result.at(0);

}

bool
gt::xml::readDomDocumentFromFile(QFile& file,
                                 QDomDocument& doc,
                                 bool namespaceProcessing,
                                 QString* errorMsg,
                                 int* errorLine,
                                 int* errorColumn)
{
    assert(!file.isOpen());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMsg)
        {
            *errorMsg = QStringLiteral(
                            "could not open file! (%1)").arg(file.fileName());
        }

        return false;
    }

    const auto content = file.readAll();
    file.close();

    return doc.setContent(content, true, errorMsg, errorLine, errorColumn);
}
