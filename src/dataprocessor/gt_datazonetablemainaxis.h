#ifndef GTDATAZONETABLEMAINAXIS_H
#define GTDATAZONETABLEMAINAXIS_H

#include "gt_datamodel_exports.h"

#include <QStringList>

#include "gt_object.h"

class GT_DATAMODEL_EXPORT GtDataZoneTableMainAxis : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList varAxisTicks READ
               ticks WRITE setTicks)
    Q_PROPERTY(QString varName READ name WRITE setName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(bool active READ active WRITE setActive)

public:

    enum AxisType
    {
        X,
        Y,
        Z
    };

    // 20.10.2016 changed: only Standard constructor is now invokable

    GtDataZoneTableMainAxis(const QString& name, AxisType axType,
                            QString title = QString());

    Q_INVOKABLE GtDataZoneTableMainAxis();

    int size() const;

    void setTicks(QStringList ticks);

    QStringList ticks() const;

    int indexOf(const QString &str, bool* ok = nullptr) const;

    QString name() const;

    void setName(const QString& name);

    QString title() const;

    void setTitle(const QString& title);

    bool active() const;

    void setActive(bool active);

    AxisType axisType() const;

private:

    QString m_title;

    QString m_name;

    QStringList m_axisTicks;

    AxisType m_axType;

    bool m_active;

};

#endif // GTDATAZONETABLEMAINAXIS_H
