/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCODEEDITOR_H
#define GTCODEEDITOR_H

#include "gt_gui_exports.h"

#include <QPlainTextEdit>

class GT_GUI_EXPORT GtCodeEditor: public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit GtCodeEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event);

    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;

    void dragMoveEvent(QDragMoveEvent* event) override;

    void dropEvent(QDropEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);

    void highlightCurrentLine();

    void updateLineNumberArea(const QRect&, int);

private:
    QWidget* m_lineNumberArea;

};

#endif // GTCODEEDITOR_H
