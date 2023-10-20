/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_jshighlighter.h
 *
 *  Created on: 25.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTJSHIGHLIGHTER_H
#define GTJSHIGHLIGHTER_H

#include "gt_gui_exports.h"

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QTextEdit>
#include <QHash>

/**
 * @brief The GtJsHighlighter class
 */
class GT_GUI_EXPORT GtJsHighlighter : public QSyntaxHighlighter
{
public:
    typedef enum
    {
        Background,
        Normal,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        Sidebar,
        LineNumber,
        Cursor,
        Marker,
        BracketMatch,
        BracketError,
        FoldIndicator
    } ColorComponent;

    explicit GtJsHighlighter(QObject* parent);
    explicit GtJsHighlighter(QTextDocument* parent);
    explicit GtJsHighlighter(QTextEdit* parent);

    void setColor(GtJsHighlighter::ColorComponent component,
                  const QColor& color);

    void mark(const QString& str, Qt::CaseSensitivity caseSensitivity);

    QStringList keywords() const;

    void setKeywords(const QStringList& keywords);

protected:
    void highlightBlock(const QString& text) override;

private:
    QSet<QString> m_keywords;

    QSet<QString> m_knownIds;

    QHash<GtJsHighlighter::ColorComponent, QColor> m_colors;

    QString m_markString;

    Qt::CaseSensitivity m_markCaseSensitivity;

    void init();

};


#endif // GTJSHIGHLIGHTER_H
