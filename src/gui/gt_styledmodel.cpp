/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_object.h"
#include "gt_objectui.h"
#include "gt_application.h"
#include "gt_logging.h"

#include "gt_styledmodel.h"

GtStyledModel::GtStyledModel(QObject* parent) : QIdentityProxyModel(parent)
{

}

QVariant
GtStyledModel::data(const QModelIndex& index, int role) const
{
//    GtObject* item = static_cast<GtObject*>(index.internalPointer());

    QModelIndex source_index = mapToSource(index);

    if (!source_index.isValid())
    {
        return QVariant();
    }

    GtObject* item = static_cast<GtObject*>(source_index.internalPointer());

    if (item)
    {
        GtObjectUI* oui = gtApp ? gtApp->defaultObjectUI(item) : nullptr;

        if (oui)
        {
            QVariant value = oui->data(item, role, index.column());

            if (value.isValid())
            {
//                if (gtApp->devMode())
//                {
//                    if (role == Qt::DecorationRole)
//                    {
//                        QIcon icn = value.value<QIcon>();

//                        QList<QSize> icnSizes = icn.availableSizes();

//                        for (int i = 0; i < icnSizes.size(); ++i)
//                        {
//                            if (icnSizes[i].width() != 16 ||
//                                    icnSizes[i].height() != 16)
//                            {
//                                gtWarning() << "invalid icon size for " <<
//                                               oui->metaObject()->className() <<
//                                               "!";
//                            }
//                        }
//                    }
//                }

                return value;
            }
        }
        else
        {
            GtObjectUI currOui;
            QVariant value = currOui.data(item, role, index.column());

            if (value.isValid())
            {
                return value;
            }
        }
    }

    return QIdentityProxyModel::data(index, role);
}

