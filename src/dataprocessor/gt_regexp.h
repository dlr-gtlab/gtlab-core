/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_regexp.h
 *
 *  Created on: 05.12.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTREGEXP_H
#define GTREGEXP_H

#include <QRegExp>

#include "gt_datamodel_exports.h"

#include "gt_object.h"

/**
 * namespace for RegualrExpressions, used in GTlab
 */
namespace gt
{
namespace re
{

/**
 * @brief woUmlauts - accepts all but ä ö ü ß
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT woUmlauts();

/**
 * @brief umlauts - accept äöüßÄÖÜ
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT umlauts();

/**
 * @brief umlautsAnsSpecialChar
 * - accepts äöüßÄÖÜ .:,;#?+~* /&%$§!"{}
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT umlautsAndSpecialChar();

/**
 * @brief onlyLetters - accepts all letter (a-z and A-Z) with a
 * minimal length of 1
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT onlyLetters();
QString GT_DATAMODEL_EXPORT onlyLettersHint();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbers();
QString GT_DATAMODEL_EXPORT onlyLettersAndNumbersHint();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1 and dot
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndDot();
QString GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndDotHint();

/**
 * @brief onlyLettersAndNumbersAndSpace - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ and space with a minimal length of 1
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndSpace();
QString GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndSpaceHint();

/**
 * @brief forUnits - accepts all letters and numbers and some
 * special symbols typical used for units
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT forUnits();

/**
 * @brief forExpressions
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT forExpressions();

/**
 * @brief forStations - accepts a combination
 * of an S and one to three numbers
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT forStations();

/**
 * @brief forDoubles
 * @return
 */
QRegExp GT_DATAMODEL_EXPORT forDoubles();

/**
 * @brief forDoublesLists
 * @return regexp for double lists
 */
QRegExp GT_DATAMODEL_EXPORT forDoublesLists();

/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 */
QRegExp GT_DATAMODEL_EXPORT forSemVers();

/**
 * @brief Matches file filter strings, e.g.
 *
 * "Python (python* python3)"
 * "Textfiles (*.txt)"
 */
GT_DATAMODEL_EXPORT const QRegExp& forFileDialogFilters();

/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 */
QRegExp GT_DATAMODEL_EXPORT forHexColorCode();

/**
 * @brief restrictRegExpWithObjectNames
 * Modifies a existing RegExp to append the given names to restrict their usage
 * with the reg exp
 * @param namesToProhibit - names to add to be forbidden
 * @param defaultRegExp - exisitng reg exp to modify
 */
void GT_DATAMODEL_EXPORT restrictRegExpWithObjectNames(
    QStringList const& namesToProhibit, QRegExp& defaultRegExp);

/**
 * @brief Modifies a given QRegExp to restrict usage of sibling objects of the given GtObject obj
 * The template type T has to be defined as the class of the sibling object to restrict.
 * Note: The template type must be const
 * e.g. gt::re::restrictRegExpWithObjectSiblingsNames<const GtObject>(obj, defaultRegExp);
 * @param obj - The object to find the siblings and in most use cases the object which should be renamed
 * @param defaultRegExp - the basic regular expression to be extended with the siblings names to restrict.
 * If the object obj has no parent or siblings the RegExp is not modified
 */
template <typename T>
inline void restrictRegExpWithObjectSiblingsNames(GtObject const& obj,
                                                  QRegExp& defaultRegExp)
{
    const GtObject* parent = obj.parentObject();

    if (!parent) return;

    QList<T> siblings = parent->findDirectChildren<T>();

    if (siblings.isEmpty()) return;

    QStringList names;

    for (auto* s : qAsConst(siblings))
    {
        names.append(s->objectName());
    }

    names.removeAll(obj.objectName());

    restrictRegExpWithObjectNames(names, defaultRegExp);
}

} // namespace re

} // namespace gt

#endif // GTREGEXP_H
