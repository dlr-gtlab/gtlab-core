/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_colorpropertyitem.h"
#include "gt_colorproperty.h"

#include "gt_colorpropertyeditor.h"
//#include <QColorDialog>

GtColorPropertyItem::GtColorPropertyItem()
{
}

QVariant
GtColorPropertyItem::data(int column, int role) const
{
    if (!colorProperty()) return {};

    if (column < 0 || column >= 3) return {};

    if (column == 0)
    {
        return GtPropertyItem::data(column, role);
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (column == 2)
            {
                QColor c = colorProperty()->getVal();

                if (!c.isValid())
                {
                    return QStringLiteral("-");
                }

                return c.name(QColor::HexArgb);
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtColorPropertyItem::setData(int column, const QVariant& value,
                                GtObject* obj, int role)
{
    if (column == 0)
    {
        GtPropertyItem::setData(column, value, obj, role);
        return true;
    }

    if (column != 2) return false;

    if (role != Qt::EditRole) return false;

    return GtPropertyItem::setData(column, value, obj, role);
}

GtColorProperty*
GtColorPropertyItem::colorProperty() const
{
    return qobject_cast<GtColorProperty*>(m_property);
}

QWidget*
GtColorPropertyItem::editorWidget(
    QWidget* parent,
    const GtPropertyValueDelegate* /*delegate*/) const
{
    auto* e = new GtColorPropertyEditor(parent);

    return e;
}

void
GtColorPropertyItem::setEditorData(QWidget* editor,
                                      QVariant& /*var*/) const
{
    auto* e = static_cast<GtColorPropertyEditor*>(editor);

    e->setColorProperty(colorProperty());
}

void
GtColorPropertyItem::setModelData(QWidget* /*editor*/,
                                  QAbstractItemModel* /*model*/,
                                  const QModelIndex& /*index*/) const
{

}
