/* GTlab - Gas Turbine laboratory
 * copyright 2023 by DLR
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTQMLOBJECTLISTMODEL_H
#define GTQMLOBJECTLISTMODEL_H


#include <QAbstractListModel>

/**
 * @brief A list model holding QObjects for Qml views
 */
class GtQmlObjectListModel : public QAbstractListModel
{

public:

    explicit GtQmlObjectListModel(QObject *parent = nullptr);

    void addObject(QObject* obj);

    int rowCount(const QModelIndex & /* parent */) const override;

    QVariant data(const QModelIndex &index, int /* role */) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QObjectList objects;
};
#endif // GTQMLOBJECTLISTMODEL_H
