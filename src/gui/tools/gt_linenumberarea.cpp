/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_linenumberarea.h"
#include "gt_codeeditor.h"

GtLineNumberArea::GtLineNumberArea(GtCodeEditor* editor) : QWidget(editor),
    m_codeEditor(editor)
{
}

QSize
GtLineNumberArea::sizeHint() const
{
    return {m_codeEditor->lineNumberAreaWidth(), 0};
}

void
GtLineNumberArea::paintEvent(QPaintEvent* event)
{
    m_codeEditor->lineNumberAreaPaintEvent(event);
}

