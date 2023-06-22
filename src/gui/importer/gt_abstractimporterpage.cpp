/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractimporterpage.cpp
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"
#include "gt_palette.h"
#include "gt_application.h"

#include "gt_abstractimporterpage.h"

GtAbstractImporterPage::GtAbstractImporterPage(GtObject* obj) :
    m_obj(obj)
{
    gt::gui::applyThemeToWidget(this);
}

GtObject*
GtAbstractImporterPage::object()
{
    return m_obj;
}
