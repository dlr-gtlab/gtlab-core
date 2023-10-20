/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TESTMDIEXTEXTERNALOBJECTVIEWER_H
#define TESTMDIEXTEXTERNALOBJECTVIEWER_H

#include "gt_mdiitem.h"
#include "gt_externalizedobjectfetcher.h"

#include "test_dmi_externalobject.h"

#include <QAbstractTableModel>

class ExternalObjectModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

public slots:

    void setExternalObject(GtObject* obj);

private:

    /// keeps external object fetched
    GtExternalizedObjectFetcher<TestDmiExternalObject> m_obj;
};

class TestMdiExtExternalObjectViewer : public GtMdiItem
{
    Q_OBJECT

public:

    Q_INVOKABLE TestMdiExtExternalObjectViewer();

    void setData(GtObject* obj);

private:

    QPointer<ExternalObjectModel> m_model;
};

#endif // TESTMDIEXTEXTERNALOBJECTVIEWER_H
