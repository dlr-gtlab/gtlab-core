/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelcontainer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 21.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_project.h"
#include "gt_label.h"
#include "gt_labeldata.h"

#include "gt_labelcontainer.h"

GtLabelContainer::GtLabelContainer(GtProject* project, GtLabel* label) :
    m_project(project),
    m_label(label)
{
}

GtProject*
GtLabelContainer::project()
{
    return m_project;
}

GtLabel*
GtLabelContainer::label()
{
    return m_label;
}

bool
GtLabelContainer::isValid()
{
    if (m_project == Q_NULLPTR)
    {
        return false;
    }

    if (m_label == Q_NULLPTR)
    {
        return false;
    }

    GtLabelData* labelData = m_project->labelData();

    if (labelData == Q_NULLPTR)
    {
        return false;
    }

    return labelData->labelExists(m_label->objectName());
}
