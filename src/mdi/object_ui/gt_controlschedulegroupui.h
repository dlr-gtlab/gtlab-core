/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulegroupui.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.05.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTROLSCHEDULEGROUPUI_H
#define GTCONTROLSCHEDULEGROUPUI_H

#include "gt_objectui.h"

#include <QStringList>

class GtControlScheduleGroupUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtControlScheduleGroupUI
     */
    Q_INVOKABLE GtControlScheduleGroupUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

private:

    /**
     * @brief uniqueObjectNameHelper
     * @param name
     * @param objs
     * @param initName
     * @param iteration
     * @return
     */
    QString uniquePostTemplateIdHelper(const QString& name,
                                       const QStringList& strings,
                                       QString initName = QString(),
                                       int iteration = 0)
    {
        foreach (const QString& str, strings)
        {
            if (name == str)
            {
                if (initName.isEmpty())
                {
                    initName = name;
                }

                iteration++;

                QString new_name = initName + QStringLiteral("[") +
                        QString::number(iteration + 1) + QStringLiteral("]");

                return uniquePostTemplateIdHelper(new_name, strings, initName,
                                              iteration);
            }
        }

        return name;
    }

public slots:

    /**
     * @brief routine to add a control schedule from file
     * @param obj
     */
    void addControlSchedule(GtObject* obj);

    /**
     * @brief routine to create a new control schedule from sketch
     * @param obj
     */
    void createControlSchedule(GtObject* obj);

private slots:
    /**
     * @brief onLineEditCheckRequired
     */
    void onLineEditCheckRequired();
};

#endif // GTCONTROLSCHEDULEGROUPUI_H
