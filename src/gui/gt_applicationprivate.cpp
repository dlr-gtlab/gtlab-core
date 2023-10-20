/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_applicationprivate.cpp
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
