/* GTlab - Gas Turbine laboratory
 * Source File: gt_regexp.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.12.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_regexp.h"


QRegExp
gt::re::woUmlauts()
{
    return QRegExp(("[^äöüßÄÖÜ]*"));
}

QRegExp
gt::re::umlauts()
{
    return QRegExp(("[äöüßÄÖÜ]"));
}

QRegExp
gt::re::umlautsAndSpecialChar()
{
    return QRegExp(("[äöüßÄÖÜ\\.\\:\\,\\;\\#\\?\\+\\~\\*\\/"
                    "\\&\\%\\$\\§\\!\"\\{\\}\\=\\`\\´\\'\\°\\^]"));
}

QRegExp
gt::re::onlyLetters()
{
    return QRegExp(("[A-Za-z]+"));
}

QRegExp
gt::re::onlyLettersAndNumbers()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\[\\]]+"));
}

QRegExp
gt::re::onlyLettersAndNumbersAndDot()
{
    return QRegExp(("[A-Za-z0-9\\.\\_\\-\\[\\]]+"));
}

QRegExp
gt::re::onlyLettersAndNumbersAndSpace()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\[\\]\\s\\␣]+"));
}

QRegExp
gt::re::forUnits()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\+\\^\\°\\%\\/]*"));
}

QRegExp
gt::re::forExpressions()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\+\\^\\°\\/\\*\\.\\,\\(\\)\\[\\]]*"));
}

QRegExp
gt::re::forStations()
{
    return QRegExp(("S[0-9]{1,3}"));
}

QRegExp
gt::re::forDoubles()
{
    return QRegExp(("-?[0-9]+.*[E,e]?-?[0-9]*"));
}

QRegExp
gt::re::forDoublesLists()
{
    return QRegExp(("[eE0-9\\-\\.\\;]+"));
}

QRegExp
gt::re::forSemVers()
{
    return QRegExp(R"(^(0|[1-9]\d*)(\.(0|[1-9]\d*)(\.(0|[1-9]\d*))?)?(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))"
                                         R"((?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)");
}

const QRegularExpression&
gt::re::forFileDialogFilters()
{
    static auto r = []() {
        QRegularExpression r(R"(([a-z,A-Z,0-9,_,-]+)" // check for a string
            R"([\s]*[(][a-z,A-Z,0-9,_,-,*,?,.]+)" // check for space + (string string)
            R"(([\s]+[a-z,A-Z,0-9,_,-,*,?,.]+)*[)]))");
        r.optimize();
        return r;
    }();

    return r;
}
