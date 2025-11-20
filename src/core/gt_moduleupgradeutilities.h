/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Created on: 17.11.2025
 *  Author: Jens Schmeink (AT-TWK)
 */
#ifndef GT_MODULEUPGRADEUTILITIES_H
#define GT_MODULEUPGRADEUTILITIES_H

#include "gt_core_exports.h"

#include <QDomNode>
#include <QDomElement>

namespace gt {

namespace module_upgrade {

/**
 * @brief The SearchMode enum
 * Defines whether the search should continue into child elements
 * that are themselves matches.
 */
enum class SearchMode {
    Recursive,   ///< Search the entire subtree and return all matching elements.
    NonRecursive ///< Return only top-level matches; ignore matches inside other matches.
};

/**
 * @brief findElementsByClass
 * Collects the sub elements of a given root element which have a value
 * for "class" which matches an entry in the given "classNames" list
 *
 * @param node - root node to search in
 * @param classNames - list of classnames to check for
 * @param searchMode - is it allowed to have elements as subelements of
 * others which match the comparison. This might shorten the search
 */
GT_CORE_EXPORT QVector<QDomElement> findElementsByClass(
    const QDomNode& node, const QStringList& classNames,
    SearchMode searchMode = SearchMode::Recursive);

/**
 * @brief findElementsByAttribute
 * Based on a root node the function searches all QDomElements which match
 * an attribute with one of the given entries of values list.
 * Examples are:
 * 1.) For the attribute "class" search for elements with the values {"MyCalc", "MyTask"}
 *     to find specific objects
 * 2.) For the attribute "type" search for elements with the values {"QString", "str"}
 *     to find specific values/properties
 *
 * @param node - root element
 * @param attribute - attribute of node element to match
 * @param values - values for comparison
 * @param searchMode - is it allowed to have elements as subelements of
 * others which match the comparison. This might shorten the search
 * @return a list of the elements which match the attribute search
 */
GT_CORE_EXPORT QVector<QDomElement> findElementsByAttribute(
    const QDomNode& node, const QString& attribute,
    const QStringList& values,
    SearchMode searchMode = SearchMode::Recursive);

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
GT_CORE_EXPORT QDomElement findParentByAttribute(const QDomElement& start,
                                                 const QString& attribute,
                                                 const QStringList& values);



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
GT_CORE_EXPORT QDomElement addObjectElement(QDomElement& parent,
                                            const QString& className,
                                            const QString& objectName);

/**
 * @brief addObjectList
 * In cases of multiple child elements the xml structure uses a list container
 * object as a mid layer. Such an object can be added with this function *
 * @param parent to add the element to
 * @return the new element
 */
GT_CORE_EXPORT QDomElement addObjectListElement(QDomElement& parent);

/**
 * @brief normalizePropertyContainerId
 * For renaming in property containers:
 * Properties in a container with a given $formerNameKey are removed and their
 * value is used to replace the ident of the property container entry
 * @param container
 * @param formerNameKey
 * @param replaceMap - a map to collect the mapping of the former uuid based
 * identification and the new name (for later usage)
 */
GT_CORE_EXPORT void normalizePropertyContainerId(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap);

/**
 * @brief setPropertyTypeAndValue
 * @param propElement
 * @param newType
 * @param newValue
 * @return false if domelement is invalid, else true
 */
GT_CORE_EXPORT bool setPropertyTypeAndValue(QDomElement& propElement,
                                            const QString& newType,
                                            const QString& newValue);

/**
 * @brief setPropertyTypeAndValue
 * @param root - root element to find the property as a sub element
 * @param name - name of the property to change
 * @param newType - new type to set for the property
 * @param newValue - new value to set for the property
 * @return true in case of success
 */
GT_CORE_EXPORT bool setPropertyTypeAndValue(const QDomElement& root,
                                            const QString& name,
                                            const QString& newType,
                                            const QString& newValue);

/**
 * @brief propertyElement
 * Find a property element as a sub element of the given root by the name
 * of the property element
 * @param name - name of the property elment to search
 * @param root - root element to start search from
 * @return element with given name if it exists, else empty element
 */
GT_CORE_EXPORT QDomElement propertyElement(const QString& name,
                                           const QDomElement& root);

/**
 * @brief doubleValue
 * Find a property element as a sub element of the given root by the name
 * of the property element and return its double value
 * @param parent - element which has the property as child
 * @param propName - name of the property to evaluate
 * @return value of the property - 0.0 if element is not found nor a number
 */
GT_CORE_EXPORT double doubleValue(QDomElement const& parent,
                                  const QString& propName);

/**
 * @brief addPropertyElement
 * Add a new generated property element to a given parent.
 * @param parent to append the property to
 * @param propertyName - name of the new property
 * @param propertyType - type of the new property
 * @param value - value of the new property
 * @return the created element
 */
GT_CORE_EXPORT QDomElement addPropertyElement(
    QDomElement& parent, const QString& propertyName,
    const QString& propertyType, const QString& value);

/**
 * @brief addDoublePropertyElement
 * Add a new generated property element of type double to a given parent.
 * @param parent to append the property to
 * @param propertyName - name of the new property
 * @param val
 */
GT_CORE_EXPORT void addDoublePropertyElement(
    QDomElement& parent, const QString& propertyName, double val);

/**
 * @brief removeProperty
 * Removes a child property element from a given parent element
 * @param parent of the old property element to remove
 * @param propertyName - name of the property to remove
 */
GT_CORE_EXPORT void removeProperty(QDomElement& parent,
                                   const QString& propertyName);

/**
 * @brief renamePropertyElement - rename a property
 * @param parent - parent object which has the GtProperty
 * @param oldName - old name to replace
 * @param newName - new name to use now
 */
GT_CORE_EXPORT void renamePropertyElement(
    const QDomElement& parent, const QString& oldName,
    const QString& newName);


} // module_upgrade
} // gt


#endif // GT_MODULEUPGRADEUTILITIES_H
