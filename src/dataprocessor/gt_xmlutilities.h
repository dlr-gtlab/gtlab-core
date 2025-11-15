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

} // namespace xml

} // namespace gt

#endif // GTXMLUTILITIES_H
