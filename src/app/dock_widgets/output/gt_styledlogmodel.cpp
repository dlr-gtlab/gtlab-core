/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QFont>

#include "gt_styledlogmodel.h"
#include "gt_logmodel.h"
#include "gt_icons.h"
#include "gt_colors.h"

#include <gt_loglevel.h>

GtStyledLogModel::GtStyledLogModel(QObject* parent) :
    QIdentityProxyModel(parent)
{

}

QVariant
GtStyledLogModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= gtLogModel->rowCount())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DecorationRole:
        {
            const int level =
                gtLogModel->data(index,
                                 GtLogModel::LogLevelRole).toInt();

            switch (level)
            {
                case gt::log::DebugLevel:
                    return gt::gui::icon::bug();

                case gt::log::InfoLevel:
                    return gt::gui::icon::infoBlue16();

                case gt::log::WarnLevel:
                    return gt::gui::icon::processFailed16();

                case gt::log::ErrorLevel:
                    return gt::gui::icon::error16();

                case gt::log::FatalLevel:
                    return gt::gui::icon::fatal16();
            }
        }

        case Qt::TextColorRole:
        {
            const int level =
                gtLogModel->data(index,
                                 GtLogModel::LogLevelRole).toInt();

            switch (level)
            {
                case gt::log::WarnLevel:
                    return gt::gui::color::warningText();

                case gt::log::ErrorLevel:
                    return gt::gui::color::errorText();

                case gt::log::FatalLevel:
                    return gt::gui::color::fatalText();
            }
        }

        case Qt::FontRole:
        {
            const int level =
                gtLogModel->data(index,
                                 GtLogModel::LogLevelRole).toInt();

            switch (level)
            {
                case gt::log::DebugLevel:
                {
                    QFont font =
                        QIdentityProxyModel::data(index, role).value<QFont>();
                    font.setItalic(true);
                    return font;
                }

                case gt::log::ErrorLevel:
                {
                    QFont font =
                        QIdentityProxyModel::data(index, role).value<QFont>();
                    font.setBold(true);
                    return font;
                }
            }
        }

        case Qt::BackgroundColorRole:
        {
            const int level =
                gtLogModel->data(index, GtLogModel::LogLevelRole).toInt();

            switch (level)
            {
                case gt::log::FatalLevel:
                    return gt::gui::color::fatalTextBackground();
            }
        }
    }

    return QIdentityProxyModel::data(index, role);
}

