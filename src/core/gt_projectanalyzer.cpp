/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectanalyzer.cpp
 *
 *  Created on: 26.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_coreapplication.h"
#include "gt_footprint.h"
#include "gt_objectmemento.h"

#include "gt_projectanalyzer.h"

#include <memory>

class GtProjectAnalyzerImpl
{
public:
    /// Unknown classes
    QStringList m_unknownClasses;

    /// footprint of project file
    QString m_footPrint;

    /**
     * @brief readData
     * @param data
     * @return
     */
    void readData(const GtProject* project);

};

GtProjectAnalyzer::GtProjectAnalyzer(const GtProject *project)
    : m_pimpl{std::make_unique<GtProjectAnalyzerImpl>()}
{

    if (project)
    {
        // read information
        m_pimpl->readData(project);
    }
}

GtProjectAnalyzer::~GtProjectAnalyzer() = default;

bool
GtProjectAnalyzer::hasIrregularities()
{
    if (m_pimpl->m_footPrint.isEmpty())
    {
        return true;
    }

    if (!m_pimpl->m_unknownClasses.isEmpty() && gtApp->devMode())
    {
        return true;
    }

    GtFootprint footprint(m_pimpl->m_footPrint);

    return !footPrint().isCompatible();
}

QStringList
GtProjectAnalyzer::unknownClasses()
{
    return m_pimpl->m_unknownClasses;
}

GtFootprint
GtProjectAnalyzer::footPrint()
{
    return GtFootprint(m_pimpl->m_footPrint);
}

void
GtProjectAnalyzerImpl::readData(const GtProject* project)
{
    // read footpring of project file
    m_footPrint = project->readFootprint();

    // check for unknown classes
    for (GtObject const* c : project->findChildren())
    {
        if (c->isDummy())
        {
            GtObjectMemento m = c->toMemento();
            QString m_class = m.className();

            if (!m_unknownClasses.contains(m_class))
            {
                m_unknownClasses << m_class;
            }
        }
    }
}
