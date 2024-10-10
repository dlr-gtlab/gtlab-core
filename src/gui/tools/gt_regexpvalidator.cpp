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

GtRegExpValidator::GtRegExpValidator(QObject* parent) :
    QValidator(parent),
    m_regExp(".*")
{

}

GtRegExpValidator::GtRegExpValidator(const QRegExp& regExp,
                                     bool strict, const QString &hint,
                                     QObject* parent) :
    QValidator(parent),
    m_regExp(regExp),
    m_strict(strict),
    m_hint(hint)
{   
}

QValidator::State
GtRegExpValidator::validate(QString& input, int& pos) const
{
    if (m_regExp.exactMatch(input)) return QValidator::Acceptable;

    if (m_strict)
    {
        gtLogOnce(Warning) << tr("Failure while renaming: %1").arg(m_hint);
        return QValidator::Invalid;
    }
    else
    {
        if (input.isEmpty()) return QValidator::Intermediate;

        gtLogOnce(Warning) << tr("Failure while renaming: %1").arg(m_hint);
        return QValidator::Intermediate;
    }
}
