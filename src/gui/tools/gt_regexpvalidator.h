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

/**
 * @brief The GtRegExpValidator class
 * A GTlab specific Validator for regular expressions.
 * Main difference to the QRegExpValidator is the behaviour to allow
 * more intermediate states
 */
class GtRegExpValidator : public QValidator
{
public:
    /**
     * @brief GtRegExpValidator
     * @param regExp - regular expression to use
     * @param strict - flag if the check should be as strict as in the
     * QRegExpValidator or with the check only at the end of change
     * @param parent
     */
    GtRegExpValidator(const QRegExp& regExp, bool strict,
                      QObject* parent = nullptr);

    const QRegExp& regExp();

private:
    /**
     * @brief validate - validation mlementation
     * @param input - input string reference to check
     * @param pos - position of the curser
     * @return the current validation state
     */
    QValidator::State validate(QString& input, int& pos) const override;

    QRegExp m_regExp;

    bool m_strict;
};

#endif // GTREGEXPVALIDATOR_H
