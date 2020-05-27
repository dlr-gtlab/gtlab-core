/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectanalyzer.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 26.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTANALYZER_H
#define GTPROJECTANALYZER_H

#include "gt_core_exports.h"

#include "gt_project.h"
#include "gt_footprint.h"

class GtProjectAnalyzerImpl;

/**
 * @brief The GtProjectAnalyzer class provides methods to analyse project
 * information and compatibility checks regarding the current GTlab framework.
 */
class GtProjectAnalyzer
{
public:
    /**
     * @brief Constructor.
     * @param project Project for which information and compatibility checks
     * should be made
     */
    GT_CORE_EXPORT GtProjectAnalyzer(GtProject* project);

    /**
     * @brief Destructor.
     */
    GT_CORE_EXPORT ~GtProjectAnalyzer();

    /**
     * @brief Returns true if project has some irregularities compared to the
     * current GTlab framework. Returns false if no irregularities are found.
     * @return indicator for irregularities
     */
    GT_CORE_EXPORT bool hasIrregularities();

    /**
     * @brief Returns of list of unknown classes.
     * @return List of unknown classes.
     */
    GT_CORE_EXPORT QStringList unknownClasses();

    /**
     * @brief Returns footprint of project.
     * @return Footprint of project.
     */
    GT_CORE_EXPORT GtFootprint footPrint();

private:
    /// Private implementation
    GtProjectAnalyzerImpl* m_pimpl;

};

#endif // GTPROJECTANALYZER_H
