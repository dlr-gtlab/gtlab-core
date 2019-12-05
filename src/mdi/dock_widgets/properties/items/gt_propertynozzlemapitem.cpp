#include "gt_propertynozzlemapitem.h"
#include "gt_nozzlemap.h"
#include "gt_propertynozzlemapeditor.h"
#include "gt_nozzlemapproperty.h"

GtPropertyNozzleMapItem::GtPropertyNozzleMapItem()
{

}

QVariant
GtPropertyNozzleMapItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return QVariant();
    }

    if (m_property == Q_NULLPTR)
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (column == 2)
            {
                GtNozzleMap* map = dataProperty()->nozzleMap();

                if (map == Q_NULLPTR)
                {
                    return QStringLiteral("-");
                }

                return map->id();
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtPropertyNozzleMapItem::setData(int column, const QVariant& value,
                                 GtObject* obj, int role)
{
    if (column == 0)
    {
        GtPropertyItem::setData(column, value, obj, role);
        return true;
    }

    if (column != 2)
    {
        return false;
    }

    if (role != Qt::EditRole)
    {
        return false;
    }

    return GtPropertyItem::setData(column, value, obj, role);
}

GtNozzleMapProperty*
GtPropertyNozzleMapItem::dataProperty() const
{
    return qobject_cast<GtNozzleMapProperty*>(m_property);
}

QWidget*
GtPropertyNozzleMapItem::editorWidget(
        QWidget* parent,
        const GtPropertyValueDelegate* /*delegate*/) const
{
    qDebug() << "property nozzle map item editorWidget";
    GtPropertyNozzleMapEditor* e = new GtPropertyNozzleMapEditor(parent);

    return e;
}

void
GtPropertyNozzleMapItem::setEditorData(QWidget* editor, QVariant& /*var*/) const
{
    GtPropertyNozzleMapEditor* e =
        static_cast<GtPropertyNozzleMapEditor*>(editor);

    e->setNozzleMapProperty(dataProperty());
}

void
GtPropertyNozzleMapItem::setModelData(QWidget* /*editor*/,
                                      QAbstractItemModel* /*model*/,
                                      const QModelIndex& /*index*/) const
{

}
