/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */
 
#ifndef GTFINISHEDPROCESSLOADINGHELPER_H
#define GTFINISHEDPROCESSLOADINGHELPER_H


#include <QPointer>
#include "gt_abstractloadinghelper.h"

#include "gt_gui_exports.h"

#include <memory>

class GtObject;
class GtObjectMemento;
class GtObjectMementoDiff;
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
     * @param data Initial and result mementos to compare
     * @param sumDiff Resulting task diff
     * @param source Source object receiving the task diff
     * @param taskName Name of the task being merged
     */
    GtFinishedProcessLoadingHelper(QList<GtObjectMemento>* data,
                                   QPointer<GtObject> source,
                                   const QString& taskName);

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
    /// pointer to data to work on
    QList<GtObjectMemento>* m_data;

    /// pointer to summed diff object (result)
    std::unique_ptr<GtObjectMementoDiff> m_sumDiff;

    /// source object to apply the diff to
    QPointer<GtObject> m_source;
};

#endif // GTFINISHEDPROCESSLOADINGHELPER_H
