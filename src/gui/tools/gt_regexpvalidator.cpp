/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 10.10.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_regexpvalidator.h"

#include <gt_logging.h>

GtRegExpValidator::GtRegExpValidator(const QRegExp& regExp,
                                     bool strict,
                                     QObject* parent) :
    QValidator(parent),
    m_regExp(regExp),
    m_strict(strict)
{
}

const QRegExp&
GtRegExpValidator::regExp()
{
    return m_regExp;
}

QValidator::State
GtRegExpValidator::validate(QString& input, int& pos) const
{
    if (m_regExp.exactMatch(input)) return QValidator::Acceptable;

    QValidator::State retVal;

    if (input.isEmpty()) return QValidator::Intermediate;

    if (m_strict) retVal = QValidator::Invalid;
    else retVal = QValidator::Intermediate;

    return retVal;
}
