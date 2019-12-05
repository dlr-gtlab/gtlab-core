/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineui.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QStringList>

#include "gt_splineeditor.h"
#include "gt_application.h"

#include "gt_splineui.h"

GtSplineUI::GtSplineUI()
{

}

QStringList
GtSplineUI::openWith(GtObject* /*obj*/)
{
    return QStringList() << GT_CLASSNAME(GtSplineEditor);
}

QIcon
GtSplineUI::icon(GtObject*) const
{
    return GtApplication::icon("bezierCurve.png");
}
