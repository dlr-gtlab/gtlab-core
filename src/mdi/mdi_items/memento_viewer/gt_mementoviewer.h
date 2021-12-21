/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMEMENTOVIEWER_H
#define GTMEMENTOVIEWER_H

#include "gt_mdi_exports.h"

#include "gt_mdiitem.h"

class GtXmlHighlighter;

/**
 * @brief The GtMementoViewer class
 */
class GT_MDI_EXPORT GtMementoViewer : public GtMdiItem
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
    virtual ~GtMementoViewer();

    /** Virtual function to specify item specific icon.
        @return Object specific icon */
    virtual QIcon icon() const override;

    /**
     * @brief allowsMultipleInstances
     * @return true because the memento viewer can be opened multiple times
     */
    bool allowsMultipleInstances() override;

public slots:
    void onThemeChanged() override;

private:
    /// Xml highlighter
    GtXmlHighlighter* m_highlighter;

};

#endif // GTMEMENTOVIEWER_H
