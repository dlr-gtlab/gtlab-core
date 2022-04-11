/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMimeData>
#include <QPainter>
#include <QTextBlock>

#include "gt_codeeditor.h"
#include "gt_linenumberarea.h"
#include "gt_application.h"

GtCodeEditor::GtCodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    m_lineNumberArea = new GtLineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)),
            SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)),
            SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()),
            SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setAcceptDrops(true);

    setLineWrapMode(QPlainTextEdit::NoWrap);
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

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
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
    //    qDebug() << "CodeEditor::dragEnterEvent";
    if (event->mimeData()->formats().contains("GtObject"))
    {
        //            clear();
        //            setPlainText(event->mimeData()->data("GtdObject"));
        //        qDebug() << "   |-> accepted!";
        event->accept();
    }
    else
    {
        //        qDebug() << "   |-> ignored!";
        event->ignore();
    }


    //        QPlainTextEdit::dragEnterEvent(event);
}

void
GtCodeEditor::dragMoveEvent(QDragMoveEvent* event)
{
    //    qDebug() << "CodeEditor::dragMoveEvent";
    //    event->accept();

    if (event->mimeData()->formats().contains("GtObject"))
    {
        //            clear();
        //            setPlainText(event->mimeData()->data("GtdObject"));
        //        qDebug() << "   |-> accepted!";
        event->accept();
    }
    else
    {
        //        qDebug() << "   |-> ignored!";
        event->ignore();
    }


    //    QPlainTextEdit::dragMoveEvent(event);
}

void
GtCodeEditor::dropEvent(QDropEvent* event)
{
    //    qDebug() << "CodeEditor::dropEvent";

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
        if (event->delta() > 0)
        {
            zoomIn(2);
        }
        else
        {
            zoomOut(2);
        }

        const int tabStop = 4;
        QFontMetrics metrics(font());
        setTabStopWidth(tabStop * metrics.width(' '));
    }
    else
    {
        QPlainTextEdit::wheelEvent(event);
    }
}

void
GtCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
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

    if (gtApp->inDarkMode())
    {
        painter.fillRect(event->rect(), Qt::darkGray);
    }
    else
    {
        painter.fillRect(event->rect(), Qt::lightGray);
    }

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
            painter.drawText(0, top, m_lineNumberArea->width(),
                             fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
