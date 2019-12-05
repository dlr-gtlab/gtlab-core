/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTCONTROLSCHEDULEPROPERTY_H
#define GTCONTROLSCHEDULEPROPERTY_H

#include "gt_datamodel_exports.h"

#include <QPointer>

#include "gt_property.h"
//#include "gt_object.h"
//#include "gt_controlschedule.h"

class GtControlSchedule;
class GtObject;

/**
 * @brief The GtControlScheduleProperty class
 */
class GT_DATAMODEL_EXPORT GtControlScheduleProperty :
        public GtProperty<QString>
{
    Q_OBJECT

public:
    GtControlScheduleProperty(const QString& name,
                              const QString& brief,
                              const QString& id,
                              GtObject* obj);

    /**
     * @brief valueToVariant
     * @return
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = 0) Q_DECL_OVERRIDE;

    /**
     * @brief controlSchedule
     * @return
     */
    GtControlSchedule* controlSchedule();

    /**
     * @brief object
     * @return
     */
    GtObject* object();

//    void setObject(GtObject* o);

private:

    QPointer<GtObject> m_obj;
};

#endif // GTCONTROLSCHEDULEPROPERTY_H
