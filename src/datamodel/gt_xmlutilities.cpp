/* GTlab - Gas Turbine laboratory
 * Source File: gt_xmlutilities.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QDomElement>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QFile>

#include "gt_xmlutilities.h"

bool
GtXmlUtilities::writeDomElementOrderedAttribute(const QDomElement& element,
                                                QXmlStreamWriter& writer)
{
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
                writer.writeCharacters(c_node.toText().data());
            }
            else if (c_node.nodeType() == QDomNode::ElementNode)
            {
                if (!GtXmlUtilities::writeDomElementOrderedAttribute(
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
GtXmlUtilities::writeDomDocumentToFile(const QString& filePath,
                                       const QDomDocument& doc,
                                       bool attrOrdered)
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    if (attrOrdered)
    {
        QXmlStreamWriter str_w(&file);
        str_w.setAutoFormatting(true);

        str_w.writeStartDocument(QStringLiteral("1.0"));

        QDomElement rootElement = doc.documentElement();

        if (!rootElement.isNull())
        {
            if (!GtXmlUtilities::writeDomElementOrderedAttribute(
                        rootElement, str_w))
            {
                return false;
            }
        }

        str_w.writeEndDocument();
    }
    else
    {
        QTextStream stream(&file);
        stream << doc.toString(5);
    }

    file.close();

    return true;
}
