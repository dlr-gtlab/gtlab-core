/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOR_H
#define GTCALCULATOR_H

#include "gt_core_exports.h"
#include "gt_labelproperty.h"
#include "gt_modeproperty.h"
#include "gt_processcomponent.h"
#include "gt_calculatordata.h"

#include <QPointer>
#include <memory>

class QDir;
class GtModeProperty;
class GtLabelProperty;
class GtAbstractRunnable;

/**
 * @brief The GtCalculator class
 */
class GT_CORE_EXPORT GtCalculator : public GtProcessComponent
{
    Q_OBJECT

public:
    /**
     * @brief The CalculatorInformation class
     * Struct for access to general information about the calculator
     * given while the registration.
     */
    struct GT_CORE_EXPORT CalculatorInformation
    {
    public:
    
        /**
         * @brief constructor
         * @param data Calculator data
         */
        explicit CalculatorInformation(GtCalculatorData data);

        /**
         * @brief author
         * @return author of the calculator
         */
        QString const& author() const;

        /**
         * @brief id
         * @return id of the calculator
         */
        QString const& id() const;

        /**
         * @brief version
         * @return version number of the calculator
         */
        GtVersionNumber const& version() const;

    private:
        GtCalculatorData m_dat;
    };

    /**
     * @brief Destructor
     */
    ~GtCalculator() override;

    /**
     * @brief Execution process of the calculator.
     * @return Whether execution process was successful or not.
     */
    bool exec() override;

    /**
     * @brief Main run method of the calculator.
     * @return Whether run process was successful or not.
     */
    virtual bool run() = 0;

    /**
     * @brief Returns list of all linked datamodel objects.
     * @return List of linked datamodel objects.
     */
    using GtProcessComponent::linkedObjects;

    /**
     * @brief Returns true if run should be marked as failed if warning flag
     * is activated. Otherwise false is returnes.
     * @return Whether run should be marked as failed or not.
     */
    bool runFailsOnWarning();

    /**
     * @brief calculatorInformation
     * @return
     */
    CalculatorInformation calculatorInformation();

protected:
    /**
     * @brief Constructor
     */
    GtCalculator();

    /// Delete temporary path indicator
    bool m_deleteTempPath;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

Q_DECLARE_METATYPE(GtCalculator*)

#endif // GTCALCULATOR_H
