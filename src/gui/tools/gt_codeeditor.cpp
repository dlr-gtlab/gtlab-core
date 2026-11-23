/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMimeData>
#include <QContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <functional>

#include "gt_codeeditor.h"
#include "gt_codefoldmanager.h"
#include "gt_guiutilities.h"
#include "gt_linenumberarea.h"
#include "gt_colors.h"

GtCodeEditor::GtCodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    m_lineNumberArea = new GtLineNumberArea(this);

    m_foldManager = new GtFoldManager(document(), this);

    connect(this, SIGNAL(blockCountChanged(int)),
            SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)),
            SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()),
            SLOT(highlightCurrentLine()));

    connect(m_foldManager, &GtFoldManager::foldStateChanged, this, [this]()
    {
        updateLineNumberAreaWidth(0);
        viewport()->update();
        if (m_lineNumberArea != nullptr)
        {
            m_lineNumberArea->update();
        }
    });

    m_foldRecalcTimer = new QTimer(this);
    m_foldRecalcTimer->setSingleShot(true);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    m_foldRecalcTimer->setInterval(150);
    connect(m_foldRecalcTimer, &QTimer::timeout, this,
            [this]()
            {
                recalculateFoldRegions();
            });
    connect(this, &QPlainTextEdit::textChanged, this, [this]()
    {
        m_foldRecalcTimer->start();
    });

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setAcceptDrops(true);

    setLineWrapMode(QPlainTextEdit::NoWrap);
    setFrameStyle(QFrame::NoFrame);
}

int
GtCodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, this->blockCount());

    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    if (m_foldManager != nullptr && m_foldManager->hasRegions())
    {
        space += foldGutterWidth();
    }

    return space;
}

int
GtCodeEditor::foldGutterWidth()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return 14;
}

GtFoldManager*
GtCodeEditor::foldManager() const
{
    return m_foldManager;
}

QString
GtCodeEditor::fullText() const
{
    if (m_foldManager != nullptr)
    {
        return m_foldManager->fullText();
    }

    return toPlainText();
}

bool
GtCodeEditor::performFoldOp(const std::function<bool()>& operation)
{
    if (m_foldManager == nullptr || !m_foldManager->hasRegions())
    {
        return false;
    }

    const QTextCursor before = textCursor();
    const int logicalLine = m_foldManager->docToLogical(before.blockNumber());
    const int column = before.positionInBlock();
    const int topBefore = (int) blockBoundingGeometry(before.block())
                              .translated(contentOffset())
                              .top();

    const bool changed = operation();

    if (changed)
    {
        restoreCursorAfterFoldOp(logicalLine, column, topBefore);

        updateLineNumberAreaWidth(0);
        viewport()->update();

        if (m_lineNumberArea != nullptr)
        {
            m_lineNumberArea->update();
        }
    }

    return changed;
}

void
GtCodeEditor::restoreCursorAfterFoldOp(int logicalLine, int column,
                                       int topBefore)
{
    if (m_foldManager == nullptr)
    {
        return;
    }

    const int docLine = m_foldManager->logicalToDoc(logicalLine);

    const QTextBlock block = document()->findBlockByLineNumber(docLine);

    if (!block.isValid())
    {
        return;
    }

    QTextCursor cursor = textCursor();
    cursor.setPosition(block.position() + qMin(column, block.length()));
    setTextCursor(cursor);

    const int topAfter = (int) blockBoundingGeometry(block)
                             .translated(contentOffset())
                             .top();

    const int diff = topBefore - topAfter;

    if (diff != 0)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + diff);
    }
}

bool
GtCodeEditor::toggleFold(int docLine)
{
    return performFoldOp([this, docLine]()
                         {
                             return m_foldManager->toggleFold(docLine);
                         });
}

bool
GtCodeEditor::fold(int docLine)
{
    return performFoldOp([this, docLine]()
                         {
                             return m_foldManager->fold(docLine);
                         });
}

