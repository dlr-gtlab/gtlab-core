/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 19.06.2024
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPORTTOMEMENTOCALCULATOR_H
#define GTEXPORTTOMEMENTOCALCULATOR_H

#include "gt_core_exports.h"

#include "gt_calculator.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_stringproperty.h"
#include "gt_savefilenameproperty.h"
#include "gt_objectlinkproperty.h"

class GT_CORE_EXPORT GtExportToMementoCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE GtExportToMementoCalculator();

    /**
     * @brief Main run method of the calculator.
     * @return Whether run process was successful or not.
     */
    bool run() override;

    static GtCalculatorData calculatorData();

private:
    /// file mode
    GtModeProperty m_fileMode;

    /// file mode types
    GtModeTypeProperty m_relativeFileMode, m_relativeToProjectFileMode,
        m_absoluteFileMode;

    /// Filepath absolute
    GtSaveFileNameProperty m_absoluteFilePath;

    /// Filepath relative
    GtStringProperty m_relativeFilePath;

    /// target object
    GtObjectLinkProperty m_targetObject;

};

#endif // GTEXPORTTOMEMENTOCALCULATOR_H
