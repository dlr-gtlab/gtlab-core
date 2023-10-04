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
{ }

QVariant
GtStyledLogModel::data(const QModelIndex& index, int role) const
{
    /// helper method to access logging level of index
    const auto loggingLevel = [](const QModelIndex& index){
        int level = gtLogModel->data(index, GtLogModel::LevelRole).toInt();
        return gt::log::levelFromInt(level);
    };

    int col = index.column();

    switch (role)
    {
    case Qt::DecorationRole:
        // convert level to icon
        if (col == 0)
        {
            switch (loggingLevel(index))
            {
            case gt::log::TraceLevel:
                return gt::gui::icon::traceColorized();
            case gt::log::DebugLevel:
                return gt::gui::icon::bugColorized();
            case gt::log::InfoLevel:
                return gt::gui::icon::infoColorized();
            case gt::log::WarningLevel:
                return gt::gui::icon::warningColorized();
            case gt::log::ErrorLevel:
                return gt::gui::icon::errorColorized();
            case gt::log::FatalLevel:
                return gt::gui::icon::fatalColorized();
            default:
                break;
            }
        }
        break;
    case Qt::DisplayRole:
        // we only want to display the icon not the text
        if (col == 0)
        {
            return {};
        }
        break;
    case Qt::ToolTipRole:
        // tooltip for level
        if (col == 0)
        {
            auto level = gt::log::levelToString(loggingLevel(index));
            return QString::fromStdString(level);
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignTop;
    case Qt::ForegroundRole:
        switch (loggingLevel(index))
        {
        case gt::log::TraceLevel:
        case gt::log::DebugLevel:
            return gt::gui::color::debugText();
        case gt::log::InfoLevel:
            return gt::gui::color::infoText();
        case gt::log::WarningLevel:
            return gt::gui::color::warningText();
        case gt::log::ErrorLevel:
            return gt::gui::color::errorText();
        case gt::log::FatalLevel:
            return gt::gui::color::fatalText();
        default:
            break;
        }
        break;
    case Qt::BackgroundRole:
        switch (loggingLevel(index))
        {
        case gt::log::FatalLevel:
            return gt::gui::color::fatalTextBackground();
        default:
            break;
        }
        break;
    case Qt::FontRole:
        switch (loggingLevel(index))
        {
        case gt::log::TraceLevel:
        case gt::log::DebugLevel:
        {
            QFont font = QIdentityProxyModel::data(index, role).value<QFont>();
            font.setItalic(true);
            return font;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }

    return QIdentityProxyModel::data(index, role);
}

QVariant
GtStyledLogModel::headerData(int section,
                             Qt::Orientation orientation,
                             int role) const
{
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignLeft;
    }
    return QIdentityProxyModel::headerData(section, orientation, role);
}

