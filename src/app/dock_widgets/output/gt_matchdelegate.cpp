/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_matchdelegate.h"
#include "gt_logmodel.h"

#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QStyleOption>
#include <QTableView>
#include <QTextFormat>
#include <QTextLayout>
#include <QVector>

namespace
{

/**
 * @brief Anzahl der eingefügten Zeilenumbrüche, die die Originalposition
 *        `originalPos` verschieben. Ein Umbruch an Position p verschiebt
 *        alle Zeichen ab p (bei includeEqual auch p selbst).
 */
int
countWrapShift(const QVector<int>& wrapPositions,
               int originalPos,
               bool includeEqual)
{
    int shift = 0;
    for (const int wrap : wrapPositions)
    {
        const bool shifts = includeEqual ? (wrap <= originalPos)
                                         : (wrap < originalPos);
        if (!shifts)
        {
            break;
        }
        ++shift;
    }
    return shift;
}

/**
 * @brief Text an `width` umbrechen (Wortgrenzen; Wörter, die länger als
 *        die gesamte Zeile sind, werden zeichenweise zerlegt).
 *        `wrapPositions` (optional) sammelt die Originalpositionen, an
 *        denen jeweils ein Zeilenumbruch eingefügt wurde, damit Match-
 *        Positionen auf den umgebrochenen Text umgerechnet werden können.
 */
QString
wrapTextAtWidth(const QString& text,
                const QFontMetrics& metrics,
                int width,
                QVector<int>* wrapPositions)
{
    if (width <= 0 || text.isEmpty())
    {
        return text;
    }

    const auto widthOf = [&metrics](const QString& str) -> int {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        return metrics.horizontalAdvance(str);
#else
        return metrics.width(str);
#endif
    };
    const int spaceWidth = widthOf(QStringLiteral(" "));

    QString wrapped;
    wrapped.reserve(text.size());

    int pos = 0;
    const int n = static_cast<int>(text.size());
    int lineWidth = 0; // bisherige Breite der aktuellen Zeile

    const auto newline = [&wrapped, wrapPositions, &lineWidth](int at) {
        wrapped.append(QLatin1Char('\n'));
        if (wrapPositions != nullptr)
        {
            wrapPositions->append(at);
        }
        lineWidth = 0;
    };

    while (pos < n)
    {
        // Zeilenumbruch in der Meldung: so beibehalten
        if (text.at(pos) == QLatin1Char('\n'))
        {
            wrapped.append(QLatin1Char('\n'));
            pos = pos + 1;
            lineWidth = 0;
            continue;
        }

        // nächstes Wort (bis Leerzeichen oder Zeilenumbruch)
        int wordEnd = pos;
        while (wordEnd < n
               && text.at(wordEnd) != QLatin1Char(' ')
               && text.at(wordEnd) != QLatin1Char('\n'))
        {
            ++wordEnd;
        }

        const QString word = text.mid(pos, wordEnd - pos);
        const int wordWidth = widthOf(word);
        const int candidateWidth =
            (lineWidth > 0 ? lineWidth + spaceWidth : 0) + wordWidth;

        if (candidateWidth > width && lineWidth > 0)
        {
            // Wort passt nicht auf die aktuelle Zeile
            newline(pos);
        }

        if ((lineWidth > 0 ? lineWidth + spaceWidth : 0) + wordWidth > width)
        {
            // Wort ist länger als eine ganze Zeile: zeichenweise umbrechen
            QString chunk;
            int chunkStart = pos;
            for (int i = 0; i < word.size(); ++i)
            {
                if (!chunk.isEmpty() && widthOf(chunk + word.at(i)) > width)
                {
                    wrapped.append(chunk);
                    newline(chunkStart + chunk.size());
                    chunkStart = chunkStart + chunk.size();
                    chunk.clear();
                }
                chunk.append(word.at(i));
            }
            wrapped.append(chunk);
            lineWidth = widthOf(chunk);
        }
        else
        {
            // Wort auf die aktuelle Zeile setzen
            if (lineWidth > 0)
            {
                wrapped.append(QLatin1Char(' '));
                lineWidth += spaceWidth;
            }
            wrapped.append(word);
            lineWidth += wordWidth;
        }

        pos = wordEnd;
        // Leerzeichen nach dem Wort zum Umbruchpunkt dazu zählen
        while (pos < n && text.at(pos) == QLatin1Char(' '))
        {
            ++pos;
        }
    }

    return wrapped;
}

} // namespace

GtMatchDelegate::GtMatchDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void
GtMatchDelegate::paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    /// clear initial
    QString text = opt.text;
    opt.text.clear();

    const QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

    const auto matchIt = m_matches.constFind(index);

    // eigene Selection-Logik
    if (matchIt != m_matches.constEnd() && (opt.state & QStyle::State_Selected))
    {
        opt.state &= ~QStyle::State_Selected;
    }

    // alles außer Text zeichnen
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // komplette Zeile markieren
    if (matchIt != m_matches.constEnd())
    {
        QColor color = opt.palette.highlight().color();
        color.setAlpha(100);

        painter->fillRect(opt.rect, color);
    }

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt,
                                           opt.widget);

    // Textfarbe: immer die Level-Farbe aus dem Model, auch bei Selektion
    QColor textColor = index.data(Qt::ForegroundRole).value<QColor>();
    if (!textColor.isValid())
    {
        textColor = option.palette.color(QPalette::Text);
    }

    // Text auf den Textbereich der Zelle beschränken, damit Überlauf
    // (z. B. sehr lange unumbrichbare Zeichenketten) benachbarte
    // Zeilen und Spalten nicht überdeckt
    painter->save();
    painter->setClipRect(textRect);

    drawHighlightedText(painter, textRect, opt, text, textColor,
        matchIt != m_matches.constEnd()
            ? matchIt.value()
            : GtOutputDock::Matches{});

    painter->restore();
}