bool
GtCodeEditor::unfold(int docLine)
{
    return performFoldOp([this, docLine]()
                         {
                             return m_foldManager->unfold(docLine);
                         });
}

void
GtCodeEditor::foldAllRegions()
{
    performFoldOp([this]()
                  {
                      m_foldManager->foldAll();
                      return true;
                  });
}

void
GtCodeEditor::unfoldAllRegions()
{
    performFoldOp([this]()
                  {
                      m_foldManager->unfoldAll();
                      return true;
                  });
}

void
GtCodeEditor::toggleFoldAtCurrentLine()
{
    toggleFold(textCursor().blockNumber());
}

void
GtCodeEditor::foldAtCurrentLine()
{
    fold(textCursor().blockNumber());
}

void
GtCodeEditor::unfoldAtCurrentLine()
{
    unfold(textCursor().blockNumber());
}

void
GtCodeEditor::handleLineNumberAreaClick(const QPoint& pos)
{
    const bool foldActive = (m_foldManager != nullptr
                             && m_foldManager->hasRegions());

    const int gutter = foldActive ? foldGutterWidth() : 0;

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top =
        (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= pos.y())
    {
        if (block.isVisible() && bottom > pos.y())
        {
            if (foldActive && gutter > 0
                    && pos.x() >= m_lineNumberArea->width() - gutter
                    && m_foldManager->isFoldable(blockNumber))
            {
                toggleFold(blockNumber);
            }
            else
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::StartOfLine);
                setTextCursor(cursor);
            }

            return;
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void
GtCodeEditor::recalculateFoldRegions()
{
}

void
GtCodeEditor::paintEvent(QPaintEvent* event)
{
    QPlainTextEdit::paintEvent(event);

    if (m_foldManager == nullptr || !m_foldManager->hasFolds())
    {
        return;
    }

    QPainter painter(viewport());

    painter.setPen(gt::gui::color::code_editor::foldedMarker());

    const int blockCount = document()->blockCount();

    QTextBlock block = firstVisibleBlock();

    while (block.isValid() && block.isVisible())
    {
        const GtFoldManager::Span* span =
            m_foldManager->spanForDocBlock(block.blockNumber());

        if (span != nullptr)
        {
            QRect r = blockBoundingRect(block).toRect();
            r.translate(contentOffset().toPoint()
                        + QPoint(viewport()->contentsMargins().left(), 0));

            if (r.intersects(viewport()->rect()))
            {
                const int markerWidth = fontMetrics()
                                            .horizontalAdvance(
                                                QStringLiteral("…"));

                // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                painter.drawText(r.adjusted(r.width() - markerWidth - 2, 0,
                                            -2, 0),
                                 Qt::AlignRight | Qt::AlignVCenter,
                                 QStringLiteral("…"));
            }
        }

        block = block.next();
    }
}

void
GtCodeEditor::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = QPlainTextEdit::createStandardContextMenu();

    if (menu == nullptr)
    {
        QPlainTextEdit::contextMenuEvent(event);
        return;
    }

    if (m_foldManager != nullptr && m_foldManager->hasRegions())
    {
        menu->addSeparator();
        menu->addAction(tr("Fold All"), this,
                        [this]()
                        {
                            foldAllRegions();
                        });
        menu->addAction(tr("Unfold All"), this,
                        [this]()
                        {
                            unfoldAllRegions();
                        });
    }

    menu->exec(event->globalPos());
    menu->deleteLater();
}

void
GtCodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
GtCodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
    {
        m_lineNumberArea->scroll(0, dy);
    }
    else
    {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(),
                                 rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

void
GtCodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                        lineNumberAreaWidth(),
                                        cr.height()));
}

void
GtCodeEditor::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->formats().contains("GtObject"))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void
GtCodeEditor::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->formats().contains("GtObject"))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void
GtCodeEditor::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->formats().contains("GtObject"))
    {
        clear();
        setPlainText(event->mimeData()->data("GtObject"));
    }

    QPlainTextEdit::dropEvent(event);
}

