/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_pyhighlighter.cpp
 *
 *  Created on: 28.04.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_pyhighlighter.h"
#include "gt_application.h"

GtPyHighlighter::GtPyHighlighter(QTextDocument* parent) :
    QSyntaxHighlighter(parent),
    keywords({"and", "assert", "break", "class", "continue", "def",
             "del", "elif", "else", "except", "exec", "finally",
             "for", "from", "global", "if", "import", "in" ,
             "is", "lambda", "not", "or", "pass", "print", "raise",
             "return", "try", "while", "yield", "None", "True", "False"}),
    operators({"=",
              // Comparison
              "==", "!=", "<", "<=", ">", ">=",
              // Arithmetic
              "\\+", "-", "\\*", "/", "//", "%", "\\*\\*",
              // In-place
              "\\+=", "-=", "\\*=", "/=", "%=",
              // Bitwise
              "\\^", "\\|", "&", "~", ">>", "<<"
              }),
    braces({"{", "}", "\\(", "\\)", "\\[", "\\]"})

{
    initializeStyles();

    triSingleQuote.setPattern("'''");
    triDoubleQuote.setPattern("\"\"\"");

    initializeRules();
}

void
GtPyHighlighter::initializeRules()
{
    foreach (const QString& currKeyword, keywords)
    {
        rules.append(GtHighlightingRule(QString("\\b%1\\b").arg(currKeyword), 0,
                                        basicStyles.value("keyword")));
    }

    foreach (const QString& currOperator, operators)
    {
        rules.append(GtHighlightingRule(QString("%1").arg(currOperator), 0,
                                        basicStyles.value("operator")));
    }

    foreach (const QString& currBrace, braces)
    {
        rules.append(GtHighlightingRule(QString("%1").arg(currBrace), 0,
                                        basicStyles.value("brace")));
    }

    // 'self'
    rules.append(GtHighlightingRule("\\bself\\b", 0,
                                    basicStyles.value("self")));

    // Double-quoted string, possibly containing escape sequences
    // FF: originally in python : r'"[^"\\]*(\\.[^"\\]*)*"'
    rules.append(GtHighlightingRule("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"", 0,
                                    basicStyles.value("string")));
    // Single-quoted string, possibly containing escape sequences
    // FF: originally in python : r"'[^'\\]*(\\.[^'\\]*)*'"
    rules.append(GtHighlightingRule("'[^'\\\\]*(\\\\.[^'\\\\]*)*'", 0,
                                    basicStyles.value("string")));

    // 'def' followed by an identifier
    // FF: originally: r'\bdef\b\s*(\w+)'
    rules.append(GtHighlightingRule("\\bdef\\b\\s*(\\w+)", 1,
                                    basicStyles.value("defclass")));
    //  'class' followed by an identifier
    // FF: originally: r'\bclass\b\s*(\w+)'
    rules.append(GtHighlightingRule("\\bclass\\b\\s*(\\w+)", 1,
                                    basicStyles.value("defclass")));

    // From '#' until a newline
    // FF: originally: r'#[^\\n]*'
    rules.append(GtHighlightingRule("#[^\\n]*", 0,
                                    basicStyles.value("comment")));

    // Numeric literals
    rules.append(GtHighlightingRule("\\b[+-]?[0-9]+[lL]?\\b", 0,
                                    basicStyles.value("numbers")));
                                    // r'\b[+-]?[0-9]+[lL]?\b'

    rules.append(GtHighlightingRule(
                     "\\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\\b", 0,
                     basicStyles.value("numbers")));
                     // r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b'

    rules.append(GtHighlightingRule(
                     "\\b[+-]?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b", 0,
                     basicStyles.value("numbers")));
                    // r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b'

    QTextCharFormat spaceFormat;
    spaceFormat.setForeground(Qt::lightGray);
    rules.append(GtHighlightingRule(" ", 0, spaceFormat));
    rules.append(GtHighlightingRule("	", 0, spaceFormat));
}

