/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_statecontainer.h
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATECONTAINER_H
#define GTSTATECONTAINER_H

#include <QPointer>

#include "gt_object.h"
#include "gt_core_exports.h"

class GtStateHandler;
class GtStateGroup;
class GtProject;
class GtState;

/**
 * @brief The GtStateContainer class
 */
class GT_CORE_EXPORT GtStateContainer : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent object.
     * @param Project object.
     */
    explicit GtStateContainer(GtObject* parent, GtProject* project = nullptr);

    /**
     * @brief Returns state container specific project.
     */
    GtProject* project();

    /**
     * @brief initializeState
     * @param id
     * @param initValue
     * @param guardian
     * @param forceInitialization
     * @return
     */
    GtState* initializeState(const QString& groupId, const QString& id,
                             const QString& path, const QVariant& initValue,
                             GtObject* guardian,
                             bool forceInitialization = false);

private:
    /// Project pointer.
    QPointer<GtProject> m_project;

    /**
     * @brief findGroup
     * @param id
     * @return
     */
    GtStateGroup* findGroup(const QString& id);

};

#endif // GTSTATECONTAINER_H
