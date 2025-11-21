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
 * @brief normalizePropertyContainerId
 * For renaming in property containers:
 * Properties in a container with a given $formerNameKey are removed and their
 * value is used to replace the ident of the property container entry
 * @param container - a QDomElement to describe a property conainter
 * @param formerNameKey - this is the attribute name to describe the name of
 * the container entry.
 * Often these are values like "name", "ParamName", "ResultName",...
 * This will be the attribute the function will look for to find the name value
 * to set as element id.
 * @param replaceMap - a map to collect the mapping of the former uuid based
 * identification and the new name (for later usage)
 */
GT_CORE_EXPORT void normalizePropertyContainerId(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap);


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
 * @brief setPropertyTypeAndValue
 * A propety element given as QDomElement will be changed: Its values for the
 * attributes type and value will be set to the new values given by the function
 * @param propElement - property element
 * @param newType - new type to set for the property
 * @param newValue - new value to set for the property
 * @return false if domelement is invalid, else true
 */
GT_CORE_EXPORT bool setPropertyTypeAndValue(QDomElement& propElement,
                                            const QString& newType,
                                            const QString& newValue);

/**
 * @brief setPropertyTypeAndValue
 * A propety element given as QDomElement will be changed: Its values for the
 * attributes type and value will be set to the new values given by the function.
 * The property element is not given as an argument but will be found
 * as a child of the given root with the given name
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

} // module_upgrade
} // gt


#endif // GT_MODULEUPGRADEUTILITIES_H
