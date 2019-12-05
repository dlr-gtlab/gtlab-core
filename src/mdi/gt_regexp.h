/* GTlab - Gas Turbine laboratory
 * Source File: gt_regexp.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.12.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTREGEXP_H
#define GTREGEXP_H

#include <QRegExp>

#include "gt_mdi_exports.h"

/**
 * namespace for RegualrExpressions, used in GTlab
 */
namespace GtRegExp
{
    /**
     * @brief woUmlauts - accepts all but ä ö ü ß
     * @return
     */
    QRegExp GT_MDI_EXPORT woUmlauts();

    /**
     * @brief umlauts - accept äöüßÄÖÜ
     * @return
     */
    QRegExp GT_MDI_EXPORT umlauts();

    /**
     * @brief umlautsAnsSpecialChar
     * - accepts äöüßÄÖÜ .:,;#?+~* /&%$§!"{}
     * @return
     */
    QRegExp GT_MDI_EXPORT umlautsAndSpecialChar();

    /**
     * @brief onlyLetters - accepts all letter (a-z and A-Z) with a
     * minimal length of 1
     * @return
     */
    QRegExp GT_MDI_EXPORT onlyLetters();

    /**
     * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
     * and numbers (0-9) and -  and _ with a minimal length of 1
     * @return
     */
    QRegExp GT_MDI_EXPORT onlyLettersAndNumbers();

    /**
     * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
     * and numbers (0-9) and -  and _ with a minimal length of 1 and dot
     * @return
     */
    QRegExp GT_MDI_EXPORT onlyLettersAndNumbersAndDot();

    /**
     * @brief onlyLettersAndNumbersAndSpace - accepts all letter (a-z and A-Z)
     * and numbers (0-9) and -  and _ and space with a minimal length of 1
     * @return
     */
    QRegExp GT_MDI_EXPORT onlyLettersAndNumbersAndSpace();

    /**
     * @brief forUnits - accepts all letters and numbers and some
     * special symbols typical used for units
     * @return
     */
    QRegExp GT_MDI_EXPORT forUnits();

    /**
     * @brief forExpressions
     * @return
     */
    QRegExp GT_MDI_EXPORT forExpressions();

    /**
     * @brief forStations - accepts a combination
     * of an S and one to three numbers
     * @return
     */
    QRegExp GT_MDI_EXPORT forStations();

    /**
     * @brief forDoubles
     * @return
     */
    QRegExp GT_MDI_EXPORT forDoubles();

    /**
     * @brief forDoublesLists
     * @return
     */
    QRegExp GT_MDI_EXPORT forDoublesLists();
}

#endif // GTREGEXP_H
