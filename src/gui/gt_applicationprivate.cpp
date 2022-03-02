/* GTlab - Gas Turbine laboratory
 * Source File: gt_applicationprivate.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_applicationprivate.h"

GtApplicationPrivate::GtApplicationPrivate(QObject* parent) : QObject(parent),
    m_commandRoot(nullptr)
{

}
