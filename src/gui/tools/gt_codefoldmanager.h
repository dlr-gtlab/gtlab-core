/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 20.11.2026
 *  Author: Schmalfuess (AT-TWK)
 */

#ifndef GTCODEFOLDMANAGER_H
#define GTCODEFOLDMANAGER_H

#include <QHash>
#include <QList>
#include <QVector>

#include "gt_gui_exports.h"

class QTextDocument;

/**
 * @brief GtFoldManager
 *
 * Manages true document folding for a QTextDocument based editor.
 *
 * A folded region physically removes its body lines from the document;
 * the hidden text is stored in the manager and restored when the region
 * is unfolded again. All public methods operate on document lines (lines
 * of the current, possibly folded, document) unless stated otherwise.
 *
 * The manager is language neutral: the foldable regions are computed by
 * a region calculator (e.g. for Python) and handed over via
 * updateLogicalRegions().
 *
 * Known limitation: folding a region that contains already folded nested
 * regions absorbs the nested spans; the nested fold state is lost when
 * the outer region is unfolded.
 */
class GT_GUI_EXPORT GtFoldManager: public QObject
{
    Q_OBJECT

public:
    /// One foldable region in the logical (unfolded) line numbering
    struct Region
    {
        int startLine;   ///< first logical line (0-based), the header
        int endLine;     ///< last logical line (0-based), last body line
    };

    /// A folded region in the current document
    struct Span
    {
        int logicalStart;   ///< logical line of the header
        int logicalEnd;     ///< logical line of the last body line
        int startOffset;    ///< character offset of the header line
        int endOffset;      ///< character offset right after the header text
        QString headerText; ///< text of the header line
        QString hiddenText; ///< stored body text (restored on unfold)
    };

    explicit GtFoldManager(QTextDocument* document, QObject* parent = nullptr);

    QTextDocument* document() const;

    void setDocument(QTextDocument* document);

    /// Replace the foldable regions (logical line numbering). Existing
    /// folds that still match a region are preserved.
    void updateLogicalRegions(const QVector<Region>& regions);

    bool hasRegions() const;

    /// @return true if the given document line is a folded region header
    bool isFolded(int docLine) const;

    /// @return true if the given document line is a foldable region header
    bool isFoldable(int docLine) const;

    /// @return true if any region is currently folded
    bool hasFolds() const;

    /// @return true if the given logical line is hidden inside a folded region
    bool isLogicalLineHidden(int logicalLine) const;

    int logicalLineToDocumentLine(int logicalLine) const;

    int documentLineToLogicalLine(int docLine) const;

    int logicalLineCount() const;

    /// @return the full logical text (visible + hidden lines), same format
    ///         as QTextDocument::toPlainText()
    QString fullText() const;

    // Fold operations on document lines

    bool toggleFold(int docLine);

    bool fold(int docLine);

    bool unfold(int docLine);

    void foldAll();

    void unfoldAll();

    /// Unfold the folded region that contains the given logical line
    void unfoldRegionContainingLogicalLine(int logicalLine);

    /// @return pointer to the span whose header is the given document line
    const Span* spanForDocBlock(int docBlockNumber) const;

    int logicalToDoc(int logicalLine) const;

    int docToLogical(int docBlockNumber) const;

signals:
    /// Emitted whenever the fold state or the document changed in a way
    /// that requires the editor to update its display
    void foldStateChanged();

private:
    void handleContentsChange(int pos, int removed, int inserted);

    /// Handle an external document change that may be an undo/redo of a
    /// fold/unfold operation and keep the spans in sync with the document's
    /// undo stack
    void handleFoldUndo(int pos, int removed, int inserted);

    void rebuildMapping();

    /// Remove the span at @a index and insert its hidden text back into
    /// the document at @a insertPos (never loses text)
    void restoreSpan(int index, int insertPos);

    /// Remember @a span as the last span that was removed from the manager,
    /// so that an external deletion of its hidden text (an undo/redo of the
    /// original fold command) re-creates the span
    void recordLastUnfold(const Span& span);

    /// Keep @a m_lastDocText in sync with a document change
    void syncDocTextCache(int pos, int removed, int inserted);

    /// Handle the no-argument contentsChanged signal: derive the edit
    /// region from a prefix/suffix diff of the cached text and dispatch it
    /// to handleContentsChange()
    void handleDocumentTextChanged();

    QTextDocument* m_document;

    QVector<Region> m_logicalRegions;

    QVector<Span> m_spans;

    QHash<int, int> m_blockToSpanIndex;

    QVector<int> m_docToLogical;

    QVector<int> m_logicalToDoc;

    bool m_applying;

    /// Cache of the document text, kept in sync through all changes
    QString m_lastDocText;

    /// The span that was removed from the manager most recently
    Span m_lastUnfold;

    /// @c true while @a m_lastUnfold describes a document state that still
    /// matches the document
    bool m_lastUnfoldValid;
};

#endif // GTCODEFOLDMANAGER_H
