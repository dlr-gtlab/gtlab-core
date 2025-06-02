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
#include "gt_regularexpression.h"

#include "gt_object.h"

/**
 * namespace for RegularExpressions, used in GTlab
 */
namespace gt
{

/**
 *  THIS NAMESPACE IS DEPRECATED
 *
 *  DO NOT ADD NEW FUNCTIONALITY
 *
 *  Use QRegularExpression based functions from gt_regularexpression.h instead!
 */
namespace re_deprecated
{

/**
 * @brief only use this for tested regular expressions
 * @param re
 * @return
 */
GT_DATAMODEL_EXPORT QRegExp toRegExp(const QRegularExpression& re);

/**
 * @brief woUmlauts - accepts all but ä ö ü ß
 * @return
 */
[[deprecated ("Use gt::rex::woUmlauts instead")]]
inline QRegExp woUmlauts()
{
    return toRegExp(gt::rex::woUmlauts());
}

/**
 * @brief umlauts - accept äöüßÄÖÜ
 * @return
 */
[[deprecated ("Use gt::rex::umlauts instead")]]
inline QRegExp umlauts()
{
    return toRegExp(gt::rex::umlauts());
}

/**
 * @brief umlautsAnsSpecialChar
 * - accepts äöüßÄÖÜ .:,;#?+~* /&%$§!"{}
 * @return
 */
 [[deprecated ("Use gt::rex::umlautsAndSpecialChar instead")]]
inline QRegExp umlautsAndSpecialChar()
{
    return toRegExp(gt::rex::umlautsAndSpecialChar());
}

/**
 * @brief onlyLetters - accepts all letter (a-z and A-Z) with a
 * minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLetters instead")]]
inline QRegExp GT_DATAMODEL_EXPORT onlyLetters()
{
    return toRegExp(gt::rex::onlyLetters());
}

[[deprecated ("Use gt::rex::onlyLettersHint instead")]]
inline QString onlyLettersHint()
{
    return gt::rex::onlyLettersHint();
}

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbers instead")]]
inline QRegExp onlyLettersAndNumbers()
{
    return toRegExp(gt::rex::onlyLettersAndNumbers());
}

[[deprecated ("Use gt::rex::onlyLettersAndNumbersHint instead")]]
inline QString onlyLettersAndNumbersHint()
{
    return gt::rex::onlyLettersAndNumbersHint();
}

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1 and dot
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndDot instead")]]
inline QRegExp onlyLettersAndNumbersAndDot()
{
    return toRegExp(gt::rex::onlyLettersAndNumbersAndDot());
}

[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndDotHint instead")]]
inline QString onlyLettersAndNumbersAndDotHint()
{
    return gt::rex::onlyLettersAndNumbersAndDotHint();
}

/**
 * @brief onlyLettersAndNumbersAndSpace - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ and space with a minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndSpace instead")]]
inline QRegExp onlyLettersAndNumbersAndSpace()
{
    return toRegExp(gt::rex::onlyLettersAndNumbersAndSpace());
}

[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndSpaceHint instead")]]
inline QString onlyLettersAndNumbersAndSpaceHint()
{
    return gt::rex::onlyLettersAndNumbersAndSpaceHint();
}

/**
 * @brief forUnits - accepts all letters and numbers and some
 * special symbols typical used for units
 * @return
 */
[[deprecated ("Use gt::rex::forUnits instead")]]
inline QRegExp forUnits()
{
    return toRegExp(gt::rex::forUnits());
}

/**
 * @brief forExpressions
 * @return
 */
[[deprecated ("Use gt::rex::forExpressions instead")]]
inline QRegExp forExpressions()
{
    return toRegExp(gt::rex::forExpressions());
}

/**
 * @brief forStations - accepts a combination
 * of an S and one to three numbers
 * @return
 */
[[deprecated ("Use gt::rex::forStations instead")]]
inline QRegExp forStations()
{
    return toRegExp(gt::rex::forStations());
}

/**
 * @brief forDoubles
 * @return
 */
[[deprecated ("Use gt::rex::forDoubles instead")]]
inline QRegExp forDoubles()
{
    return toRegExp(gt::rex::forDoubles());
}

/**
 * @brief forDoublesLists
 * @return regexp for double lists
 */
[[deprecated ("Use gt::rex::forDoublesLists instead")]]
inline QRegExp forDoublesLists()
{
    return toRegExp(gt::rex::forDoublesLists());
}

/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 */
[[deprecated ("Use gt::rex::forSemVers instead")]]
inline QRegExp forSemVers()
{
    return toRegExp(gt::rex::forSemVers());
}

/**
 * @brief Matches file filter strings, e.g.
 *
 * "Python (python* python3)"
 * "Textfiles (*.txt)"
 */
[[deprecated ("Use gt::rex::forFileDialogFilters instead")]]
inline QRegExp forFileDialogFilters()
{
    return toRegExp(gt::rex::forFileDialogFilters());
}

/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 */
[[deprecated ("Use gt::rex::forHexColorCode instead")]]
inline QRegExp forHexColorCode()
{
    return toRegExp(gt::rex::forHexColorCode());
}

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

} // namespace re_deprecated

namespace re = re_deprecated;

} // namespace gt

#endif // GTREGEXP_H
