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

GtRegExpValidator::GtRegExpValidator(QObject* parent) :
    QValidator(parent)
{

}

GtRegExpValidator::GtRegExpValidator(const QRegExp& rx, QObject* parent) :
    QValidator(parent)
{   
    // TODO: Check if there is the need to add a ^ at the start and $ at
    // the end of the regexp pattern

    m_regExp = rx;
}

QValidator::State
GtRegExpValidator::validate(QString& input, int& pos) const
{


    if (m_regExp.exactMatch(input)) return QValidator::Acceptable;
    else return QValidator::Intermediate;
}
