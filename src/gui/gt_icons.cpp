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

QIcon
GtGUI::Icon::info16()
{
    return GtGUI::icon(QStringLiteral("infoIcon_16.png"));
}

QIcon
GtGUI::Icon::infoBlue16()
{
    return GtGUI::icon(QStringLiteral("infoBlueIcon_16.png"));
}

QIcon
GtGUI::Icon::runProcess16()
{
    return GtGUI::icon(QStringLiteral("runProcessIcon_16.png"));
}

QIcon
GtGUI::Icon::stop16()
{
    return GtGUI::icon(QStringLiteral("stopIcon_16.png"));
}

QIcon
GtGUI::Icon::addProcess16()
{
    return GtGUI::icon(QStringLiteral("addProcessIcon_16.png"));
}

QIcon
GtGUI::Icon::check16()
{
    return GtGUI::icon(QStringLiteral("checkIcon_16.png"));
}

QIcon
GtGUI::Icon::clear16()
{
    return GtGUI::icon(QStringLiteral("clearIcon_16.png"));
}

QIcon
GtGUI::Icon::config16()
{
    return GtGUI::icon(QStringLiteral("configIcon_16.png"));
}

QIcon
GtGUI::Icon::input16()
{
    return GtGUI::icon(QStringLiteral("inputIcon_16.png"));
}

QIcon
GtGUI::Icon::plugin16()
{
    return GtGUI::icon(QStringLiteral("pluginIcon_16.png"));
}

QIcon
GtGUI::Icon::session16()
{
    return GtGUI::icon(QStringLiteral("sessionIcon_16.png"));
}

QIcon
GtGUI::Icon::login16()
{
    return GtGUI::icon(QStringLiteral("loginIcon_16.png"));
}

QIcon
GtGUI::Icon::update16()
{
    return GtGUI::icon(QStringLiteral("updateIcon_16.png"));
}

QIcon
GtGUI::Icon::results16()
{
    return GtGUI::icon(QStringLiteral("resultsIcon_16.png"));
}

QIcon
GtGUI::Icon::file16()
{
    return GtGUI::icon(QStringLiteral("fileIcon_16.png"));
}

QIcon
GtGUI::Icon::folder16()
{
    return GtGUI::icon(QStringLiteral("folder_16.png"));
}

QIcon
GtGUI::Icon::calculator16()
{
    return GtGUI::icon(QStringLiteral("calculatorIcon_16.png"));
}

QIcon
GtGUI::Icon::copy16()
{
    return GtGUI::icon(QStringLiteral("copyIcon_16.png"));
}

QIcon
GtGUI::Icon::paste16()
{
    return GtGUI::icon(QStringLiteral("pasteIcon_16.png"));
}

QIcon
GtGUI::Icon::cut16()
{
    return GtGUI::icon(QStringLiteral("cutIcon_16.png"));
}

QIcon
GtGUI::Icon::clone16()
{
    return GtGUI::icon(QStringLiteral("cloneIcon_16.png"));
}

QIcon
GtGUI::Icon::connection16()
{
    return GtGUI::icon(QStringLiteral("connectionIcon_16.png"));
}

QIcon
GtGUI::Icon::empty16()
{
    return GtGUI::icon(QStringLiteral("emptyIcon_16.png"));
}

QIcon
GtGUI::Icon::deleteProject16()
{
    return GtGUI::icon(QStringLiteral("deleteProjectIcon_16.png"));
}

QIcon
GtGUI::Icon::processFailed16()
{
    return GtGUI::icon(QStringLiteral("processFailedIcon_16.png"));
}

QIcon
GtGUI::Icon::property16()
{
    return GtGUI::icon(QStringLiteral("propertyIcon_16.png"));
}

QIcon
GtGUI::Icon::log16()
{
    return GtGUI::icon(QStringLiteral("logIcon_16.png"));
}

QIcon
GtGUI::Icon::open16()
{
    return GtGUI::icon(QStringLiteral("openIcon_16.png"));
}

QIcon
GtGUI::Icon::skip16()
{
    return GtGUI::icon(QStringLiteral("skipIcon_16.png"));
}

