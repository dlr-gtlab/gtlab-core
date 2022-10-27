/* GTlab - Gas Turbine laboratory
 * Source File: gtd_datamodelconverter.cpp
 * copyright 2009-2022 by DLR
 *
 *  Author: Stanislaus Reitenbach (AT-TWK)
 */

#include "gt_logging.h"

#include "gt_coreupgraderoutines.h"

bool
gtlab::internal::GtDataModelConverter::to200alpha1::run(QDomElement& domElement,
                                                        const QString& context)
{
    Q_UNUSED(domElement)
    Q_UNUSED(context)
    return true;
}
