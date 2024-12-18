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
#include <QObject>

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

QString
gt::re::onlyLettersHint()
{
    return QObject::tr("Only letters are allowed");
}

QRegExp
gt::re::onlyLettersAndNumbers()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\[\\]]+"));
}

QString
gt::re::onlyLettersAndNumbersHint()
{
    return QObject::tr("Only letters and numbers are allowed");
}

QRegExp
gt::re::onlyLettersAndNumbersAndDot()
{
    return QRegExp(("[A-Za-z0-9\\.\\_\\-\\[\\]]+"));
}

QString
gt::re::onlyLettersAndNumbersAndDotHint()
{
    return QObject::tr("Only letters, numbers, dots, hyphen and underscore are allowed");
}

QRegExp
gt::re::onlyLettersAndNumbersAndSpace()
{
    return QRegExp(("[A-Za-z0-9\\_\\-\\[\\]\\s\\␣]+"));
}

QString
gt::re::onlyLettersAndNumbersAndSpaceHint()
{
    return QObject::tr("Only letters, numbers, space, hyphen and underscore are allowed");
}

QRegExp
gt::re::forUnits()
{
    return QRegExp("[A-Za-z0-9\\_\\-\\+\\^\\°\\%\\/]*");
}

QRegExp
gt::re::forExpressions()
{
    return QRegExp("[A-Za-z0-9\\_\\-\\+\\^\\°\\/\\*\\.\\,\\(\\)\\[\\]]*");
}

QRegExp
gt::re::forStations()
{
    return QRegExp("S[0-9]{1,3}");
}

QRegExp
gt::re::forDoubles()
{
    return QRegExp("-?[0-9]+.*[E,e]?-?[0-9]*");
}

QRegExp
gt::re::forDoublesLists()
{
    return QRegExp("[eE0-9\\-\\.\\;]+");
}

QRegExp
gt::re::forSemVers()
{
    return QRegExp(R"((0|[1-9]\d*)(\.(0|[1-9]\d*)(\.(0|[1-9]\d*))?)?(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))"
                                         R"((?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?)");
}

const QRegExp&
gt::re::forFileDialogFilters()
{
    static auto r = QRegExp(R"(([a-z,A-Z,0-9,_,-]+)" // check for a string
                            R"([\s]*[(][a-z,A-Z,0-9,_,-,*,?,.]+)" // check for space + (string string)
                            R"(([\s]+[a-z,A-Z,0-9,_,-,*,?,.]+)*[)]))");

    return r;
}

QRegExp
gt::re::forHexColorCode()
{
    return QRegExp("^#([0-9a-fA-F]{3}|[0-9a-fA-F]{4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$");
}
