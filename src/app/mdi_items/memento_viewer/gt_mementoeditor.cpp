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

GtMementoEditor::GtMementoEditor(QWidget* parent) : GtCodeEditor(parent)
{

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
