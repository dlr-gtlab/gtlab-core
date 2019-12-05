/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTCONTROLSCHEDULEUI_H
#define GTCONTROLSCHEDULEUI_H

#include "gt_mdi_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtControlScheduleUI class
 */
class GT_MDI_EXPORT GtControlScheduleUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtControlScheduleUI
     */
    Q_INVOKABLE GtControlScheduleUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

    /**
     * @brief openWith
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) Q_DECL_OVERRIDE;

    /**
     * @brief hasValidationRegExp
     * @return
     */
    bool hasValidationRegExp() Q_DECL_OVERRIDE;

    /**
     * @brief validatorRegExp
     * @return
     */
    QRegExp validatorRegExp() Q_DECL_OVERRIDE;
};

#endif // GTCONTROLSCHEDULEUI_H
