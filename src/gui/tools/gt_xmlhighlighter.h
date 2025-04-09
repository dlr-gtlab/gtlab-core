/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2023 Copyright (c) 2015 Dmitry Ivanov
 */

#ifndef GTXMLHIGHLIGHTER_H
#define GTXMLHIGHLIGHTER_H

#include "gt_gui_exports.h"

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QRegExp>

class GT_GUI_EXPORT GtXmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit GtXmlHighlighter(QObject * parent);
    explicit GtXmlHighlighter(QTextDocument * parent);
    explicit GtXmlHighlighter(QTextEdit * parent);

protected:
    void highlightBlock(const QString & text) override;

private:
    void highlightByRegex(const QTextCharFormat & format,
                          const QRegExp & regex, const QString & text);

    void setRegexes();
    void setFormats();

public slots:
    void onThemeChanged();

private:
    QTextCharFormat     m_xmlKeywordFormat;
    QTextCharFormat     m_xmlElementFormat;
    QTextCharFormat     m_xmlAttributeFormat;
    QTextCharFormat     m_xmlValueFormat;
    QTextCharFormat     m_xmlCommentFormat;

    QList<QRegExp>      m_xmlKeywordRegexes;
    QRegExp             m_xmlElementRegex;
    QRegExp             m_xmlAttributeRegex;
    QRegExp             m_xmlValueRegex;
    QRegExp             m_xmlCommentRegex;
};

#endif // GTXMLHIGHLIGHTER_H
