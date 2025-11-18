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

enum class LinkFileSaveType
{
    OneFile,
    WithLinkedFiles
};

/**
 * @brief Save a project XML document as a master file plus linked object files.
 *
 * This function serializes GTlab project XML files into:
 *   - a master XML file, and
 *   - zero or more separate “linked” object files,
 * and writes all of them atomically.
 *
 * Objects marked with @c aslink="true" on their <object> elements are not kept
 * fully embedded in the master file. Instead, for each such node:
 *   - The object subtree is written to a separate XML file with the extension
 *     @c .gtobj.xml and a path derived from the object hierarchy, e.g.
 *     @c Parameterization/HPT_curvePackage/ABC-123_Mean_Line.gtobj.xml.
 *   - The original <object> node in the master document is replaced by an
 *     <objectref> node that references the separate file via a relative
 *     @c href attribute (relative to @p baseDir).
 *
 * All files (the master file and all linked object files) are written using
 * a batch saver that provides all-or-nothing semantics:
 *   - If all writes succeed, the master file and all linked files are updated,
 *     and previous versions are preserved as backup files.
 *   - If any write fails, no file on disk is modified and a descriptive error
 *     message is returned via @p errorOut.
 *
 * The input document @p doc is not modified by the caller; the function works
 * on an internal copy when transforming <object> into <objectref> nodes.
 *
 * This mechanism is purely about how objects are stored on disk. It is
 * unrelated to “externalized objects” in the runtime sense (objects that are
 * not kept in memory and are loaded on demand).
 *
 * @param projectName    Logical name of the project, used only for logging and
 *                       error reporting. It does not affect file names or XML
 *                       content.
 * @param doc            Project XML document to save. Must have a valid root
 *                       element; otherwise the function fails and returns @c false.
 * @param baseDir        Base directory used as the root for all output paths.
 *                       The @p masterFilePath and all linked object files
 *                       are written inside this directory (or its subfolders).
 * @param masterFilePath Path of the master XML file to write. This may be an
 *                       absolute path or a path under @p baseDir.
 * @param saveType       Whether to store with link files or as one file.
 * @param errorOut       Optional pointer to a string that will receive a
 *                       human-readable error description if the function
 *                       returns @c false. If @c nullptr, the error text is
 *                       discarded.
 *
 * @return @c true if all files were written successfully and the on-disk state
 *         is consistent; @c false if an error occurred (in which case no files
 *         are updated).
 */
GT_DATAMODEL_EXPORT
bool saveProjectXmlWithLinkedObjects(const QString& projectName,
                                     const QDomDocument& doc,
                                     const QDir& baseDir,
                                     const QString& masterFilePath,
                                     LinkFileSaveType saveType,
                                     QString* errorOut);


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
GT_DATAMODEL_EXPORT QDomDocument loadProjectXmlWithLinkedObjects(
    const QString& masterPath, QStringList* warnings = nullptr);

} // namespace xml

} // namespace gt

#endif // GTXMLUTILITIES_H
