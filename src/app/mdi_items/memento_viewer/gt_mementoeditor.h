/* GTlab - Gas Turbine laboratory
 * Source File: gt_mementoeditor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMEMENTOEDITOR_H
#define GTMEMENTOEDITOR_H

#include "gt_codeeditor.h"

/**
 * @brief The GtMementoEditor class
 */
class GtMementoEditor : public GtCodeEditor
{
    Q_OBJECT

public:
    /**
     * @brief GtMementoEditor
     * @param parent
     */
    explicit GtMementoEditor(QWidget* parent = nullptr);

protected:
    /**
     * @brief dropEvent
     * @param event
     */
    virtual void dropEvent(QDropEvent* event);

};

#endif // GTMEMENTOEDITOR_H
