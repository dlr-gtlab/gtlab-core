/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_extendedtaskdata.h
 *
 *  Created on: 08.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_EXTENDEDTASKDATA_H
#define GT_EXTENDEDTASKDATA_H

#include "gt_gui_exports.h"

#include <QIcon>

#include "gt_taskdata.h"

class GtExtendedTaskDataImpl;
class GtCustomProcessWizard;

#if defined(_MSC_VER) || defined(__APPLE__)
#include <memory>
typedef std::shared_ptr<GtExtendedTaskDataImpl> GtExtendedTaskData;
#else
#include <tr1/memory>
typedef std::tr1::shared_ptr<GtExtendedTaskDataImpl> GtExtendedTaskData;
#endif

#define GT_EXTENDED_TASK_DATA(A) GtExtendedTaskDataImpl::newExtendedTaskData(A::staticMetaObject);


/**
 * @brief The GtExtendedTaskDataImpl class
 */
class GT_GUI_EXPORT GtExtendedTaskDataImpl : public GtTaskDataImpl
{
public:
    /**
     * @brief Constructor.
     * @param metaData Task class meta object.
     */
    explicit GtExtendedTaskDataImpl(const QMetaObject& metaData);

    /**
     * @brief Destructor.
     */
    ~GtExtendedTaskDataImpl() override;

    /// Task icon
    QIcon icon;

    /// Custom task wizard
    /// Owning. @TODO: consider encapsulation
    GtCustomProcessWizard* wizard;

    /**
     * @brief Identifies if task data is valid.
     * @return Returns true if task data is valis. Otherwise false is returned.
     */
    bool isValid() const override;

    /**
     * @brief Generates new extended task data based on given meta data.
     * @param metaData Task class meta object.
     * @return Extended task data.
     */
    static GtExtendedTaskData newExtendedTaskData(const QMetaObject& metaData);

};

#endif // GT_EXTENDEDTASKDATA_H
