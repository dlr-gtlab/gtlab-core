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

class GtXmlHighlighter;

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

private:
    /// Xml highlighter
    GtXmlHighlighter* m_highlighter;

};

#endif // GTMEMENTOVIEWER_H
