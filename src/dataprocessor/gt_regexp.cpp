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

void
gt::re::restrictRegExpWithObjectNames(const QStringList& namesToProhibit,
                                      QRegExp& defaultRegExp)
{
    QString forbiddenPattern = "(?!";
    for (int i = 0; i < namesToProhibit.size(); ++i)
    {
        forbiddenPattern += namesToProhibit[i] + "$";
        if (i < namesToProhibit.size() - 1)
        {
            forbiddenPattern += "|";
        }
    }
    forbiddenPattern += ")";

    // Ergänze die verbotenen Wörter zur Basis-Regex
    QString finalPattern = "^" + forbiddenPattern
                           + defaultRegExp.pattern() + "$";

    defaultRegExp = QRegExp(finalPattern);
}

QRegExp
gt::re_deprecated::toRegExp(const QRegularExpression &re)
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
