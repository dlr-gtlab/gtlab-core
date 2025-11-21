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
#include <tl/optional.hpp>

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
 * @brief findParentByAttribute
 * Searches in the hierarchy of QDomElements upwards to find a parent
 * which has an attribute which matches its value to one
 * of a given list of values
 * @param start dom element to search a parent element for
 * @param attribute of the parent which is meant to be checked
 * @param values - value list of accepted values for the match
 * @return the dom element which fullfills the match check
 * Returns null element if not found
 */
QDomElement GT_DATAMODEL_EXPORT findParentByAttribute(const QDomElement& start,
                                                      const QString& attribute,
                                                      const QStringList& values);



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

/**
 * @brief addNewPropertyElement
 * Add a new generated property element to a given parent.
 * @param parent to append the property to
 * @param propertyName - name of the new property
 * @param propertyType - type of the new property
 * @param value - value of the new property
 * @return the created element
 */
QDomElement GT_DATAMODEL_EXPORT addNewPropertyElement(QDomElement& parent,
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

/**
 * @brief addNewDoublePropertyElement
 * Add a new generated property element of type double to a given parent.
 * @param parent to append the property to
 * @param propertyName - name of the new property
 * @param val
 */
inline QDomElement addNewDoublePropertyElement(QDomElement& parent,
                                            const QString& propertyId,
                                            double val)
{
    return addNewPropertyElement(parent, propertyId, QStringLiteral("double"),
                                 QString::number(val));
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

/**
 * @brief removeProperty
 * Removes a child property element from a given parent element
 * @param parent of the old property element to remove
 * @param propertyName - name of the property to remove
 * @return success indicator
 */
bool GT_DATAMODEL_EXPORT removePropertyElement(QDomElement& parent,
                                               const QString& propertyName);

/**
 * @brief renamePropertyElement - rename a property
 * @param parent - parent object which has the GtProperty
 * @param oldName - old name to replace
 * @param newName - new name to use now
 * @return success indicator
 */
bool GT_DATAMODEL_EXPORT renamePropertyElement(
    const QDomElement& parent, const QString& oldName,
    const QString& newName);

/**
 * @brief getDoublePropetyElementValue
 * Find a property element as a sub element of the given root by the name
 * of the property element and return its double value in an optional
 * @param parent - element which has the property as child
 * @param propName - name of the property to evaluate
 * @return value of the property given as optional to indicate if the value
 * is valid or not
 */
tl::optional<double> GT_DATAMODEL_EXPORT getDoublePropetyElementValue(
    QDomElement const& parent, const QString& propName);

/**
 * @brief addObjectElement
 * This function appends a basic structure for a object to a parent
 * as a given QDomElement. The new object has the attributes
 * "name" and "class" (from the arguments) and
 * "uuid" (generated for the new object)
 * @param parent
 * @param className of the added element
 * @param objectName of the added element
 * @return the new element
 */
QDomElement GT_DATAMODEL_EXPORT addObjectElement(QDomElement& parent,
                                                 const QString& className,
                                                 const QString& objectName);

/**
 * @brief addObjectList
 * In cases of multiple child elements the xml structure uses a list container
 * object as a mid layer. Such an object can be added with this function *
 * @param parent to add the element to
 * @return the new element
 */
QDomElement GT_DATAMODEL_EXPORT addObjectListElement(QDomElement& parent);

} // namespace xml

} // namespace gt

#endif // GTXMLUTILITIES_H