void
GtCodeEditor::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier && !isReadOnly())
    {
        if (event->angleDelta().y() > 0)
        {
            zoomIn(2);
        }
        else
        {
            zoomOut(2);
        }

        const int tabStop = 4;
        QFontMetrics metrics(font());
        setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
    }
    else
    {
        QPlainTextEdit::wheelEvent(event);
    }
}

void
GtCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> baseSelections = extraSelections();
    QList<QTextEdit::ExtraSelection> preserved;

    for (const auto& sel : baseSelections)
    {
        if (sel.format.background() !=
            gt::gui::color::code_editor::highlightLine())
        {
            preserved.append(sel);
        }
    }

    QList<QTextEdit::ExtraSelection> extraSelections = preserved;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(
                    gt::gui::color::code_editor::highlightLine());
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void
GtCodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_lineNumberArea);

    if (gt::gui::isApplicationDarkTheme())
    {
        painter.fillRect(event->rect(), Qt::darkGray);
    }
    else
    {
        painter.fillRect(event->rect(), Qt::lightGray);
    }

    const bool foldActive = (m_foldManager != nullptr
                             && m_foldManager->hasRegions());

    const int gutter = foldActive ? foldGutterWidth() : 0;

    const QColor indicatorColor =
        gt::gui::color::code_editor::foldIndicator();

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top =
        (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, m_lineNumberArea->width() - gutter,
                             fontMetrics().height(), Qt::AlignRight, number);

            if (foldActive)
            {
                const int h = (int) blockBoundingRect(block).height();

                const bool folded = m_foldManager->isFolded(blockNumber);
                const bool foldable = !folded
                                      && m_foldManager->isFoldable(blockNumber);

                if (folded || foldable)
                {
                    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                    const int x0 = m_lineNumberArea->width() - gutter + 3;
                    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                    const int w = qMax(4, gutter - 6);
                    const int s = qMin(w, qMax(3, h - 4));
                    const int cy = top + h / 2;

                    painter.setPen(indicatorColor);

                    QPolygonF tri;

                    if (folded)
                    {
                        // Right-pointing triangle (region is folded)
                        tri << QPointF(x0, cy - s / 2)
                            << QPointF(x0, cy + s / 2)
                            << QPointF(x0 + w, cy);
                    }
                    else
                    {
                        // Down-pointing triangle (region is foldable)
                        tri << QPointF(x0, cy - s / 2)
                            << QPointF(x0 + w, cy - s / 2)
                            << QPointF(x0 + w / 2, cy + s / 2);
                    }

                    painter.drawPolygon(tri);
                }
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


void
GtCodeEditor::highlightOccurrences(const QString& text)
{
    // Preserve existing line highlight (if any)
    QList<QTextEdit::ExtraSelection> baseSelections = extraSelections();
    QList<QTextEdit::ExtraSelection> preserved;

    for (const auto& sel : baseSelections)
    {
        if (sel.format.background() ==
            gt::gui::color::code_editor::highlightLine())
        {
            preserved.append(sel);
        }
    }

    // Start fresh with preserved selections only
    QList<QTextEdit::ExtraSelection> selections = preserved;

    if (text.isEmpty())
    {
        // Clear search highlights, keep line highlight
        setExtraSelections(selections);
        return;
    }

    QTextDocument* doc = document();
    QTextCharFormat fmt;
    fmt.setBackground(gt::gui::color::code_editor::highlightSearch());

    int startPos = 0;
    while (true)
    {
        QTextCursor cursor = doc->find(text, startPos,
                                       QTextDocument::FindCaseSensitively);

        if (cursor.isNull())
            break;

        QTextEdit::ExtraSelection sel;
        sel.cursor = cursor;
        sel.format = fmt;
        selections.append(sel);
        // Move start position past this match to avoid infinite
        // loop on zero‑length search text
        startPos = cursor.selectionEnd();
    }

    setExtraSelections(selections);
}
