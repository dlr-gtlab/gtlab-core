/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_mementoeditor.cpp
 *
 *  Created on: 26.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMimeData>
#include <QMenu>

#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"

#include "gt_mementoeditor.h"
#include "gt_colors.h"

GtMementoEditor::GtMementoEditor(QWidget* parent) : GtCodeEditor(parent)
{
    // No special init required
}

void
GtMementoEditor::highlightOccurrences(const QString& text)
{
    // Preserve existing line highlight (if any)
    QList<QTextEdit::ExtraSelection> baseSelections = this->extraSelections();
    QList<QTextEdit::ExtraSelection> preserved;
    for (const auto& sel : baseSelections)
    {
        if (sel.format.background() == gt::gui::color::code_editor::highlightLine())
            preserved.append(sel);
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
    fmt.setBackground(gt::gui::color::code_editor::highlightLine());

    int startPos = 0;
    while (true)
    {
        QTextCursor cursor = doc->find(text, startPos, QTextDocument::FindCaseSensitively);
        if (cursor.isNull())
            break;
        QTextEdit::ExtraSelection sel;
        sel.cursor = cursor;
        sel.format = fmt;
        selections.append(sel);
        // Move start position past this match to avoid infinite loop on zero‑length search text
        startPos = cursor.selectionEnd();
        if (text.length() == 0)
            ++startPos;
    }

    setExtraSelections(selections);
}


void
GtMementoEditor::dropEvent(QDropEvent* event)
{
    const QString content = document()->toPlainText();

    if (!event->mimeData()->formats().contains("GtObject"))
    {
        QPlainTextEdit::dropEvent(event);
        return;
    }

    const QString droppedContent = event->mimeData()->data("GtObject");

    if (!content.isEmpty())
    {
        GtObjectMemento contentM(content.toUtf8());
        GtObjectMemento droppedM(droppedContent.toUtf8());

        if (!contentM.isNull() && !droppedM.isNull())
        {
            if (contentM.className() == droppedM.className() &&
                    contentM.uuid() == droppedM.uuid())
            {
                QMenu menu;
                menu.addAction("Show");
                QAction* diffAction = menu.addAction("Diff");

                QAction* a = menu.exec(QCursor::pos());

                if (a == diffAction)
                {
                    clear();
                    GtObjectMementoDiff diff(contentM, droppedM);
                    setPlainText(diff.toByteArray());
                    return;
                }
            }
        }
    }

    GtCodeEditor::dropEvent(event);
}
