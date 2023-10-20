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

class GtObject;
class GtObjectMemento;
class GtObjectMementoDiff;
/**
 * @brief The GtFinishedProcessLoadingHelper class
 * Class to handle the memento diff process at the end of a process whcih
 * was started in the gui
 *
 * reads some of the parameters of the ProcessExecutor
 *
 * A widget is displayed while the helper is working
 */
class GT_GUI_EXPORT GtFinishedProcessLoadingHelper :
        public GtAbstractLoadingHelper
{
    Q_OBJECT

public:
    /**
     * @brief GtFinishedProcessLoadingHelper
     * Constructor which reads some necessary data from the processExecutor
     *
     * @param data
     * @param sumDiff
     * @param source
     * @param taskName
     */
    GtFinishedProcessLoadingHelper(QList<GtObjectMemento>* data,
                                   QPointer<GtObject> source,
                                   const QString& taskName);

    ~GtFinishedProcessLoadingHelper();

    /**
     * @brief run
     * main function of the loading helper
     *
     * handles the memento analysis and application to the
     * source object of the task
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

    /// source object to buil diff
    QPointer<GtObject> m_source;

    /// Name of the task: used to give the used command a suitable name
    [[deprecated("unused")]] QString m_taskName;
};

#endif // GTFINISHEDPROCESSLOADINGHELPER_H
