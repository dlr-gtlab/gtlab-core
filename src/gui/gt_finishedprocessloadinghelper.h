/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */
 
#ifndef GTFINISHEDPROCESSLOADINGHELPER_H
#define GTFINISHEDPROCESSLOADINGHELPER_H


#include "gt_abstractloadinghelper.h"

#include "gt_gui_exports.h"

#include <memory>

class GtObjectMementoDiff;
/**
 * @brief The GtFinishedProcessLoadingHelper class
 * Carries the completed task diff through the GUI loading procedure.
 */
class GT_GUI_EXPORT GtFinishedProcessLoadingHelper :
        public GtAbstractLoadingHelper
{
    Q_OBJECT

public:
    /**
     * @brief GtFinishedProcessLoadingHelper
     * @param dataDiff Diff from task-start data to task results
     */
    explicit GtFinishedProcessLoadingHelper(GtObjectMementoDiff dataDiff);

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

private:
    /// pointer to summed diff object (result)
    std::unique_ptr<GtObjectMementoDiff> m_sumDiff;

};

#endif // GTFINISHEDPROCESSLOADINGHELPER_H
