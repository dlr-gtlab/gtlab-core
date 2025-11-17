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


static QString
sanitizeName(const QString& name)
{
    if (name.isEmpty())
        return QStringLiteral("unnamed");

    QString out;
    out.reserve(name.size());
    for (QChar c : name)
    {
        if (c.isLetterOrNumber() || c == QLatin1Char('-') ||
            c == QLatin1Char('_'))
            out.append(c);
        else
            out.append(QLatin1Char('_'));
    }
    return out;
}

static QString
sanitizeUuid(const QString& uuid)
{
    // Strip { } if present to make file names shell-friendlier
    QString u = uuid;
    u.remove(QLatin1Char('{'));
    u.remove(QLatin1Char('}'));
    return u;
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

void
gt::xml::collectLinkedObjects(QDomDocument& masterDoc, QDomNode& node,
                              const QDir& rootDir, const QDir& linksRootDir,
                              QStringList& objectPath,
                              QVector<LinkedObject>& outExternal)
{
    for (QDomNode child = node.firstChild(); !child.isNull();)
    {
        QDomNode next = child.nextSibling(); // in case we replace child

        if (!child.isElement())
        {
            child = next;
            continue;
        }

        QDomElement elem = child.toElement();

        if (elem.tagName() == S_OBJECT_TAG)
        {
            const QString objName =
                elem.attribute(S_NAME_TAG, elem.attribute(S_CLASS_TAG));
            const QString className = elem.attribute(S_CLASS_TAG);
            const QString uuid = elem.attribute(S_UUID_TAG);

            const QString sanitizedObjName = sanitizeName(objName);
            const QString cleanUuid = sanitizeUuid(uuid);

            // track current object in hierarchy
            objectPath.push_back(sanitizedObjName);

            const QString asLink = elem.attribute(S_ASLINK_TAG,
                                                  QStringLiteral("false"));

            const bool shouldExternalize =
                asLink.compare(QStringLiteral("true"), Qt::CaseInsensitive) ==
                    0 ||
                asLink == QStringLiteral("1");

            if (shouldExternalize)
            {
                // ---- build external document ----
                QDomDocument extDoc(S_OBJECTFILE_TAG);
                QDomElement root =
                    extDoc.createElement(S_OBJECTFILE_TAG);
                extDoc.appendChild(root);

                QDomNode imported = extDoc.importNode(elem, /*deep=*/true);
                root.appendChild(imported);

                // remove aslink only in the external copy
                if (imported.isElement())
                {
                    QDomElement importedElem = imported.toElement();
                    importedElem.removeAttribute(S_ASLINK_TAG);
                }

                // ---- compute directory + filename ----
                QString relDir;
                if (objectPath.size() > 1)
                {
                    QStringList dirParts = objectPath;
                    dirParts.removeLast();
                    relDir = dirParts.join(QLatin1Char('/'));
                }

                const QString absDir = relDir.isEmpty()
                                           ? linksRootDir.absolutePath()
                                           : linksRootDir.filePath(relDir);

                QString fileName;
                if (!cleanUuid.isEmpty())
                    fileName = QStringLiteral("%1_%2.gtobj.xml")
                                   .arg(sanitizedObjName, cleanUuid);
                else
                    fileName = QStringLiteral("%1.gtobj.xml").arg(sanitizedObjName);

                const QString filePath = QDir(absDir).filePath(fileName);

                // href stored in master is relative to rootDir
                const QString href = rootDir.relativeFilePath(filePath);

                LinkedObject ext;
                ext.filePath = filePath;
                ext.href = href;
                ext.doc = extDoc;
                outExternal.push_back(std::move(ext));

                // ---- replace <object> with <objectref> in master ----
                QDomElement refElem =
                    masterDoc.createElement(S_OBJECTREF_TAG);
                refElem.setAttribute(S_CLASS_TAG, className);
                refElem.setAttribute(S_NAME_TAG, objName);
                if (!uuid.isEmpty())
                    refElem.setAttribute(S_UUID_TAG, uuid);
                refElem.setAttribute(S_HREF_TAG, href);

                node.replaceChild(refElem, child);

                // do not recurse into this object any further
                objectPath.pop_back();
                child = next;
                continue;
            }
            else
            {
                // normal object: recurse into its children
                collectLinkedObjects(masterDoc, child, rootDir, linksRootDir,
                                     objectPath, outExternal);
                objectPath.pop_back();
                child = next;
                continue;
            }
        }
        else
        {
            // anything else: recurse into children
            collectLinkedObjects(masterDoc, child, rootDir, linksRootDir,
                                 objectPath, outExternal);
            child = next;
        }
    }
}
