/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 16.12.2015
 *  Author: Richard Becker (AT-TW)
 *  Tel.: +49 2203 601 3493
 */

#ifndef GT_TABLEVALUES_H
#define GT_TABLEVALUES_H

#include "gt_datamodel_exports.h"
#include "gt_object.h"
#include "gt_unit.h"

#include <QString>
#include <QVector>

class GT_DATAMODEL_EXPORT GtTableValues : public GtObject
{
   Q_OBJECT

    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QVector<double> values READ values WRITE setValues)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)
public:
    Q_INVOKABLE GtTableValues();
    ~GtTableValues();

    /**
     * @brief description Getter method for the values description.
     * @return Values description
     */
    QString description() const;

    /**
     * @brief setDescription Setter method for the the values description.
     * @param description New values description.
     */
    void setDescription(const QString &description);

    QString unit() const;
    void setUnit(const QString &unit);

    QVector<double> values() const;
    void setValues(const QVector<double> &values);

    void scale(double factor);

    template <class F>
    void scaleValuesByFunction(F& functionObject);

private:
    /// Axis unit
    QString m_unit;

    /// Brief description of axis
    QString m_brief;

    /// Array of axis ticks
    QVector<double> m_vals;
};

template <class F> void
GtTableValues::scaleValuesByFunction(F& functionObject)
{
    for (int i = 0; i < m_vals.size(); ++i)
    {
        m_vals[i] = functionObject(m_vals[i]);
    }
}


#endif // GT_TABLEVALUES_H
