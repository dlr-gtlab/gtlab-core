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

public slots:

    /**
     * @brief is schedule activated
     * @param obj: schedule ptr
     * @return is activated
     */
    bool isActivated(GtObject* obj);

    /**
     * @brief is schedule deactivated
     * @param obj: schedule ptr
     * @return is deactivated
     */
    bool isDeactivated(GtObject* obj);

    /**
     * @brief activate schedule
     * @param obj: schedule ptr
     */
    void activate(GtObject* obj);

    /**
     * @brief deactivate schedule
     * @param obj: schedule ptr
     */
    void deactivate(GtObject* obj);


protected:
    /**
     * @brief if the conrtol is inactive the font change to italic and the name
     * is striked out // if active the default fonts is setted
     * @param obj: pointer to control scheduele object
     * @param role: role
     * @param column: column
     * @return current font settings
     */
    QVariant specificData(GtObject* obj, int role, int column) const override;
};

#endif // GTCONTROLSCHEDULEUI_H
