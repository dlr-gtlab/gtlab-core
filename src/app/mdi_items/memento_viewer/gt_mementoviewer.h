/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMEMENTOVIEWER_H
#define GTMEMENTOVIEWER_H

#include "gt_mdiitem.h"

#include <QList>

class GtXmlHighlighter;
class GtMementoEditor;
class GtSearchWidget;
class QShortcut;
class QTextCursor;

/**
 * @brief The GtMementoViewer class
 */
class GtMementoViewer : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtMementoViewer
     */
    Q_INVOKABLE GtMementoViewer();

    /**
     * @brief ~GtMementoViewer
     */
    ~GtMementoViewer() override;

    /** Virtual function to specify item specific icon.
        @return Object specific icon */
    QIcon icon() const override;

    /**
     * @brief allowsMultipleInstances
     * @return true because the memento viewer can be opened multiple times
     */
    bool allowsMultipleInstances() const override;

public slots:
    void onThemeChanged() override;

private slots:
    void onSearchTextChanged(const QString& text);

private:
    /// Xml highlighter
    GtXmlHighlighter* m_highlighter;
    /// Editor widget
    GtMementoEditor* m_editor;
    /// Search widget
    GtSearchWidget* m_searchWidget;
    /// Cached match positions
    QList<QTextCursor> m_matches;
    int m_currentMatch = -1;

    // Navigation shortcuts
    QShortcut* m_nextShortcut = nullptr;
    QShortcut* m_prevShortcut = nullptr;

private slots:
    void goToNextMatch();
    void goToPrevMatch();

};

#endif // GTMEMENTOVIEWER_H
