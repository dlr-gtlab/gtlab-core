/* GTlab - Gas Turbine laboratory
 * Source File: gt_posttemplatepath.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_posttemplatepath.h"

GtPostTemplatePath::GtPostTemplatePath(const QString& path)
{
    setObjectName(path);
}

QString
GtPostTemplatePath::path()
{
    return objectName();
}
