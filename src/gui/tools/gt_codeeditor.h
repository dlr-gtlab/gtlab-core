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
#include <QTimer>
#include <functional>

class GtFoldManager;

class GT_GUI_EXPORT GtCodeEditor: public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit GtCodeEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event);

    int lineNumberAreaWidth();

    /**
     * @brief Highlight all occurrences of text (case-sensitive)
     * @param text Search text
     */
    void highlightOccurrences(const QString& text);

    /**
     * @brief Returns the fold manager of the editor
     */
    GtFoldManager* foldManager() const;

    /**
     * @brief Returns the full text of the editor, including the lines
     *        that are currently hidden in folded regions
     * @return the full text
     */
    QString fullText() const;

    // Fold operations on document lines

    /**
     * @brief Toggles the fold state of the region at the given document line
     * @param docLine document line of the region header
     * @return true if the fold state was changed
     */
    bool toggleFold(int docLine);

    /**
     * @brief Folds the region at the given document line
     * @param docLine document line of the region header
     * @return true if the region was folded
     */
    bool fold(int docLine);

    /**
     * @brief Unfolds the region at the given document line
     * @param docLine document line of the region header
     * @return true if the region was unfolded
     */
    bool unfold(int docLine);

    /**
     * @brief Folds all foldable regions
     */
    void foldAllRegions();

    /**
     * @brief Unfolds all folded regions
     */
    void unfoldAllRegions();

    /**
     * @brief Toggles the fold state of the region at the current line
     */
    void toggleFoldAtCurrentLine();

    /**
     * @brief Folds the region at the current line
     */
    void foldAtCurrentLine();

    /**
     * @brief Unfolds the region at the current line
     */
    void unfoldAtCurrentLine();

    /**
     * @brief Handles a mouse click in the line number area (fold glyph or
     *        cursor positioning)
     * @param pos click position in line number area coordinates
     */
    void handleLineNumberAreaClick(const QPoint& pos);

    /**
     * @brief Width of the fold indicator gutter in the line number area
     */
    static int foldGutterWidth();

protected:
    virtual void paintEvent(QPaintEvent* event) override;

    virtual void contextMenuEvent(QContextMenuEvent* event) override;

    virtual void recalculateFoldRegions();

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

    GtFoldManager* m_foldManager;

    QTimer* m_foldRecalcTimer;

    /// @brief Executes a fold operation while keeping the cursor and the
    ///        scroll position stable
    bool performFoldOp(const std::function<bool()>& operation);

    /// @brief Restores the cursor and the scroll position after a fold
    ///        operation, keeping the affected line at its position
    void restoreCursorAfterFoldOp(int logicalLine, int column, int topBefore);
};

#endif // GTCODEEDITOR_H
