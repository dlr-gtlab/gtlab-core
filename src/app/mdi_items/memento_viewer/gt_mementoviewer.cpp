/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "gt_mementoviewer.h"
#include "gt_icons.h"
#include "gt_mementoeditor.h"
#include "gt_xmlhighlighter.h"
#include "gt_searchwidget.h"
#include <QShortcut>
#include <QKeySequence>
#include <QTextDocument>
#include <QTextCursor>
#include "gt_application.h"

GtMementoViewer::GtMementoViewer()
{
    setObjectName("Memento Viewer");

    // Create read-only editor and store reference
    m_editor = new GtMementoEditor(widget());
    m_editor->setReadOnly(true);

    m_highlighter = new GtXmlHighlighter(m_editor->document());

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    widget()->setLayout(lay);

    // Add editor to layout
    lay->addWidget(m_editor);

    // Search widget placed at bottom-left
    QHBoxLayout* searchLay = new QHBoxLayout;
    searchLay->setContentsMargins(0, 0, 0, 0);
    searchLay->setSpacing(0);
    m_searchWidget = new GtSearchWidget(widget());
    searchLay->addWidget(m_searchWidget);
    searchLay->addStretch(1);
    m_searchWidget->enableFindNextButtons();
    lay->addLayout(searchLay);

    // Shortcut (Ctrl+F) triggers search widget
    auto* shortcut = new QShortcut(gtApp->getShortCutSequence("search"),
                                   m_editor);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(shortcut, &QShortcut::activated, m_searchWidget,
            &GtSearchWidget::enableSearch);

    // Navigation shortcuts: F3 (next), Shift+F3 (previous)
    m_nextShortcut = new QShortcut(
        gtApp->getShortCutSequence("jumpToNextElement"), m_editor);
    connect(m_nextShortcut, &QShortcut::activated, this,
            &GtMementoViewer::goToNextMatch);
    m_prevShortcut = new QShortcut(
        gtApp->getShortCutSequence("jumpToPreviousElement"), m_editor);
    connect(m_prevShortcut, &QShortcut::activated, this,
            &GtMementoViewer::goToPrevMatch);

    // Connect search changes to editor highlighting
    connect(m_searchWidget, &GtSearchWidget::textChanged, this,
            &GtMementoViewer::onSearchTextChanged);
    // Connect navigation button clicks
    connect(m_searchWidget, &GtSearchWidget::nextClicked, this,
            &GtMementoViewer::goToNextMatch);
    connect(m_searchWidget, &GtSearchWidget::prevClicked, this,
            &GtMementoViewer::goToPrevMatch);
}

GtMementoViewer::~GtMementoViewer() = default;

QIcon
GtMementoViewer::icon() const
{
    return gt::gui::icon::listFormatted();
}

bool
GtMementoViewer::allowsMultipleInstances() const
{
    return true;
}

void
GtMementoViewer::onThemeChanged()
{
    m_highlighter->onThemeChanged();
}

void
GtMementoViewer::onSearchTextChanged(const QString& text)
{
    if (!m_editor) return;
    // Highlight all occurrences and store current search text
    m_editor->highlightOccurrences(text);
    m_searchText = text;

    // Rebuild match list
    m_matches.clear();
    m_currentMatch = -1;
    if (text.isEmpty()) return;
    QTextDocument* doc = m_editor->document();
    int startPos = 0;

    while (true)
    {
        QTextCursor cursor = doc->find(text, startPos,
                                       QTextDocument::FindCaseSensitively);
        if (cursor.isNull()) break;

        m_matches.append(cursor);
        startPos = cursor.selectionEnd();

        if (text.length() == 0) ++startPos;
    }
}

void
GtMementoViewer::goToNextMatch()
{
    if (m_matches.isEmpty() || !m_editor) return;

    // Move to next match
    m_currentMatch = (m_currentMatch + 1) % m_matches.size();
    QTextCursor cur = m_matches.at(m_currentMatch);
    m_editor->setTextCursor(cur);
    m_editor->ensureCursorVisible();

    // Reapply search highlights after cursor movement (preserves line highlight)
    if (!m_searchText.isEmpty())
        m_editor->highlightOccurrences(m_searchText);
}

void
GtMementoViewer::goToPrevMatch()
{
    if (m_matches.isEmpty() || !m_editor) return;

    // Move to previous match
    m_currentMatch = (m_currentMatch - 1 + m_matches.size()) % m_matches.size();
    QTextCursor cur = m_matches.at(m_currentMatch);
    m_editor->setTextCursor(cur);
    m_editor->ensureCursorVisible();

    // Reapply search highlights after cursor movement (preserves line highlight)
    if (!m_searchText.isEmpty())
        m_editor->highlightOccurrences(m_searchText);
}

