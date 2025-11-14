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
[[deprecated ("Use gt::rex::woUmlauts instead")]]
QRegExp GT_DATAMODEL_EXPORT woUmlauts();

/**
 * @brief umlauts - accept äöüßÄÖÜ
 * @return
 */
[[deprecated ("Use gt::rex::umlauts instead")]]
QRegExp GT_DATAMODEL_EXPORT umlauts();

/**
 * @brief umlautsAnsSpecialChar
 * - accepts äöüßÄÖÜ .:,;#?+~* /&%$§!"{}
 * @return
 */
[[deprecated ("Use gt::rex::umlautsAndSpecialChar instead")]]
QRegExp GT_DATAMODEL_EXPORT umlautsAndSpecialChar();

/**
 * @brief onlyLetters - accepts all letter (a-z and A-Z) with a
 * minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLetters instead")]]
QRegExp GT_DATAMODEL_EXPORT onlyLetters();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbers instead")]]
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbers();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1 and dot
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndDot instead")]]
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndDot();

/**
 * @brief onlyLettersAndNumbersAndSpace - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ and space with a minimal length of 1
 * @return
 */
[[deprecated ("Use gt::rex::onlyLettersAndNumbersAndSpace instead")]]
QRegExp GT_DATAMODEL_EXPORT onlyLettersAndNumbersAndSpace();

/**
 * @brief forUnits - accepts all letters and numbers and some
 * special symbols typical used for units
 * @return
 */
[[deprecated ("Use gt::rex::forUnits instead")]]
QRegExp GT_DATAMODEL_EXPORT forUnits();

/**
 * @brief forExpressions
 * @return
 */
[[deprecated ("Use gt::rex::forExpressions instead")]]
QRegExp GT_DATAMODEL_EXPORT forExpressions();

/**
 * @brief forStations - accepts a combination
 * of an S and one to three numbers
 * @return
 */
[[deprecated ("Use gt::rex::forStations instead")]]
QRegExp GT_DATAMODEL_EXPORT forStations();

/**
 * @brief forDoubles
 * @return
 */
[[deprecated ("Use gt::rex::forDoubles instead")]]
QRegExp GT_DATAMODEL_EXPORT forDoubles();

/**
 * @brief forDoublesLists
 * @return regexp for double lists
 */
[[deprecated ("Use gt::rex::forDoublesLists instead")]]
QRegExp GT_DATAMODEL_EXPORT forDoublesLists();

/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 */
[[deprecated ("Use gt::rex::forSemVers instead")]]
QRegExp GT_DATAMODEL_EXPORT forSemVers();

/**
 * @brief Matches file filter strings, e.g.
 *
 * "Python (python* python3)"
 * "Textfiles (*.txt)"
 */
[[deprecated ("Use gt::rex::forFileDialogFilters instead")]]
GT_DATAMODEL_EXPORT const QRegExp& forFileDialogFilters();

} // namespace re

} // namespace gt

#endif // GTREGEXP_H
