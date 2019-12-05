#ifndef GTDOUBLEMAPCONTAINER_H
#define GTDOUBLEMAPCONTAINER_H

#include "gt_mdi_exports.h"

#include "gt_object.h"

#include <QMap>
#include <QStringList>

/**
 * @brief The GtDoubleMapContainer class
 */
class GT_MDI_EXPORT GtDoubleMapContainer : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList keys READ keys WRITE setKeys)
    Q_PROPERTY(QStringList values READ valuesString WRITE setValuesString)

public:
    GtDoubleMapContainer();

    QStringList keys() const;

    void setKeys(const QStringList& keys);

    QList<double> values() const;

    QStringList valuesString() const;

    void setValues(const QList<double>& values);

    void setValuesString(const QStringList& values);

    void addEntry(QString key, double value);

    bool isValid();

    QMap<QString, double> doubleMap();

    void clear();

    void assignValuesFrom(GtDoubleMapContainer* from);

private:

    QStringList m_keys;

    QList<double> m_values;
};

#endif // GTDOUBLEMAPCONTAINER_H
