/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTPROPERTYCONTROLSCHEDULEEDITOR_H
#define GTPROPERTYCONTROLSCHEDULEEDITOR_H

#include <QWidget>
#include <QPointer>

class QLineEdit;
class GtControlScheduleProperty;
class GtObject;

class GtPropertyControlScheduleEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyControlScheduleEditor
     */
    GtPropertyControlScheduleEditor(QWidget* parent = Q_NULLPTR);

    /**
     * @brief setControlScheduleProperty
     * @param prop
     */
    void setControlScheduleProperty(GtControlScheduleProperty* prop);

private:
    ///
    QLineEdit* m_scheduleId;

    ///
    QPointer<GtControlScheduleProperty> m_prop;

    /**
     * @brief updateText
     */
    void updateText();

private slots:
    /**
     * @brief selectMap
     */
    void selectMap();

    /**
     * @brief deleteMap
     */
    void deleteMap();
};

#endif // GTPROPERTYCONTROLSCHEDULEEDITOR_H
