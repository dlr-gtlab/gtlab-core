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

/**
 * Hint: You find several helpful functions for xml manipulation in the
 * namespace gt::xml in the file gt_xmlutilities.h
 */

namespace gt {

namespace module_upgrade {

/**
 * @brief convertPropertyContainerToMap
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
GT_CORE_EXPORT void convertPropertyContainerToMap(
    QDomElement& container, const QString& formerNameKey,
    QMap<QString, QString>& replaceMap);

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
