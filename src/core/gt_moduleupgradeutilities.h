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

#include <stdexcept>

#include <QDomNode>
#include <QDomElement>

namespace gt {

    struct ConversionError : public std::runtime_error
    {
        explicit ConversionError(const std::string& what) : std::runtime_error(what)
        {}
    };

namespace module_upgrade_utils {

/**
 * @brief findElementsByClass
 * Collects the sub elements of a given root element which have a value
 * for "class" which matches an entry in the given "classNames" list
 *
 * @param node - root node to search in
 * @param classNames - list of classnames to check for
 * @param allowNestedMatches - is it allowed to have elements as subelements of
 * others which match the comparison. This might shorten the search
 */
QList<QDomElement> findElementsByClass(const QDomNode& node,
                                       const QStringList& classNames,
                                       bool allowNestedClassElements);

/**
 * @brief findElementsByAttribute
 * Based on a root node the function searches all QDomElements which matches
 * an attribute with one of the given entries of values list,
 * e.g. if "class"(attribute) in {"MyCalc", "MyTask"}(values) or
 * "type" in {"QString", "str"}"
 *
 * @param node - root element
 * @param attribute - attribute of node element to match
 * @param values - values for comparison
 * @param allowNestedMatches - is it allowed to have elements as subelements of
 * others which match the comparison. This might shorten the search
 * @return a list of the elements which match the atrribute search
 */
QList<QDomElement> findElementsByAttribute(
    const QDomNode& node,
    const QString& attribute,
    const QStringList& values,
    bool allowNestedMatches);

/**
 * @brief findParentByAttribute
 * Searches in the hierarchy of QDomElements upwards to find a parent
 * which has an attribute which matches its value to one
 * of a given list of values
 * @param start dom element to search a parent element for
 * @param attribute of the parent which is meant to be checked
 * @param values - value list of accepted values for the match
 * @return the dom element which fullfills the match check
 */
QDomElement findParentByAttribute(const QDomElement& start,
                                  const QString& attribute,
                                  const QStringList& values);

/**
 * @brief newUuid
 * @return a new genereated uuid
 */
QString newUuid();

QDomElement appendNewGtlabObject(QDomElement& parent,
                                 QString const& className,
                                 QString const& objectName);

QDomElement addObjectList(QDomElement& parent);

namespace properties {

    /**
     * @brief updateTypeAndValue
     * @param propElement
     * @param newType
     * @param newValue
     * @return false if domelement is invalid, else true
     */
    bool updateTypeAndValue(QDomElement& propElement, const QString& newType,
                            const QString& newValue);

    /**
     * @brief updateTypeAndValue
     * @param name - name of the property to change
     * @param root - root element to find the property as a sub element
     * @param newType - new type to set for the property
     * @param newValue - new value to set for the property
     * @return true in case of success
     */
    bool updateTypeAndValue(QString const& name,
                            const QDomElement& root,
                            const QString& newType,
                            const QString& newValue);

    /**
     * @brief propNode
     * @param name
     * @param root
     * @return element with given name if it exisists, else empty element
     */
    QDomElement propNode(QString const& name, const QDomElement& root);

    /**
     * @brief doubleValue
     * @param parent - element which has the property as child
     * @param propName - name of the property to evaluete
     * @return value of the property
     */
    double doubleValue(QDomElement const& parent,
                       QString const& propName);

    /**
     * @brief appendProperty
     * @param parent to append the property to
     * @param propertyName - name of the new property
     * @param propertyType - type of the new property
     * @param value - value of the new propety
     */
    void appendProperty(QDomElement& parent, QString const& propertyName,
                        QString const& propertyType, QString const& value);

    /**
     * @brief appendDoubleProperty
     * @param parent to append the property to
     * @param propertyName - name of the new property
     * @param val
     */
    void appendDoubleProperty(QDomElement& parent,
                              QString const& propertyName,
                              double val);

    /**
     * @brief removeProperty
     * @param parent of the old property element to remove
     * @param propertyName - name of the property to remove
     */
    void removeProperty(QDomElement& parent, QString const& propertyName);

    /**
     * @brief renameProperty - rename a property
     * @param parent - parent object which has the GtProperty
     * @param oldName - old name to replace
     * @param newName - new name to use now
     */
    void renameProperty(const QDomElement& parent,
                        const QString& oldName,
                        const QString& newName);

} // properties


} // module_upgrade
} // gt


#endif // GT_MODULEUPGRADEUTILITIES_H