void
GtPyHighlighter::highlightBlock(const QString& text)
{
    foreach (const GtHighlightingRule& currRule, rules)
    {
        int idx = currRule.pattern.indexIn(text, 0);

        while (idx >= 0)
        {
            // Get index of Nth match
            idx = currRule.pattern.pos(currRule.nth);
            int length = currRule.pattern.cap(currRule.nth).length();
            setFormat(idx, length, currRule.format);
            idx = currRule.pattern.indexIn(text, idx + length);
        }
    }

    setCurrentBlockState(0);

    // Do multi-line strings
    bool isInMultilne = matchMultiline(text, triSingleQuote, 1,
                                       basicStyles.value("string2"));

    if (!isInMultilne)
    {
        matchMultiline(text, triDoubleQuote, 2, basicStyles.value("string2"));
    }
}

void
GtPyHighlighter::onThemeChanged()
{
    initializeStyles();
    initializeRules();
    rehighlight();
}

void
GtPyHighlighter::initializeStyles()
{
    if (!gtApp->inDarkMode())
    {
        basicStyles.insert("keyword", getTextCharFormat("blue"));
        basicStyles.insert("operator", getTextCharFormat("red"));
        basicStyles.insert("brace", getTextCharFormat("darkGray"));
        basicStyles.insert("defclass", getTextCharFormat("black", "bold"));
        basicStyles.insert("string", getTextCharFormat("magenta"));
        basicStyles.insert("string2", getTextCharFormat("darkMagenta"));
        basicStyles.insert("comment", getTextCharFormat("darkGreen", "italic"));
        basicStyles.insert("self", getTextCharFormat("black", "italic"));
        basicStyles.insert("numbers", getTextCharFormat("brown"));
    }
    else
    {
        basicStyles.insert("keyword", getTextCharFormat("violet"));
        basicStyles.insert("operator", getTextCharFormat("white"));
        basicStyles.insert("brace", getTextCharFormat("white"));
        basicStyles.insert("defclass", getTextCharFormat("cyan", "bold"));
        basicStyles.insert("string", getTextCharFormat("palegreen"));
        basicStyles.insert("string2", getTextCharFormat("palegreen"));
        basicStyles.insert("comment", getTextCharFormat("gray", "italic"));
        basicStyles.insert("self", getTextCharFormat("white", "italic"));
        basicStyles.insert("numbers", getTextCharFormat("yellow"));
    }
}

bool
GtPyHighlighter::matchMultiline(const QString& text,
                                const QRegExp& delimiter,
                                const int inState,
                                const QTextCharFormat& style)
{
    int start = -1;
    int add = -1;

    // If inside triple-single quotes, start at 0
    if (previousBlockState() == inState)
    {
        start = 0;
        add = 0;
    }
    // Otherwise, look for the delimiter on this line
    else
    {
        start = delimiter.indexIn(text);
        // Move past this match
        add = delimiter.matchedLength();
    }

    // As long as there's a delimiter match on this line...
    while (start >= 0)
    {
        // Look for the ending delimiter
        int end = delimiter.indexIn(text, start + add);

        int length = 0;

        // Ending delimiter on this line?
        if (end >= add)
        {
            length = end - start + add + delimiter.matchedLength();
            setCurrentBlockState(0);
        }
        // No; multi-line string
        else
        {
            setCurrentBlockState(inState);
            length = text.length() - start + add;
        }

        // Apply formatting and look for next
        setFormat(start, length, style);
        start = delimiter.indexIn(text, start + length);
    }

    // Return True if still inside a multi-line string, False otherwise
    return (currentBlockState() == inState);
}

const QTextCharFormat
GtPyHighlighter::getTextCharFormat(const QString& colorName,
                                   const QString& style)
{
    QTextCharFormat charFormat;
    QColor color(colorName);
    charFormat.setForeground(color);

    if (style.contains("bold", Qt::CaseInsensitive))
    {
        charFormat.setFontWeight(QFont::Bold);
    }

    if (style.contains("italic", Qt::CaseInsensitive))
    {
        charFormat.setFontItalic(true);
    }

    return charFormat;
}