void
GtMatchDelegate::setMatches(const QHash<QPersistentModelIndex,
                                        GtOutputDock::Matches>& matches)
{
    m_matches = matches;
}

QSize
GtMatchDelegate::sizeHint(const QStyleOptionViewItem& option,
                          const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    // Die View fragt die Zeilenhöhe über die erste Spalte ab (Icon-Spalte
    // ohne Text), größenentscheidend ist aber die Textspalte -> dort
    // Text und Breite verwenden
    QString text = opt.text;
    int column = index.column();
    if (text.isEmpty() && index.isValid() && index.model())
    {
        column = GtLogModel::columnFromRole(GtLogModel::MessageRole);
        text = index.model()->index(index.row(), column)
                   .data(Qt::DisplayRole).toString();
    }
    if (text.isEmpty())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    const QStyle* style = opt.widget ? opt.widget->style()
                                     : QApplication::style();

    // Zellgröße der betrachteten Spalte
    int cellWidth = opt.rect.width();
    int rowHeight = opt.rect.height();
    if (const auto* table = qobject_cast<const QTableView*>(opt.widget))
    {
        //cellWidth = table->columnWidth(messageColumn);
        rowHeight = table->rowHeight(index.row());
    }
    if (cellWidth <= 0 || rowHeight <= 0)
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    // Textbereich mit den stilabhängigen Einrückungen bestimmen
    opt.rect = QRect(0, 0, cellWidth, rowHeight);
    const QRect textRect =
        style->subElementRect(QStyle::SE_ItemViewItemText, &opt);
    if (textRect.width() <= 0)
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    // Schneller Fall: Text passt ohne Umbruch auf eine Zeile
    int textWidth;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    textWidth = opt.fontMetrics.horizontalAdvance(text);
#else
    textWidth = opt.fontMetrics.width(text);
#endif
    if (!text.contains(QLatin1Char('\n')) && textWidth <= textRect.width())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    // Zeilenanzahl des umgebrochenen Texts bestimmen (gleiche
    // Umbruchlogik wie in paint())
    const QString wrappedText =
        wrapTextAtWidth(text, opt.fontMetrics, textRect.width(),
                        nullptr);
    const int lineCount = wrappedText.count(QLatin1Char('\n')) + 1;

    // vertikale Einrückung des Textbereichs berücksichtigen
    // (+ 1px Sicherheitsabstand, damit die letzte Zeile nicht abgeschnitten wird)
    const int vInset = rowHeight - textRect.height();

    return QSize(cellWidth, lineCount * opt.fontMetrics.height() + vInset + 1);
}

void
GtMatchDelegate::drawHighlightedText(QPainter* painter,
                                    const QRect& rect,
                                    const QStyleOptionViewItem& option,
                                    const QString& text,
                                    const QColor& textColor,
                                    const GtOutputDock::Matches& matches) const
{
    // langen Text an der Breite des Textbereichs umbrechen, damit
    // jede Zeile in die Spalte passt
    const QFontMetrics metrics(option.font);
    QVector<int> wrapPositions;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const int textWidth = metrics.horizontalAdvance(text);
#else
    const int textWidth = metrics.width(text);
#endif
    const QString wrappedText =
        (!text.contains(QLatin1Char('\n')) && textWidth <= rect.width())
            ? text
            : wrapTextAtWidth(text, metrics, rect.width(), &wrapPositions);

    QVector<QTextLayout::FormatRange> ranges;
    ranges.reserve(matches.size());

    for (const auto& match : matches)
    {
        if (match.length <= 0)
        {
            continue;
        }

        // Match-Positionen auf den umgebrochenen Text umrechnen
        // (jeder eingefügte Zeilenumbruch verschiebt die folgenden
        // Positionen um ein Zeichen)
        const int start = static_cast<int>(match.start);
        const int end = start + static_cast<int>(match.length);

        QTextLayout::FormatRange range;
        range.start = start + countWrapShift(wrapPositions, start, true);
        range.length = end + countWrapShift(wrapPositions, end, false)
                        - range.start;

        QTextCharFormat format;
        QColor highlightColor(128, 128, 128);
        highlightColor.setAlpha(120);
        format.setBackground(highlightColor);

        range.format = format;
        ranges.push_back(range);
    }

    QTextLayout layout(wrappedText, option.font);
    layout.setFormats(ranges);

    // alle Zeilen der Meldung layouten (unterstützt Mehrzeiler) und
    // Gesamthöhe für die vertikale Zentrierung bestimmen
    qreal totalHeight = 0.0;
    layout.beginLayout();
    while (true)
    {
        QTextLine line = layout.createLine();
        if (!line.isValid())
        {
            break;
        }
        // Sicherheit: Zeilen, die durch Rundungen knapp über der
        // Breite lägen, werden sauber gestutzt statt zu überquellen
        line.setLineWidth(rect.width());
        totalHeight += line.height();
    }
    layout.endLayout();

    // Textfarbe setzen, da QTextLayout den Text andernfalls mit dem
    // aktuellen Pen des Painters (Standard: schwarz) zeichnet
    painter->setPen(textColor);

    QPointF pos = rect.topLeft();
    pos.setY(pos.y() + qMax(0.0, (rect.height() - totalHeight) / 2.0));

    layout.draw(painter, pos);
}
