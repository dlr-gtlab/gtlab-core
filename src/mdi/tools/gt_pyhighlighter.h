/* GTlab - Gas Turbine laboratory
 * Source File: gt_pyhighlighter.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.04.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PYHIGHLIGHTER_H
#define GT_PYHIGHLIGHTER_H

#include "gt_mdi_exports.h"

#include <QSyntaxHighlighter>

//! Container to describe a highlighting rule. Based on a regular expression, a relevant match # and the format.
class GtHighlightingRule
{
public:
    GtHighlightingRule(const QString& patternStr, int n,
                       const QTextCharFormat& matchingFormat)
    {
        originalRuleStr = patternStr;
        pattern = QRegExp(patternStr);
        nth = n;
        format = matchingFormat;
    }

    QString originalRuleStr;

    QRegExp pattern;

    int nth;

    QTextCharFormat format;
};

/**
 * @brief The GtPyHighlighter class
 */
class GT_MDI_EXPORT GtPyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    GtPyHighlighter(QTextDocument* parent = Q_NULLPTR);

protected:
    void highlightBlock(const QString& text);

private:
    QStringList keywords;

    QStringList operators;

    QStringList braces;

    QHash<QString, QTextCharFormat> basicStyles;

    void initializeRules();

    //! Highlighst multi-line strings, returns true if after processing we are still within the multi-line section.
    bool matchMultiline(const QString& text, const QRegExp& delimiter,
                        const int inState, const QTextCharFormat& style);

    const QTextCharFormat getTextCharFormat(const QString& colorName,
                                            const QString& style = QString());

    QList<GtHighlightingRule> rules;

    QRegExp triSingleQuote;

    QRegExp triDoubleQuote;
};

#endif // GT_PYHIGHLIGHTER_H
