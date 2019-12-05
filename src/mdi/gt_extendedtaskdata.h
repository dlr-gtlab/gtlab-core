/* GTlab - Gas Turbine laboratory
 * Source File: gt_extendedtaskdata.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_EXTENDEDTASKDATA_H
#define GT_EXTENDEDTASKDATA_H

#include "gt_mdi_exports.h"

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
class GT_MDI_EXPORT GtExtendedTaskDataImpl : public GtTaskDataImpl
{
public:
    /**
     * @brief Constructor.
     * @param metaData Task class meta object.
     */
    GtExtendedTaskDataImpl(const QMetaObject& metaData);

    /**
     * @brief Destructor.
     */
    virtual ~GtExtendedTaskDataImpl();

    /// Task icon
    QIcon icon;

    /// Custom task wizard
    GtCustomProcessWizard* wizard;

    /**
     * @brief Identifies if task data is valid.
     * @return Returns true if task data is valis. Otherwise false is returned.
     */
    bool isValid() const Q_DECL_OVERRIDE;

    /**
     * @brief Generates new extended task data based on given meta data.
     * @param metaData Task class meta object.
     * @return Extended task data.
     */
    static GtExtendedTaskData newExtendedTaskData(const QMetaObject& metaData);

};

#endif // GT_EXTENDEDTASKDATA_H
