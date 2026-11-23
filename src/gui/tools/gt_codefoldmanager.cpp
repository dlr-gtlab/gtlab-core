/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 20.11.2026
 *  Author: Schmalfuess (AT-TWK)
 */

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <algorithm>
#include <utility>

#include "gt_codefoldmanager.h"

GtFoldManager::GtFoldManager(QTextDocument* document, QObject* parent) :
    QObject(parent),
    m_document(document),
    m_applying(false),
    m_lastUnfoldValid(false)
{
    if (m_document != nullptr)
    {
        m_lastDocText = m_document->toPlainText();

        connect(m_document,
                &QTextDocument::contentsChanged,
                this,
                &GtFoldManager::handleDocumentTextChanged);
    }
}

QTextDocument*
GtFoldManager::document() const
{
    return m_document;
}

void
GtFoldManager::setDocument(QTextDocument* document)
{
    if (m_document == document)
    {
        return;
    }

    if (m_document != nullptr)
    {
        disconnect(m_document,
                   &QTextDocument::contentsChanged,
                   this,
                   &GtFoldManager::handleDocumentTextChanged);
    }

    m_document = document;

    m_lastDocText = (m_document != nullptr) ? m_document->toPlainText()
                                            : QString();
    m_lastUnfoldValid = false;

    m_logicalRegions.clear();
    m_spans.clear();
    m_blockToSpanIndex.clear();
    m_docToLogical.clear();
    m_logicalToDoc.clear();

    if (m_document != nullptr)
    {
        connect(m_document,
                &QTextDocument::contentsChanged,
                this,
                &GtFoldManager::handleDocumentTextChanged);
    }
}

void
GtFoldManager::updateLogicalRegions(const QVector<Region>& regions)
{
    const QString full = fullText();

    const QStringList lines = full.split('\n');

    m_logicalRegions = regions;

    // Unfold the spans that no longer match a region, bottom up, so the
    // hidden text is never lost. Spans that still match a region stay
    // folded; their offsets were already rebased by the document change
    // that triggered the recalculation.
    QList<int> docLines;

    for (auto it = m_blockToSpanIndex.constBegin();
         it != m_blockToSpanIndex.constEnd(); ++it)
    {
        const Span& span = m_spans.at(it.value());

        bool preserved = false;

        for (const Region& region : regions)
        {
            if (region.startLine == span.logicalStart
                    && region.endLine == span.logicalEnd
                    && region.startLine < lines.size()
                    && lines.at(region.startLine) == span.headerText)
            {
                preserved = true;
                break;
            }
        }

        if (!preserved)
        {
            docLines.append(it.key());
        }
    }

    std::sort(docLines.begin(), docLines.end(), std::greater<int>());

    m_applying = true;

    for (const int docLine : docLines)
    {
        unfold(docLine);
    }

    m_applying = false;

    rebuildMapping();

    emit foldStateChanged();
}

bool
GtFoldManager::hasRegions() const
{
    return !m_logicalRegions.isEmpty();
}

bool
GtFoldManager::isFolded(int docLine) const
{
    return spanForDocBlock(docLine) != nullptr;
}

bool
GtFoldManager::isFoldable(int docLine) const
{
    const int logicalLine = docToLogical(docLine);

    for (const Region& region : m_logicalRegions)
    {
        if (region.startLine == logicalLine && region.endLine > region.startLine)
        {
            return true;
        }
    }

    return false;
}

bool
GtFoldManager::hasFolds() const
{
    return !m_spans.isEmpty();
}

bool
GtFoldManager::isLogicalLineHidden(int logicalLine) const
{
    for (const Span& span : m_spans)
    {
        if (logicalLine > span.logicalStart
                && logicalLine <= span.logicalEnd)
        {
            return true;
        }
    }

    return false;
}

int
GtFoldManager::logicalLineToDocumentLine(int logicalLine) const
{
    return logicalToDoc(logicalLine);
}

int
GtFoldManager::documentLineToLogicalLine(int docLine) const
{
    return docToLogical(docLine);
}

