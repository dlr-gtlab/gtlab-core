/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */
#ifndef GTFINISHEDPROCESSLOADINGHELPER_H
#define GTFINISHEDPROCESSLOADINGHELPER_H

#include <memory>

#include "gt_gui_exports.h"
#include "gt_abstractloadinghelper.h"
#include "gt_objectmementodiff.h"
#include "gt_taskrunner.h"

/**
 * @brief The GtFinishedProcessLoadingHelper class
 * Calculates the completed task diff in the GUI loading procedure.
 */
class GT_GUI_EXPORT GtFinishedProcessLoadingHelper :
        public GtAbstractLoadingHelper
{
    Q_OBJECT

public:
    /**
     * @brief GtFinishedProcessLoadingHelper
     * @param data Initial and result memento pairs to compare
     */
    explicit GtFinishedProcessLoadingHelper(QList<GtTaskMementoPair> data);

    ~GtFinishedProcessLoadingHelper();

    /**
     * @brief run
     * Loading procedure entry point.
     */
    void run() override;

    /**
     * @brief sumDiff - return summed Diff- data object
     * @return
     */
    GtObjectMementoDiff* sumDiff() const;

    bool isValid() const;

private:
    /// Task mementos owned for the duration of the loading procedure
    QList<GtTaskMementoPair> m_data;

    /// pointer to summed diff object (result)
    std::unique_ptr<GtObjectMementoDiff> m_sumDiff;

    /// Whether the task mementos formed valid pairs
    bool m_valid{false};
};

#endif // GTFINISHEDPROCESSLOADINGHELPER_H
