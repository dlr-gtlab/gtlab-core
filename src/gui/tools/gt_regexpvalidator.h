/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 10.10.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTREGEXPVALIDATOR_H
#define GTREGEXPVALIDATOR_H

#include <QValidator>

class GtRegExpValidator : public QValidator
{
public:
    explicit GtRegExpValidator(QObject* parent = nullptr);

    explicit GtRegExpValidator(const QRegExp& rx, QObject* parent = nullptr);

    QValidator::State validate(QString& input, int& pos) const override;

private:
    QRegExp m_regExp;
};

#endif // GTREGEXPVALIDATOR_H
