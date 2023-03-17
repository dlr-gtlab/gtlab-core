/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_mdi_ext_externalobjectviewer.h"
#include "test_dmi_externalobject.h"

#include "gt_tableview.h"
#include "gt_utilities.h"
#include "gt_application.h"

#include <QVBoxLayout>

TestMdiExtExternalObjectViewer::TestMdiExtExternalObjectViewer()
{
    setObjectName("Test External Object Viewer");

    auto* lay = new QVBoxLayout(widget());
    auto* view = new GtTableView();
    view->setFrameStyle(QFrame::NoFrame);

    m_model = new ExternalObjectModel;
    m_model->setParent(this);
    view->setModel(m_model);

    lay->addWidget(view);
}

void
TestMdiExtExternalObjectViewer::setData(GtObject* obj)
{
    assert(m_model);

    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return;

    m_model->setExternalObject(extObj);

    connect(extObj, SIGNAL(dataChanged(GtObject*)),
            m_model.data(), SLOT(setExternalObject(GtObject*)),
            Qt::UniqueConnection);
}

void
ExternalObjectModel::setExternalObject(GtObject* obj)
{
    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return;

    beginResetModel();
    m_obj.set(extObj);
    endResetModel();
}

int
ExternalObjectModel::rowCount(const QModelIndex& parent) const
{
    if (!m_obj.isValid() || parent.isValid()) return 0;

    return m_obj.data()->params().size();
}

int
ExternalObjectModel::columnCount(const QModelIndex& parent) const
{
    if (!m_obj.isValid() || parent.isValid()) return 0;

    // params + values
    return 2;
}

QVariant
ExternalObjectModel::data(const QModelIndex& index, int role) const
{
    auto data = m_obj.data();

    if (!index.isValid() || !m_obj) return {};

    auto size = data->params().size();
    if (size == 0|| index.row() >= size)
    {
        return {};
    }

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case 0:
            return data->param(index.row());
        case 1:
            return data->value(index.row());
        }
        break;
    default:
        break;
    }

    return {};
}

QVariant
ExternalObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0 ||
        section >= columnCount() ||
        orientation != Qt::Horizontal)
    {
        return {};
    }

    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case 0:
            return tr("Params");
        case 1:
            return tr("Values");
        }
        break;
    default:
        break;
    }

    return {};
}
