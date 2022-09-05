/* GTlab - Gas Turbine laboratory
 * Source File: gtd_datamodelconverter.cpp
 * copyright 2009-2022 by DLR
 *
 *  Author: Stanislaus Reitenbach (AT-TWK)
 */

#include "gt_logging.h"

#include "gt_coreupgraderoutines.h"

bool
gtlab::internal::GtDataModelConverter::to200::run(QDomElement& domElement,
                                                  const QString& context)
{
    gtInfo() << "DO SOMETHING UPGRADE!";

    return true;
}
