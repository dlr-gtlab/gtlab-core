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
#include "gt_batchsaver.h"

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

namespace
{

struct LinkedObject
{
    QString filePath; // absolute path on disk
    QString href;     // path to use in objectref (relative to baseDir)
    QDomDocument doc; // GTLABOBJECTFILE wrapper
};

/**
 * @brief Collect objects marked for separate file storage and rewrite them as links.
 *
 * This helper recursively walks the DOM subtree rooted at @p node and looks for
 * <object> elements that have @c aslink="true" (case-insensitive) or
 * @c aslink="1". For each such object:
 *
 *  - A new @c QDomDocument is created with a @c <GTLABOBJECTFILE> root element.
 *    The <object> subtree is imported into this document and stored in
 *    @p outLinked as an @c LinkedObject entry, together with a target
 *    @c filePath and a relative @c href.
 *
 *  - The original <object> node in @p masterDoc is replaced by an
 *    @c <objectref> element that keeps the original @c class, @c name and
 *    @c uuid attributes and adds:
 *      - @c href : relative path of the separate object file (from @p baseDir),
 *      - @c load : currently set to @c "on-demand" as a hint.
 *
 * Objects without an @c aslink attribute, or with any value other than "true"
 * (case-insensitive) or "1", are left embedded in the master document and are
 * not added to @p outLinked.
 *
 * The @p objectPath parameter is used as a stack of sanitized object names
 * representing the current hierarchy (e.g. "Parameterization" /
 * "HPT_curvePackage" / "Mean_Line"). This is used only to compute the directory
 * structure and file names for the linked object files:
 *
 *  - The relative directory is built from all but the leaf name in
 *    @p objectPath, joined with '/'.
 *  - The file name is typically
 *        <cleanUuid>_<sanitizedLeafName>.gtobj.xml
 *    where @c cleanUuid is the @c uuid attribute stripped of braces.
 *
 * @param masterDoc   The master document being transformed. This function
 *                    modifies @p masterDoc in-place, replacing selected
 *                    <object> nodes with <objectref> nodes.
 * @param node        Current DOM node to process (typically the root element
 *                    when called initially). The function recurses into its
 *                    children as needed.
 * @param baseDir     Base directory used to compute absolute @c filePath and
 *                    relative @c href for each linked object file. No files are
 *                    written here; paths are just calculated.
 * @param objectPath  Stack of sanitized object names representing the current
 *                    path in the object hierarchy. The caller should pass an
 *                    empty list initially; this function pushes/pops as it
 *                    descends/ascends the tree.
 * @param outLinked   Output list that receives one @c LinkedObject entry for
 *                    each <object> node marked with @c aslink="true" or @c "1".
 *                    The caller can later use these entries to write the
 *                    separate object files to disk.
 */
void collectLinkedObjects(QDomDocument& masterDoc, QDomNode& node,
                          const QDir& rootDir, const QDir& linksRootDir,
                          QStringList& objectPath,
                          QVector<LinkedObject>& outExternal)
{
    using namespace gt::xml;

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

            const QString asLink =
                elem.attribute(S_ASLINK_TAG, QStringLiteral("false")).toLower();

            const bool createObjectRef =
                (asLink == "true" || asLink == "1" || asLink == S_REFONLY_TAG);

            // in case, the link could not be resolved
            const bool writeLinkedFile = (asLink != S_REFONLY_TAG);

            if (createObjectRef)
            {
                // ---- build external document ----
                QDomDocument extDoc(S_OBJECTFILE_TAG);
                QDomElement root = extDoc.createElement(S_OBJECTFILE_TAG);
                extDoc.appendChild(root);

                QDomNode imported = extDoc.importNode(elem, /*deep=*/true);
                root.appendChild(imported);

                // remove aslink only in the external copy
                if (imported.isElement())
                {
                    QDomElement importedElem = imported.toElement();
                    importedElem.removeAttribute(S_ASLINK_TAG);
                }

                // Note: we call collectLinkedObjects on `imported`, not on `root`,
                // so objectPath is not pushed twice for the same object.
                collectLinkedObjects(extDoc, imported, rootDir, linksRootDir,
                                     objectPath, outExternal);

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
                    fileName =
                        QStringLiteral("%1.gtobj.xml").arg(sanitizedObjName);

                const QString filePath = QDir(absDir).filePath(fileName);

                // href stored in master is relative to rootDir
                const QString href = rootDir.relativeFilePath(filePath);

                if (writeLinkedFile)
                {
                    // Avoid overwriting / replacing unresolved linked files
                    // Otherwise, we would recreate the file with default data
                    LinkedObject ext;
                    ext.filePath = filePath;
                    ext.href = href;
                    ext.doc = extDoc;
                    outExternal.push_back(std::move(ext));
                }

                // ---- replace <object> with <objectref> in master ----
                QDomElement refElem = masterDoc.createElement(S_OBJECTREF_TAG);
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

void
expandObjectRefsInDocument(QDomDocument& doc,
                           const QDir& baseDir,
                           QStringList* warnings,
                           QSet<QString>& recursionStack);

QDomDocument
loadAndExpandImpl(const QString& path,
                  QStringList* warnings,
                  QSet<QString>& recursionStack)
{


    QFileInfo fileInfo(path);
    const QString absPath = fileInfo.absoluteFilePath();

    QFile linkedFile(absPath);

    // Simple cycle protection: A.xml -> B.xml -> A.xml
    if (recursionStack.contains(absPath))
    {
        const QString msg =
            QStringLiteral("Detected recursive include of '%1'; keeping objectref nodes.").arg(absPath);
        if (warnings) warnings->push_back(msg);
        return QDomDocument();
    }

    auto warning = [&](const QString& msg)
    {
        if (warnings) warnings->push_back(msg);
        recursionStack.remove(absPath);
        return QDomDocument();
    };

    if (!linkedFile.exists())
    {
        const QString msg =
            QStringLiteral("Linked file '%1' does not exist; keeping objectref nodes.").arg(absPath);
        return warning(msg);
    }

    recursionStack.insert(absPath);

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!gt::xml::readDomDocumentFromFile(linkedFile, doc, true, &errorStr,
                                          &errorLine, &errorColumn))
    {
        const QString msg =
            QStringLiteral("Could not read or parse '%1'; keeping objectref nodes that reference it.")
                .arg(absPath);
        const QString msg2 = QObject::tr("XML ERROR! line: %1 column: %2 -> %3")
                                .arg(errorLine).arg(errorColumn).arg(errorStr);
        return warning(msg + "\n" + msg2);
    }

    if (doc.documentElement().isNull())
    {
        const QString msg =
            QStringLiteral("Could not read or parse '%1'; keeping objectref nodes that reference it.")
                .arg(absPath);
        return warning(msg);
    }

    const QDir baseDir = QFileInfo(absPath).dir();
    expandObjectRefsInDocument(doc, baseDir, warnings, recursionStack);

    recursionStack.remove(absPath);
    return doc;
}

void
expandObjectRefsInDocument(QDomDocument& doc,
                            const QDir& baseDir,
                            QStringList* warnings,
                            QSet<QString>& recursionStack)
{
    QDomNodeList nodeList = doc.elementsByTagName(gt::xml::S_OBJECTREF_TAG);

    // Snapshot to avoid weirdness while modifying the DOM
    QList<QDomElement> refs;
    refs.reserve(nodeList.count());
    for (int i = 0; i < nodeList.count(); ++i)
    {
        QDomElement e = nodeList.at(i).toElement();
        if (!e.isNull())
            refs.push_back(e);
    }

    for (const QDomElement& refElem : refs)
    {
        const QString uuid    = refElem.attribute(gt::xml::S_UUID_TAG);
        const QString relPath = refElem.attribute(gt::xml::S_HREF_TAG);

        if (relPath.isEmpty())
        {
            const QString msg =
                QStringLiteral("objectref (uuid='%1') has no href; keeping objectref.").arg(uuid);
            if (warnings) warnings->push_back(msg);
            continue; // keep objectref
        }

        const QString targetPath = baseDir.filePath(relPath);
        QDomDocument linkedDoc = loadAndExpandImpl(targetPath, warnings, recursionStack);

        if (linkedDoc.documentElement().isNull())
        {
            const QString msg =
                QStringLiteral("Could not expand link '%1' for objectref uuid='%2'; keeping objectref.")
                    .arg(targetPath, uuid);
            if (warnings) warnings->push_back(msg);
            continue; // keep objectref
        }

        QDomElement linkedRoot = linkedDoc.documentElement();

        // Typical pattern: <Root> <object ...>...</object> </Root>
        QDomElement objectElem = linkedRoot.firstChildElement("object");
        if (objectElem.isNull())
            objectElem = linkedRoot;

        QDomNode imported = doc.importNode(objectElem, /*deep=*/true);
        QDomNode parent   = refElem.parentNode();

        if (parent.isNull() || imported.isNull())
        {
            const QString msg =
                QStringLiteral("Failed to replace objectref from '%1' (uuid='%2'); keeping objectref.")
                    .arg(targetPath, uuid);
            if (warnings) warnings->push_back(msg);
            continue;
        }

        parent.replaceChild(imported, refElem);
    }
}


} // namespace


QDomDocument
gt::xml::loadProjectXmlWithLinkedObjects(const QString &masterPath, QStringList *warnings)
{
    QSet<QString> recursionStack;
    return loadAndExpandImpl(masterPath, warnings, recursionStack);
}

bool
gt::xml::saveProjectXmlWithLinkedObjects(const QString& projectName,
                                         const QDomDocument& doc,
                                         const QDir& baseDir,
                                         const QString& masterFilePath,
                                         QString* errorOut)
{

    // work on a copy, since doc is const
    QDomDocument masterDoc = doc;

    // 1) externalize objects in-memory
    QVector<LinkedObject> externals;
    QStringList objectPath;

    QDomElement rootElem = masterDoc.documentElement();
    if (!rootElem.isNull())
    {
        QDomNode rootNode = rootElem;

        // Derive package name from master file: "MyPackage.xml" -> "MyPackage"
        const QFileInfo masterInfo(masterFilePath);
        const QString packageName = masterInfo.completeBaseName();

        // Linked objects live under: <baseDir>/<MyPackage>
        const QDir linksRootDir(baseDir.filePath(packageName));

        collectLinkedObjects(masterDoc, rootNode, baseDir,
                                      linksRootDir, objectPath, externals);
    }

    // 2) batch save: externals + master
    GtBatchSaver batchsaver;

    // external object files first
    for (const LinkedObject& ext : qAsConst(externals))
    {
        // third parameter: attrOrdered=true (same as before)
        batchsaver.addXml(ext.filePath, ext.doc, true);
    }

    // master file last
    batchsaver.addXml(masterFilePath, masterDoc, true);

    if (!batchsaver.commit())
    {
        gtError() << projectName << QStringLiteral(": ")
                  << batchsaver.errorString();
        return false;
    }

    return true;
}