int
GtFoldManager::logicalLineCount() const
{
    return m_logicalToDoc.size();
}

int
GtFoldManager::logicalToDoc(int logicalLine) const
{
    if (m_document == nullptr)
    {
        return 0;
    }

    const int blockCount = m_document->blockCount();

    if (logicalLine < 0)
    {
        return 0;
    }

    if (logicalLine >= m_logicalToDoc.size())
    {
        return qMax(0, blockCount - 1);
    }

    return m_logicalToDoc.at(logicalLine);
}

int
GtFoldManager::docToLogical(int docBlockNumber) const
{
    if (m_document == nullptr)
    {
        return 0;
    }

    const int blockCount = m_document->blockCount();

    if (docBlockNumber < 0)
    {
        return 0;
    }

    if (docBlockNumber >= m_docToLogical.size())
    {
        return qMax(0, logicalLineCount() - 1);
    }

    return m_docToLogical.at(docBlockNumber);
}

QString
GtFoldManager::fullText() const
{
    if (m_document == nullptr)
    {
        return QString();
    }

    if (m_spans.isEmpty())
    {
        return m_document->toPlainText();
    }

    QString full;

    const int blockCount = m_document->blockCount();

    QTextBlock block = m_document->firstBlock();

    while (block.isValid())
    {
        full += block.text();

        if (block.blockNumber() < blockCount - 1)
        {
            full += '\n';
        }

        const Span* span = spanForDocBlock(block.blockNumber());

        if (span != nullptr)
        {
            full += span->hiddenText;
        }

        block = block.next();
    }

    return full;
}

bool
GtFoldManager::toggleFold(int docLine)
{
    if (isFolded(docLine))
    {
        return unfold(docLine);
    }

    return fold(docLine);
}

