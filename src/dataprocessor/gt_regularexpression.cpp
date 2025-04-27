/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#include "gt_regularexpression.h"
#include <qregexp.h>

namespace gt
{

template <typename T, typename ...Args>
inline T& make_singleton(Args&&... args)
{
    static T instance(std::forward<Args>(args)...);
    return instance;
}

}


const QRegularExpression&
gt::rex::woUmlauts()
{
    return make_singleton<QRegularExpression>("[^äöüßÄÖÜ]*");
}


const QRegularExpression&
gt::rex::umlauts()
{
    return make_singleton<QRegularExpression>("[äöüßÄÖÜ]");
}


const QRegularExpression&
gt::rex::umlautsAndSpecialChar()
{
    return make_singleton<QRegularExpression>("[äöüßÄÖÜ\\.\\:\\,\\;\\#\\?\\+\\~\\*\\/"
                    "\\&\\%\\$\\§\\!\"\\{\\}\\=\\`\\´\\'\\°\\^]");
}


const QRegularExpression&
gt::rex::onlyLetters()
{
    return make_singleton<QRegularExpression>("[A-Za-z]+");
}

QString gt::rex::onlyLettersHint()
{
    return QObject::tr("Only letters are allowed");
}

const QRegularExpression&
gt::rex::onlyLettersAndNumbers()
{
    return make_singleton<QRegularExpression>("[A-Za-z0-9\\_\\-\\[\\]]+");
}

QString
gt::rex::onlyLettersAndNumbersHint()
{
    return QObject::tr("Only letters and numbers are allowed");
}

const QRegularExpression&
gt::rex::onlyLettersAndNumbersAndDot()
{
    return make_singleton<QRegularExpression>("[A-Za-z0-9\\.\\_\\-\\[\\]]+");
}

QString
gt::rex::onlyLettersAndNumbersAndDotHint()
{
    return QObject::tr("Only letters, numbers, dots, hyphen and underscore are allowed");
}

const QRegularExpression&
gt::rex::onlyLettersAndNumbersAndSpace()
{
    return make_singleton<QRegularExpression>("[A-Za-z0-9\\_\\-\\[\\]\\s\\␣]+");
}

QString
gt::rex::onlyLettersAndNumbersAndSpaceHint()
{
    return QObject::tr("Only letters, numbers, space, hyphen and underscore are allowed");
}

const QRegularExpression&
gt::rex::forUnits()
{
    return make_singleton<QRegularExpression>("[A-Za-z0-9\\_\\-\\+\\^\\°\\%\\/]*");
}

const QRegularExpression&
gt::rex::forExpressions()
{
    return make_singleton<QRegularExpression>("[A-Za-z0-9\\_\\-\\+\\^\\°\\/\\*\\.\\,\\(\\)\\[\\]]*");
}

const QRegularExpression&
gt::rex::forStations()
{
    return make_singleton<QRegularExpression>("S[0-9]{1,3}");
}

const QRegularExpression&
gt::rex::forDoubles()
{
    return make_singleton<QRegularExpression>("-?(?:\\d+(\\.\\d*)?|\\.\\d+)"
                                              "([eE][+-]?\\d+)?");
}

const QRegularExpression&
gt::rex::forDoublesLists()
{
    return make_singleton<QRegularExpression>(
        "(-?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)"      // first double
        "(;(-?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?))*"  // subsequent doubles
    );
}

const QRegularExpression&
gt::rex::forSemVers()
{
    return make_singleton<QRegularExpression>(R"((0|[1-9]\d*)(\.(0|[1-9]\d*)(\.(0|[1-9]\d*))?)?(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))"
                   R"((?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?)");
}


const QRegularExpression&
gt::rex::forFileDialogFilters()
{
    return make_singleton<QRegularExpression>(
        R"(([a-z,A-Z,0-9,_,-]+)" // check for a string
        R"([\s]*[(][a-z,A-Z,0-9,_,-,*,?,.]+)" // check for space + (string string)
        R"(([\s]+[a-z,A-Z,0-9,_,-,*,?,.]+)*[)]))");
}

const QRegularExpression&
gt::rex::forHexColorCode()
{
    return make_singleton<QRegularExpression>(
        "^#([0-9a-fA-F]{3}|[0-9a-fA-F]{4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$");
}
