/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractimporterpage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"

#include "gt_abstractimporterpage.h"

GtAbstractImporterPage::GtAbstractImporterPage(GtObject* obj) :
    m_obj(obj)
{

}

GtObject*
GtAbstractImporterPage::object()
{
    return m_obj;
}
