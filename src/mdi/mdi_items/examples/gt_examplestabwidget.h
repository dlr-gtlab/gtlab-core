/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTEXAMPLESTABWIDGET_H
#define GTEXAMPLESTABWIDGET_H

#include "gt_mdi_exports.h"

#include <QTabWidget>

/**
 * @brief The GtExamplesTabWidget class
 * Main function of this TabWidget is the overload of the resizeEvent to
 * throw a signal
 */
class GT_MDI_EXPORT GtExamplesTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief GtExamplesTabWidget - Constructor
     * @param parent
     */
    Q_INVOKABLE GtExamplesTabWidget(QWidget* parent = Q_NULLPTR);

    /**
     * @brief resizeEvent
     *  Overloaded Resize event of the QTabWidget
     * @param e
     */
    void resizeEvent(QResizeEvent* e);

signals:
    /**
     * @brief resized
     *  Signal for resized
     */
    void resized();
};

#endif // GTEXAMPLESTABWIDGET_H