/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_regexp.cpp
 *
 *  Created on: 05.12.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_regexp.h"

#include "gt_regularexpression.h"

namespace
{

    QRegExp
    toRegExp(const QRegularExpression &re)
    {
        QRegExp legacy(re.pattern());

        if (re.patternOptions() & QRegularExpression::CaseInsensitiveOption)
        {
            legacy.setCaseSensitivity(Qt::CaseInsensitive);
        }
        else
        {
            legacy.setCaseSensitivity(Qt::CaseSensitive);
        }

        // Enable minimal matching if set
        if (re.patternOptions() & QRegularExpression::InvertedGreedinessOption)
        {
            legacy.setMinimal(true);
        }

        return legacy;
    }
}

QRegExp
gt::re::woUmlauts()
{
    return toRegExp(gt::rex::woUmlauts());
}


QRegExp
gt::re::umlauts()
{
    return toRegExp(gt::rex::umlauts());
}

QRegExp
gt::re::umlautsAndSpecialChar()
{
    return toRegExp(gt::rex::umlautsAndSpecialChar());
}

QRegExp
gt::re::onlyLetters()
{
    return toRegExp(gt::rex::onlyLetters());
}

QRegExp
gt::re::onlyLettersAndNumbers()
{
    return toRegExp(gt::rex::onlyLettersAndNumbers());
}

QRegExp
gt::re::onlyLettersAndNumbersAndDot()
{
    return toRegExp(gt::rex::onlyLettersAndNumbersAndDot());
}

QRegExp
gt::re::onlyLettersAndNumbersAndSpace()
{
    return toRegExp(gt::rex::onlyLettersAndNumbersAndSpace());
}

QRegExp
gt::re::forUnits()
{
    return toRegExp(gt::rex::forUnits());
}

QRegExp
gt::re::forExpressions()
{
    return toRegExp(gt::rex::forExpressions());
}

QRegExp
gt::re::forStations()
{
    return toRegExp(gt::rex::forStations());
}

QRegExp
gt::re::forDoubles()
{
    return toRegExp(gt::rex::forDoubles());
}

QRegExp
gt::re::forDoublesLists()
{
    return toRegExp(gt::rex::forDoublesLists());
}

QRegExp
gt::re::forSemVers()
{
    return toRegExp(gt::rex::forSemVers());
}

const QRegExp&
gt::re::forFileDialogFilters()
{
    static QRegExp exp = toRegExp(gt::rex::forFileDialogFilters());
    return exp;
}
