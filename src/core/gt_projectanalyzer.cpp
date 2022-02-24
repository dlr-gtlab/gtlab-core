/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectanalyzer.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 26.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_footprint.h"
#include "gt_objectmemento.h"

#include "gt_projectanalyzer.h"

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
    void readData(GtProject* project);

};

GtProjectAnalyzer::GtProjectAnalyzer(GtProject* project)
    : m_pimpl{std::make_unique<GtProjectAnalyzerImpl>()}
{

    if (project != Q_NULLPTR)
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

    if (!m_pimpl->m_unknownClasses.isEmpty())
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
GtProjectAnalyzerImpl::readData(GtProject* project)
{
    // read footpring of project file
    m_footPrint = project->readFootprint();

    // check for unknown classes
    foreach (GtObject* c, project->findChildren<GtObject*>())
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
