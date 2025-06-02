/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_statemodel.cpp
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_statehandler.h"
#include "gt_statecontainer.h"
#include "gt_stategroup.h"
#include "gt_state.h"
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_qtutilities.h"

#include "gt_statemodel.h"

GtStateModel::GtStateModel(QObject* parent) :
    GtObjectModel(gtStateHandler, parent)
{

}

int
GtStateModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4;
}

QVariant
GtStateModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    GtObject* item = objectFromIndex(index);

    if (!item)
    {
        return QVariant();
    }

    if (index.column() == 0)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                if (GtState* state = qobject_cast<GtState*>(item))
                {
                    return state->id();
                }

                return GtObjectModel::data(index, role);
            }

            case Qt::DecorationRole:
            {
                if (GtStateContainer* cont =
                            qobject_cast<GtStateContainer*>(item))
                {
                    if (!cont->project())
                    {
                        if (cont->objectName() == QLatin1String("-- Global --"))
                        {
                            return gt::gui::icon::global();
                        }

                        return gt::gui::icon::folder();
                    }

                    return gt::gui::icon::projectOpen();
                }
                else if (qobject_cast<GtStateGroup*>(item))
                {
                    return gt::gui::icon::folder();
                }
                else if (qobject_cast<GtState*>(item))
                {
                    return gt::gui::icon::objectEmpty();
                }

                break;
            }

            default:
                break;
        }
    }
    else if (index.column() == 1)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                if (GtState* state = qobject_cast<GtState*>(item))
                {
                    return variantToString(state->getValue());
                }

                return GtObjectModel::data(index, role);
            }

            default:
                break;
        }
    }
    else if (index.column() == 2)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                if (GtState* state = qobject_cast<GtState*>(item))
                {
                    return state->getValue().typeName();
                }

                return GtObjectModel::data(index, role);
            }

            case Qt::ForegroundRole:
            {
                return gt::gui::color::disabled();
            }

            default:
                break;
        }
    }
    else if (index.column() == 3)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                if (GtState* state = qobject_cast<GtState*>(item))
                {
                    return state->path();
                }

                return GtObjectModel::data(index, role);
            }

            case Qt::ForegroundRole:
            {
                return gt::gui::color::disabled();
            }

            default:
                break;
        }
    }

    return GtObjectModel::data(index, role);
}

QVariant
GtStateModel::headerData(int section, Qt::Orientation orientation,
                         int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            switch (section)
            {
                case 0:
                    return tr("Name");

                case 1:
                    return tr("Value");

                case 2:
                    return tr("Type");

                case 3:
                    return tr("Path");
            }
        }
    }

    return GtObjectModel::headerData(section, orientation, role);
}

void
GtStateModel::reload()
{
    setRootObject(gtStateHandler);
}

QString
GtStateModel::variantToString(const QVariant& variant) const
{
    QString retval;

    switch (gt::metaTypeId(variant))
    {
        case QMetaType::QPoint:
        {
            QPoint point = variant.toPoint();
            retval = QStringLiteral("[") + QString::number(point.x()) +
                     QStringLiteral(", ") + QString::number(point.y()) +
                     QStringLiteral("]");
            break;
        }

        case QMetaType::QPointF:
        {
            QPointF point = variant.toPointF();
            retval = QStringLiteral("[") + QString::number(point.x()) +
                     QStringLiteral(", ") + QString::number(point.y()) +
                     QStringLiteral("]");
            break;
        }

        default:
        {
            retval = variant.toString();
            break;
        }
    }

    return retval;
}
