/* GTlab - Gas Turbine laboratory
 * Source File: gt_loadprojecthelper.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_LOADPROJECTHELPER_H
#define GT_LOADPROJECTHELPER_H

#include "gt_core_exports.h"

#include "gt_abstractloadinghelper.h"

class GtProject;
class GtObject;

/**
 * @brief The GtLoadProjectHelper class
 */
class GT_CORE_EXPORT GtLoadProjectHelper : public GtAbstractLoadingHelper
{
    Q_OBJECT

public:
    /**
     * @brief GtLoadProjectHelper
     */
    explicit GtLoadProjectHelper(GtProject* proj);

    /**
     * @brief run
     */
    void run() override;

    /**
     * @brief loadedObjects
     * @return
     */
    const QList<GtObject*>& loadedObjects();

    /**
     * @brief project
     * @return
     */
    GtProject* project();

private:
    /// Pointer to project
    GtProject* m_project;

    /// List of loaded objects
    QList<GtObject*> m_objects;

};

#endif // GT_LOADPROJECTHELPER_H
