/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_xmlutilities.h
 *
 *  Created on: 04.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTXMLUTILITIES_H
#define GTXMLUTILITIES_H

#include "gt_datamodel_exports.h"

#include <QList>
#include <QDomElement>
#include <QDir>

class QDomDocument;
class QXmlStreamWriter;
class QString;
class QFile;

namespace gt
{
namespace xml
{

/**
 * @brief Writes given QDomElement object to given XML stream. All XML
 * attributes are ordered case sensitive.
 * @param element XML DOM element
 * @param writer XML stream writer
 * @return Returns true if entire DOM element was written successfully.
 * Otherwise, false is returned.
 */
bool GT_DATAMODEL_EXPORT writeDomElementOrderedAttribute(
        const QDomElement& element,
        QXmlStreamWriter& writer);

/**
 * @brief Writes given QDomDocument object to given file path. The XML
 * attributes can be saved both unsorted and sorted. Default is sorted.
 * @param filePath Target file path (e.g. /home/files/my_file.xml)
 * @param doc Given DOM document
 * @param attrOrdered if true, the attributes are written in case sensitive
 * order. Otherwise, the order of the attributes is random (faster writing)
 * @return Returns true if entire DOM document was written successfully.
 * Otherwise, false is returned.
 */
bool GT_DATAMODEL_EXPORT writeDomDocumentToFile(const QString& filePath,
                                                const QDomDocument& doc,
                                                bool attrOrdered = true);

/**
 * @brief Writes given QDomDocument object to a output device. The XML
 * attributes can be saved both unsorted and sorted. Default is sorted.
 * @param device The device to write to
 * @param doc Given DOM document
 * @param attrOrdered if true, the attributes are written in case sensitive
 * order. Otherwise, the order of the attributes is random (faster writing)
 * @return Returns true if entire DOM document was written successfully.
 * Otherwise, false is returned.
 */
bool GT_DATAMODEL_EXPORT writeDomDocumentToDevice(QIODevice& device,
                                                  const QDomDocument& doc,
                                                  bool attrOrdered = true);


/**
 * @brief This function parses the XML document from given file.
 * File is opened in TextMode. (Fix for broken line endings)
 * Read Qt documentation for further information: QDomDocument::setContent
 */
bool GT_DATAMODEL_EXPORT readDomDocumentFromFile(QFile& file,
                                                 QDomDocument& doc,
                                                 bool namespaceProcessing,
                                                 QString* errorMsg=nullptr,
                                                 int* errorLine=nullptr,
                                                 int* errorColumn=nullptr);

/**
 * @brief Finds all elements below root of a specific gtobject type
 * @param root      The root element
 * @param className The class name to search for
 * @return List of found object elements, where attribute class==className
 */
QList<QDomElement> GT_DATAMODEL_EXPORT findObjectElementsByClassName(
    const QDomElement& root, const QString& className);

/**
 * @brief Returns all property elements under root
 * @return
 */
QList<QDomElement> GT_DATAMODEL_EXPORT propertyElements(
    const QDomElement& root);

QDomElement GT_DATAMODEL_EXPORT findPropertyElement(const QDomElement& root,
                                                    const QString& propId);

/**
 * @brief Creates a generic property xml element
 * @param doc The dom document
 * @param propertyId The id of the property e.g. "x"
 * @param propertyType The type of the property e.g. "double"
 * @param value        The value of the property e.g. "1.0"
 * @return The created element
 */
QDomElement GT_DATAMODEL_EXPORT createPropertyElement(QDomDocument& doc,
                                                   const QString& propertyId,
                                                   const QString& propertyType,
                                                   const QString& value);

inline QDomElement createDoublePropertyElement(QDomDocument& doc,
                                               const QString& propertyId,
                                               double value)
{
    return createPropertyElement(doc, propertyId, "double",
                              QString::number(value));
}

inline QDomElement createIntPropertyElement(QDomDocument& doc,
                                            const QString& propertyId,
                                            int value)
{
    return createPropertyElement(doc, propertyId, "int",
                              QString::number(value));
}

inline QDomElement createBoolPropertyElement(QDomDocument& doc,
                                             const QString& propertyId,
                                             bool value)
{
    return createPropertyElement(doc, propertyId, "bool",
                              QString::number(value));
}

inline QDomElement createStringPropertyElement(QDomDocument& doc,
                                               const QString& propertyId,
                                               QString value)
{
    return createPropertyElement(doc, propertyId, "QString",
                              std::move(value));
}

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
GT_DATAMODEL_EXPORT void collectLinkedObjects(QDomDocument& masterDoc,
                                              QDomNode& node,
                                              const QDir& rootDir,
                                              const QDir& linksRootDir,
                                              QStringList& objectPath,
                                              QVector<LinkedObject>& outLinked);

/**
 * @brief Load an XML document and recursively expand all linked object references.
 *
 * This function reads the given master XML file (e.g. package.xml) into a
 * QDomDocument and recursively replaces all <objectref> elements with the
 * contents of their linked documents.
 *
 * Linking is done via an attribute (typically "href") on <objectref> elements,
 * whose value is treated as a file path relative to the directory of the file
 * that contains the <objectref>. Each linked file is loaded, its own
 * <objectref> elements are expanded in the same way, and the resulting
 * <object> element (or, if no such child exists, the linked document's root
 * element) is imported into the master document and used to replace the
 * original <objectref>.
 *
 * Error handling:
 * - If a linked file cannot be opened or parsed, a warning message is emitted
 *   and optionally appended to @p warnings. In this case, the corresponding
 *   <objectref> element is left unchanged in the resulting document.
 * - The expansion continues for all other references even if some fail.
 * - A simple recursion guard prevents infinite loops if documents reference
 *   each other cyclically; in such a case, the affected <objectref> is kept
 *   and a warning is reported.
 *
 * @param masterPath Absolute or relative path to the master XML file
 *                   (e.g. "package.xml").
 * @param warnings   Optional output list that will receive human-readable
 *                   warning messages about failed or recursive includes. May
 *                   be nullptr if no messages are needed.
 *
 * @return A QDomDocument representing the expanded XML tree. If the master
 *         document cannot be read or parsed at all, an empty QDomDocument
 *         (with null root element) is returned.
 */
GT_DATAMODEL_EXPORT QDomDocument loadAndExpandDocument(
    const QString& masterPath, QStringList* warnings = nullptr);

} // namespace xml

} // namespace gt

#endif // GTXMLUTILITIES_H
