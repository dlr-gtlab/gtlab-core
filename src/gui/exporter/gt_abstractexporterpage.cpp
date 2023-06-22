/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractexporterpage.cpp
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"
#include "gt_palette.h"
#include "gt_application.h"

#include "gt_abstractexporterpage.h"

GtAbstractExporterPage::GtAbstractExporterPage(GtObject* obj) :
    m_obj(obj)
{
    gt::gui::applyThemeToWidget(this);
}

GtObject*
GtAbstractExporterPage::object()
{
    return m_obj;
}