bool
GtFoldManager::fold(int docLine)
{
    if (m_document == nullptr)
    {
        return false;
    }

    const int logicalLine = docToLogical(docLine);

    if (isFolded(docLine))
    {
        return false;
    }

    const Region* region = nullptr;

    for (const Region& r : m_logicalRegions)
    {
        if (r.startLine == logicalLine && r.endLine > r.startLine)
        {
            region = &r;
            break;
        }
    }

    if (region == nullptr)
    {
        return false;
    }

    const int blockCount = m_document->blockCount();

    QTextBlock headerBlock = m_document->findBlockByLineNumber(docLine);

    if (!headerBlock.isValid())
    {
        return false;
    }

    const int headerStart = headerBlock.position();

    const int headerEnd = headerStart + headerBlock.length();

    // Walk the visible blocks to collect the body (up to the last logical
    // line of the region, inclusive) and to absorb nested folded spans
    QString hidden;

    int bodyEnd = headerEnd + 1;

    QTextBlock block = headerBlock.next();

    while (block.isValid())
    {
        const int l = docToLogical(block.blockNumber());

        if (l > region->endLine)
        {
            break;
        }

        bodyEnd = block.position() + block.length()
                    + (block.blockNumber() < blockCount - 1 ? 1 : 0);

        hidden += block.text();

        if (block.blockNumber() < blockCount - 1)
        {
            hidden += '\n';
        }

        const Span* nested = spanForDocBlock(block.blockNumber());

        if (nested != nullptr)
        {
            hidden += nested->hiddenText;
        }

        if (l == region->endLine)
        {
            break;
        }

        block = block.next();
    }

    if (hidden.isEmpty())
    {
        return false;
    }

    Span span;

    span.logicalStart = region->startLine;
    span.logicalEnd = region->endLine;
    span.startOffset = headerStart;
    span.endOffset = headerEnd;
    span.headerText = headerBlock.text();
    span.hiddenText = hidden;

    // Remove the visible body as a single (undoable) change
    const bool wasApplying = m_applying;

    m_applying = true;

    QTextCursor cursor(m_document);

    cursor.setPosition(headerEnd + 1);
    cursor.setPosition(bodyEnd, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    m_lastDocText.remove(headerEnd + 1, bodyEnd - (headerEnd + 1));

    m_spans.append(span);
    std::sort(m_spans.begin(), m_spans.end(),
              [](const Span& a, const Span& b)
              {
                  return a.startOffset < b.startOffset;
              });

    // Drop the absorbed nested spans
    m_spans.erase(std::remove_if(m_spans.begin(), m_spans.end(),
                                 [this, &span](const Span& s)
                                 {
                                     return s.logicalStart > span.logicalStart
                                            && s.logicalStart <= span.logicalEnd;
                                 }),
                  m_spans.end());

    m_applying = wasApplying;

    // A new fold invalidates any pending unfold record
    m_lastUnfoldValid = false;

    rebuildMapping();

    emit foldStateChanged();

    return true;
}

bool
GtFoldManager::unfold(int docLine)
{
    const int index = m_blockToSpanIndex.value(docLine, -1);

    if (index < 0 || index >= m_spans.size())
    {
        return false;
    }

    const Span span = m_spans.at(index);

    const int pos = qMin(span.endOffset + 1, m_document->characterCount());

    const bool wasApplying = m_applying;

    m_applying = true;

    QTextCursor cursor(m_document);

    cursor.setPosition(pos);
    cursor.insertText(span.hiddenText);

    m_applying = wasApplying;

    m_lastDocText.insert(pos, span.hiddenText);

    m_spans.removeAt(index);
    m_blockToSpanIndex.clear();

    recordLastUnfold(span);

    rebuildMapping();

    emit foldStateChanged();

    return true;
}

void
GtFoldManager::foldAll()
{
    if (m_document == nullptr)
    {
        return;
    }

    QList<int> logicalStarts;

    for (const Region& region : m_logicalRegions)
    {
        if (region.endLine > region.startLine)
        {
            logicalStarts.append(region.startLine);
        }
    }

    std::sort(logicalStarts.begin(), logicalStarts.end(), std::greater<int>());

    m_applying = true;

    for (const int logicalStart : logicalStarts)
    {
        fold(logicalToDoc(logicalStart));
    }

    m_applying = false;

    rebuildMapping();

    emit foldStateChanged();
}

void
GtFoldManager::unfoldAll()
{
    if (m_document == nullptr)
    {
        return;
    }

    QList<int> docLines;

    for (auto it = m_blockToSpanIndex.constBegin();
         it != m_blockToSpanIndex.constEnd(); ++it)
    {
        docLines.append(it.key());
    }

    std::sort(docLines.begin(), docLines.end(), std::greater<int>());

    m_applying = true;

    for (const int docLine : docLines)
    {
        const int index = m_blockToSpanIndex.value(docLine, -1);

        if (index >= 0)
        {
            unfold(docLine);
        }
    }

    m_applying = false;

    rebuildMapping();

    emit foldStateChanged();
}

void
GtFoldManager::unfoldRegionContainingLogicalLine(int logicalLine)
{
    for (const Span& span : m_spans)
    {
        if (logicalLine >= span.logicalStart && logicalLine <= span.logicalEnd)
        {
            unfold(logicalToDoc(span.logicalStart));
            return;
        }
    }
}

const GtFoldManager::Span*
GtFoldManager::spanForDocBlock(int docBlockNumber) const
{
    const int index = m_blockToSpanIndex.value(docBlockNumber, -1);

    if (index < 0 || index >= m_spans.size())
    {
        return nullptr;
    }

    return &m_spans.at(index);
}

void
GtFoldManager::rebuildMapping()
{
    m_blockToSpanIndex.clear();
    m_docToLogical.clear();
    m_logicalToDoc.clear();

    if (m_document == nullptr)
    {
        return;
    }

    const int blockCount = m_document->blockCount();

    // Map document blocks to spans
    for (int i = 0; i < m_spans.size(); ++i)
    {
        const QTextBlock block = m_document->findBlock(m_spans.at(i).startOffset);

        if (block.isValid() && block.position() == m_spans.at(i).startOffset)
        {
            m_blockToSpanIndex.insert(block.blockNumber(), i);
        }
    }

    // Document line -> logical line
    m_docToLogical.resize(blockCount);

    int logical = 0;

    for (int i = 0; i < blockCount; ++i)
    {
        m_docToLogical[i] = logical;

        const int spanIndex = m_blockToSpanIndex.value(i, -1);

        if (spanIndex >= 0 && spanIndex < m_spans.size())
        {
            logical += m_spans.at(spanIndex).logicalEnd
                       - m_spans.at(spanIndex).logicalStart;
        }

        ++logical;
    }

    // Logical line -> document line
    m_logicalToDoc.resize(logical);

    logical = 0;

    for (int i = 0; i < blockCount; ++i)
    {
        if (logical < m_logicalToDoc.size())
        {
            m_logicalToDoc[logical] = i;
        }

        const int spanIndex = m_blockToSpanIndex.value(i, -1);

        if (spanIndex >= 0 && spanIndex < m_spans.size())
        {
            const Span& span = m_spans.at(spanIndex);

            for (int l = logical + 1; l <= span.logicalEnd; ++l)
            {
                if (l < m_logicalToDoc.size())
                {
                    m_logicalToDoc[l] = i;
                }
            }

            logical += span.logicalEnd - span.logicalStart;
        }

        ++logical;
    }
}

void
GtFoldManager::restoreSpan(int index, int insertPos)
{
    const Span span = m_spans.at(index);

    m_spans.removeAt(index);
    m_blockToSpanIndex.clear();

    const int pos = qMin(insertPos, m_document->characterCount());

    const bool wasApplying = m_applying;

    m_applying = true;

    QTextCursor cursor(m_document);

    cursor.setPosition(pos);
    cursor.insertText(span.hiddenText);

    m_applying = wasApplying;

    m_lastDocText.insert(pos, span.hiddenText);

    // The document state now equals the state after an unfold of this
    // span, so an external undo of this insertion must re-create the
    // span
    recordLastUnfold(span);
}

void
GtFoldManager::handleDocumentTextChanged()
{
    if (m_applying || m_document == nullptr)
    {
        // Our own fold/unfold operations manage the cache themselves
        return;
    }

    const QString newText = m_document->toPlainText();

    // Minimal edit region via common prefix and common suffix
    const QString& oldText = m_lastDocText;

    const int oldSize = oldText.size();

    const int newSize = newText.size();

    const int maxPrefix = qMin(oldSize, newSize);

    int prefix = 0;

    while (prefix < maxPrefix && oldText.at(prefix) == newText.at(prefix))
    {
        ++prefix;
    }

    const int maxSuffix = qMin(oldSize - prefix, newSize - prefix);

    int suffix = 0;

    while (suffix < maxSuffix
            && oldText.at(oldSize - 1 - suffix) == newText.at(newSize - 1 - suffix))
    {
        ++suffix;
    }

    const int pos = prefix;

    const int removed = oldSize - prefix - suffix;

    const int inserted = newSize - prefix - suffix;

    handleContentsChange(pos, removed, inserted);
}

void
GtFoldManager::handleContentsChange(int pos, int removed, int inserted)
{
    if (m_applying || m_document == nullptr)
    {
        return;
    }

    const int oldCharCount = m_document->characterCount() + removed - inserted;

    if (pos == 0 && removed >= oldCharCount)
    {
        // Full document replacement: all fold state is invalid
        m_spans.clear();
        m_blockToSpanIndex.clear();
        m_lastUnfoldValid = false;
        syncDocTextCache(pos, removed, inserted);
        rebuildMapping();
        emit foldStateChanged();
        return;
    }

    handleFoldUndo(pos, removed, inserted);

    const int rangeStart = pos;
    const int rangeEnd = pos + removed;

    // Rebase a character offset that lies outside the removed range
    auto shiftOffset = [](int& offset, int changePos, int removedChars,
                          int insertedChars)
    {
        if (offset >= changePos + removedChars)
        {
            offset += insertedChars - removedChars;
        }
        else if (changePos < offset && offset < changePos + removedChars)
        {
            offset = changePos + insertedChars;
        }
    };

    // Process the spans from the highest to the lowest offset
    for (int i = m_spans.size() - 1; i >= 0; --i)
    {
        const int lineStart = m_spans.at(i).startOffset;
        const int lineEnd = m_spans.at(i).endOffset;
        const bool headerLastOld = lineEnd >= oldCharCount - 1;
        const int lineTail = lineEnd + (headerLastOld ? 0 : 1);
        const int guardStart = (lineStart > 0) ? lineStart - 1 : lineStart;

        const bool overlap = (removed > 0)
                               ? (rangeStart < lineTail && rangeEnd > guardStart)
                               : (pos >= lineStart && pos < lineTail);

        if (!overlap)
        {
            shiftOffset(m_spans[i].startOffset, rangeStart, removed, inserted);
            shiftOffset(m_spans[i].endOffset, rangeStart, removed, inserted);
            continue;
        }

        if (removed > 0)
        {
            const bool removesNewlineBefore = (lineStart > 0)
                                              && (rangeStart <= lineStart - 1)
                                              && (lineStart - 1 < rangeEnd);

            const bool removesNewlineAfter = !headerLastOld
                                             && (rangeStart <= lineEnd)
                                             && (lineEnd < rangeEnd);

            const bool coversHeader = (rangeStart <= lineStart)
                                      && (rangeEnd >= lineEnd);

            if (removesNewlineBefore || removesNewlineAfter || coversHeader)
            {
                restoreSpan(i, lineEnd);
                continue;
            }
        }

        // The header line was modified but is still a line: adjust the
        // offsets and re-read the header text
        shiftOffset(m_spans[i].startOffset, rangeStart, removed, inserted);
        shiftOffset(m_spans[i].endOffset, rangeStart, removed, inserted);

        const QTextBlock block =
            m_document->findBlock(m_spans.at(i).startOffset);

        if (!block.isValid() || block.position() != m_spans.at(i).startOffset)
        {
            restoreSpan(i, m_spans.at(i).endOffset);
            continue;
        }

        m_spans[i].headerText = block.text();
    }

    rebuildMapping();

    emit foldStateChanged();
}

void
GtFoldManager::recordLastUnfold(const Span& span)
{
    m_lastUnfold = span;

    m_lastUnfoldValid = true;
}

void
GtFoldManager::syncDocTextCache(int pos, int removed, int inserted)
{
    m_lastDocText.remove(pos, removed);

    if (inserted > 0)
    {
        const QString newText = m_document->toPlainText().mid(pos, inserted);

        m_lastDocText.insert(pos, newText);
    }
}

void
GtFoldManager::handleFoldUndo(int pos, int removed, int inserted)
{
    // An insertion of exactly a span's hidden text right after the header
    // line is the external undo of the corresponding fold
    if (removed == 0 && inserted > 0)
    {
        const QString insertedText = m_document->toPlainText().mid(pos, inserted);

        for (int i = 0; i < m_spans.size(); ++i)
        {
            if (pos == m_spans.at(i).endOffset + 1
                    && insertedText == m_spans.at(i).hiddenText)
            {
                const Span span = m_spans.at(i);

                m_spans.removeAt(i);
                m_blockToSpanIndex.clear();

                recordLastUnfold(span);
                syncDocTextCache(pos, removed, inserted);
                return;
            }
        }
    }

    // A deletion of the last unfolded span's hidden text right after the
    // header line is the external redo of the original fold
    if (removed > 0 && inserted == 0 && m_lastUnfoldValid
            && pos == m_lastUnfold.endOffset + 1
            && m_lastDocText.mid(pos, removed) == m_lastUnfold.hiddenText)
    {
        const Span span = m_lastUnfold;

        m_spans.append(span);
        std::sort(m_spans.begin(), m_spans.end(),
                  [](const Span& a, const Span& b)
                  {
                      return a.startOffset < b.startOffset;
                  });

        m_lastUnfoldValid = false;
        syncDocTextCache(pos, removed, inserted);
        return;
    }

    // Any other change invalidates the record and is applied to the cache
    m_lastUnfoldValid = false;
    syncDocTextCache(pos, removed, inserted);
}
