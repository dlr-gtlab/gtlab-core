/* GTlab - Gas Turbine laboratory
 * Source File: gt_saveprojecthelper.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 06.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTSAVEPROJECTHELPER_H
#define GTSAVEPROJECTHELPER_H

#include "gt_core_exports.h"

#include "gt_abstractloadinghelper.h"

class GtProject;

/**
 * @brief The GtSaveProjecthelper class
 */
class GT_CORE_EXPORT GtSaveProjectHelper : public GtAbstractLoadingHelper
{
    Q_OBJECT

public:

    /**
     * @brief GtSaveProjecthelper
     * @param proj
     */
    explicit GtSaveProjectHelper(GtProject* proj);

    /**
     * @brief run saves the project
     */
    void run() override;

    /**
     * @brief project
     * @return
     */
    GtProject* project();

    /**
     * @brief success
     * @return whether the saving process was successfull
     */
    bool success();

private:
    /// Pointer to project
    GtProject* m_project;
    /// success
    bool m_success;
};

#endif // GTSAVEPROJECTHELPER_H
