#ifndef GTSTRINGMAPCONTAINER_H
#define GTSTRINGMAPCONTAINER_H

#include "gt_mdi_exports.h"

#include "gt_object.h"

#include <QMap>
#include <QStringList>

/**
 * @brief The GtStringMapContainer class
 */
class GT_MDI_EXPORT GtStringMapContainer : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList keys READ keys WRITE setKeys)
    Q_PROPERTY(QStringList values READ values WRITE setValues)

public:

    Q_INVOKABLE GtStringMapContainer();

    QStringList keys() const;

    void setKeys(const QStringList& keys);

    QStringList values() const;

    void setValues(const QStringList& values);

    void addEntry(QString key, QString value);

    bool isValid();

    QMap<QString, QString> stringMap();

    void clear();

    void assignValuesFrom(GtStringMapContainer* from);

private:

    QStringList m_keys;

    QStringList m_values;
};

#endif // GTSTRINGMAPCONTAINER_H
