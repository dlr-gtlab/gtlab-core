/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_examplestabwidget.h"

GtExamplesTabWidget::GtExamplesTabWidget(QWidget* parent) :
    QTabWidget(parent)
{
    setMinimumHeight(250);
    setMinimumWidth(250);

    setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::MinimumExpanding);

}

void
GtExamplesTabWidget::resizeEvent(QResizeEvent* e)
{
    emit resized();

    QTabWidget::resizeEvent(e);
}
