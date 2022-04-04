/* GTlab - Gas Turbine laboratory
 * Source File: gt_icons.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.04.2022
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_icons.h"

QIcon
GtGUI::icon(QString const& iconPath)
{
    QIcon icon;
    // check if icon is an absolute resource path
    if (!iconPath.startsWith(":/"))
    {
        // preprend default resource path for icons
        icon.addFile(QStringLiteral(":/icons/") + iconPath,
                     QSize(), QIcon::Normal, QIcon::Off);
    }
    else
    {
        icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
    }

    return icon;
}

QIcon
GtGUI::Icon::add16()
{
    return GtGUI::icon(QStringLiteral("addIcon_16.png"));
}

QIcon
GtGUI::Icon::delete16()
{
    return GtGUI::icon(QStringLiteral("closeIcon_16.png"));
}

QIcon
GtGUI::Icon::error16()
{
    return GtGUI::icon(QStringLiteral("errorIcon_16.png"));
}

QIcon
GtGUI::Icon::export16()
{
    return GtGUI::icon(QStringLiteral("exportIcon_16.png"));
}

QIcon
GtGUI::Icon::import16()
{
    return GtGUI::icon(QStringLiteral("importIcon_16.png"));
}

QIcon
GtGUI::Icon::saveProject16()
{
    return GtGUI::icon(QStringLiteral("saveProjectIcon_16.png"));
}
