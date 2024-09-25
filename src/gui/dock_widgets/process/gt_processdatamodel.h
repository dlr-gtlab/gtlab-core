/* GTlab - Gas Turbine laboratory
 * Source File: gt_processdatamodel.h
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 20.09.2024
 * Author: Marvin Noethen (DLR AT-TWK)
 */

#ifndef GTPROCESSDATAMODEL_H
#define GTPROCESSDATAMODEL_H

#include <QAbstractItemModel>

#include "gt_gui_exports.h"

class GtObject;
class GtProcessData;
class GtAbstractObjectFactory;


class GT_GUI_EXPORT GtProcessDataModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit GtProcessDataModel(QObject* parent = nullptr);

    /// QAbstractItemModel methods
    QModelIndex index(int row, int col,
                      const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;


    /// GtProcessDataModel specific methods
    QModelIndex indexFromObject(GtObject* obj, int col = 0) const;

    // this method could be outsourced to a namespace
    GtObject* objectFromIndex(const QModelIndex& index) const;

    // interacts with the gtDataModel to append and delete elements
    QModelIndex appendChild(GtObject* child, GtObject* parent);

    bool deleteFromModel(QList<GtObject*> objects);

    /// here we could also define the following methods

    // QModelIndexList appendChildren(const QList<GtObject*>& children,
    //                                   GtObject* parent);

    // bool deleteFromModel(GtObject* obj) override;


    /// purely wrapped gtDataModel methods
    // void reduceToParents(QList<GtObject*>& toReduce);

    // QMimeData* mimeDataFromObject(GtObject* obj, bool newUuid = false) const;

    // GtObject* objectFromMimeData(const QMimeData* mime,
    //                              bool newUuid = false,
    //                              GtAbstractObjectFactory* factory = nullptr);


    QStringList mimeTypes() const override;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;

protected:
    Qt::DropActions supportedDropActions() const override;

    Qt::DropActions supportedDragActions() const override;

private:
    GtProcessData* currentProcessData() const;

    QModelIndex mapToGtDataModel(const QModelIndex& index) const;

    QModelIndexList mapToGtDataModel(const QModelIndexList& indexes) const;

private slots:
    void resetModel();
//signals:
//    void triggerEndResetDataModelView();
};

#endif // GTPROCESSDATAMODEL_H
