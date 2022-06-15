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
GtGUI::Icon::plugin()
{
    return GtGUI::icon(QStringLiteral("pluginIcon.png"));
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

QIcon
GtGUI::Icon::process16()
{
    return GtGUI::icon(QStringLiteral("processIcon_16.png"));
}

QIcon
GtGUI::Icon::emptyElement()
{
    return GtGUI::icon(QStringLiteral("emptyElementIcon.png"));
}

QIcon
GtGUI::Icon::arrowRight()
{
    return GtGUI::icon(QStringLiteral("arrowrightIcon.png"));
}

QIcon
GtGUI::Icon::arrowRightBlue()
{
    return GtGUI::icon(QStringLiteral("arrowrightBlueIcon.png"));
}

QIcon
GtGUI::Icon::global()
{
    return GtGUI::icon(QStringLiteral("globalIcon.png"));
}

QPixmap
GtGUI::Pixmap::splash()
{
    return QPixmap(":pixmaps/splash.png");
}

QPixmap
GtGUI::Pixmap::accessLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/accessLogo.png"));
}

QPixmap
GtGUI::Pixmap::updateLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/updateLogo.png"));
}

QPixmap
GtGUI::Pixmap::changeLogLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/changelogLogo.png"));
}

QIcon
GtGUI::Icon::loop16()
{
    return GtGUI::icon(QStringLiteral("loopIcon_16.png"));
}

QIcon
GtGUI::Icon::bug()
{
    return GtGUI::icon(QStringLiteral("bugIcon.png"));
}

QIcon
GtGUI::Icon::fatal16()
{
    return GtGUI::icon(QStringLiteral("fatalIcon_16.png"));
}

QIcon
GtGUI::Icon::prp16()
{
    return GtGUI::icon(QStringLiteral("prpIcon_16.png"));
}

QIcon
GtGUI::Icon::examples16()
{
    return GtGUI::icon(QStringLiteral("examplesIcon_16.png"));
}

QIcon
GtGUI::Icon::double16()
{
    return GtGUI::icon(QStringLiteral("doubleIcon_16.png"));
}

QIcon
GtGUI::Icon::int16()
{
    return GtGUI::icon(QStringLiteral("intIcon_16.png"));
}

QIcon
GtGUI::Icon::o16()
{
    return GtGUI::icon(QStringLiteral("oIcon_16.png"));
}

QIcon
GtGUI::Icon::string16()
{
    return GtGUI::icon(QStringLiteral("stringIcon_16.png"));
}

QIcon
GtGUI::Icon::sleep16()
{
    return GtGUI::icon(QStringLiteral("sleepIcon_16.png"));
}

QIcon
GtGUI::Icon::inProgress16()
{
    return GtGUI::icon(QStringLiteral("inProgressIcon_16.png"));
}

QIcon
GtGUI::Icon::network16()
{
    return GtGUI::icon(QStringLiteral("networkIcon_16.png"));
}

QIcon
GtGUI::Icon::undo24()
{
    return GtGUI::icon(QStringLiteral("undoIcon_24.png"));
}

QIcon
GtGUI::Icon::redo24()
{
    return GtGUI::icon(QStringLiteral("redoIcon_24.png"));
}

QIcon
GtGUI::Icon::history16()
{
    return GtGUI::icon(QStringLiteral("historyIcon_16.png"));
}

QIcon
GtGUI::Icon::queue16()
{
    return GtGUI::icon(QStringLiteral("queueIcon_16.png"));
}

QIcon
GtGUI::Icon::stopRequest16()
{
    return GtGUI::icon(QStringLiteral("stopRequestIcon_16.png"));
}

QIcon
GtGUI::Icon::addElement16()
{
    return GtGUI::icon(QStringLiteral("addElementIcon_16.png"));
}

QString
GtGUI::Pixmap::logoString()
{
    return {":/pixmaps/gt-logo.png"};
}

QPixmap
GtGUI::Pixmap::jumpTo()
{
    return QPixmap(QStringLiteral(":/icons/jumpToIcon.png"));
}

QIcon
GtGUI::Icon::application()
{
    return GtGUI::icon(QStringLiteral("applicationIcon.png"));
}

QIcon
GtGUI::Icon::perspectives()
{
    return GtGUI::icon(QStringLiteral("perspectivesIcon.png"));
}

QIcon
GtGUI::Icon::input2()
{
    return GtGUI::icon(QStringLiteral("input2Icon.png"));
}

QIcon
GtGUI::Icon::pluginSettings()
{
    return GtGUI::icon(QStringLiteral("pluginSettingsIcon.png"));
}

QIcon
GtGUI::Icon::language()
{
    return GtGUI::icon(QStringLiteral("languageIcon.png"));
}

QIcon
GtGUI::Icon::login()
{
    return GtGUI::icon(QStringLiteral("loginIcon.png"));
}

QIcon
GtGUI::Icon::pathSettings()
{
    return GtGUI::icon(QStringLiteral("pathSettingsIcon.png"));
}

QIcon
GtGUI::Icon::restore16()
{
    return GtGUI::icon(QStringLiteral("restoreIcon_16.png"));
}

QIcon
GtGUI::Icon::frame()
{
    return GtGUI::icon(QStringLiteral("frameIcon.png"));
}

QIcon
GtGUI::Icon::unknown()
{
    return GtGUI::icon(QStringLiteral("unknownIcon.png"));
}

QIcon
GtGUI::Icon::exclamation16()
{
    return GtGUI::icon(QStringLiteral("exclamationIcon_16.png"));
}

QIcon
GtGUI::Icon::search16()
{
    return GtGUI::icon(QStringLiteral("searchIcon_16.png"));
}

QIcon
GtGUI::Icon::clear2_16()
{
    return GtGUI::icon(QStringLiteral("clearIcon2_16.png"));
}

QIcon
GtGUI::Icon::check24()
{
    return GtGUI::icon(QStringLiteral("checkIcon_24.png"));
}

QIcon
GtGUI::Icon::close24()
{
    return GtGUI::icon(QStringLiteral("closeIcon_24.png"));
}

QPixmap
GtGUI::Pixmap::downloaderLogo()
{
    return {QStringLiteral(":/pixmaps/downloaderLogo.png")};
}

QIcon
GtGUI::Icon::dropdown()
{
    return GtGUI::icon(QStringLiteral("dropDownIcon.png"));
}

QIcon
GtGUI::Icon::data16()
{
    return GtGUI::icon(QStringLiteral("dataIcon_16.png"));
}

QIcon
GtGUI::Icon::loop2_16()
{
    return GtGUI::icon(QStringLiteral("loopIcon2_16.png"));
}

QIcon
GtGUI::Icon::arrowLeft()
{
    return GtGUI::icon(QStringLiteral("arrowleftIcon.png"));
}

QIcon
GtGUI::Icon::minimize()
{
    return GtGUI::icon(QStringLiteral("minimizeIcons.png"));
}

QIcon
GtGUI::Icon::components16()
{
    return GtGUI::icon(QStringLiteral("componentsIcon_16.png"));
}

QIcon
GtGUI::Icon::perspectives16()
{
    return GtGUI::icon(QStringLiteral("perspectivesIcon_16.png"));
}

QIcon
GtGUI::Icon::addProject()
{
    return GtGUI::icon(QStringLiteral("addProjectIcon.png"));
}

QIcon
GtGUI::Icon::examples()
{
    return GtGUI::icon(QStringLiteral("examplesIcon.png"));
}

QIcon
GtGUI::Icon::help()
{
    return GtGUI::icon(QStringLiteral("questionIcon.png"));
}

QIcon
GtGUI::Icon::info()
{
    return GtGUI::icon(QStringLiteral("infoIcon.png"));
}

QIcon
GtGUI::Icon::arrowUp()
{
    return GtGUI::icon(QStringLiteral("arrowupIcon.png"));
}

QIcon
GtGUI::Icon::arrowDown()
{
    return GtGUI::icon(QStringLiteral("arrowdownIcon.png"));
}

QIcon
GtGUI::Icon::arrowDownBlue()
{
    return GtGUI::icon(QStringLiteral("arrowdownBlueIcon.png"));
}

QIcon
GtGUI::Icon::chain16()
{
   return GtGUI::icon(QStringLiteral("chainIcon_16.png"));
}

QIcon
GtGUI::Icon::post()
{
   return GtGUI::icon(QStringLiteral("postIcon.png"));
}

QIcon
GtGUI::Icon::labels()
{
    return GtGUI::icon(QStringLiteral("labelsIcon.png"));
}

QIcon
GtGUI::Icon::closedProject16()
{
    return GtGUI::icon(QStringLiteral("closedProjectIcon_16.png"));
}

QIcon
GtGUI::Icon::comment()
{
    return GtGUI::icon(QStringLiteral("commentIcon.png"));
}

QPixmap
GtGUI::Pixmap::logo()
{
    return {":/pixmaps/gt-logo.png"};
}

QIcon
GtGUI::Icon::stack()
{
    return GtGUI::icon(QStringLiteral("stackIcon.png"));
}

QIcon
GtGUI::Icon::collection16()
{
    return GtGUI::icon(QStringLiteral("collectionIcon_16.png"));
}

QIcon
GtGUI::Icon::globals16()
{
    return GtGUI::icon(QStringLiteral("globalsIcon_16.png"));
}

QIcon
GtGUI::Icon::map16()
{
    return GtGUI::icon(QStringLiteral("mapIcon_16.png"));
}

QIcon
GtGUI::Icon::carpetPlot()
{
    return GtGUI::icon(QStringLiteral("carpetPlotIcon2.png"));
}

QIcon
GtGUI::Icon::xyPlot16()
{
    return GtGUI::icon(QStringLiteral("schedules4Icon_16.png"));
}

QIcon
GtGUI::Icon::tsDiagram16()
{
    return GtGUI::icon(QStringLiteral("tsDiagramm_16.png"));
}

QIcon
GtGUI::Icon::histogram16()
{
    return GtGUI::icon(QStringLiteral("histogramIcon_16.png"));
}

QIcon
GtGUI::Icon::printer()
{
    return GtGUI::icon(QStringLiteral("printerIcon_16.png"));
}

QIcon
GtGUI::Icon::printPDF()
{
    return GtGUI::icon(QStringLiteral("printPDFIcon.png"));
}

QIcon
GtGUI::Icon::openProject16()
{
    return GtGUI::icon(QStringLiteral("openProjectIcon_16.png"));
}

QIcon
GtGUI::Icon::list16()
{
    return GtGUI::icon(QStringLiteral("listIcon_16.png"));
}

QIcon
GtGUI::Icon::question16()
{
    return GtGUI::icon(QStringLiteral("questionIcon_16.png"));
}

QIcon
GtGUI::Icon::zoom16()
{
    return GtGUI::icon(QStringLiteral("zoomIcon_16.png"));
}

QPixmap
GtGUI::Pixmap::question24()
{
    return QPixmap(":/icons/questionIcon_24.png");
}

QIcon
GtGUI::Icon::cross16()
{
    return GtGUI::icon(QStringLiteral("crossIcon_16.png"));
}

QIcon
GtGUI::Icon::checkSmall16()
{
    return GtGUI::icon(QStringLiteral("checkSmallIcon_16.png"));
}

QIcon
GtGUI::Icon::arrowDownBlueAll()
{
    return GtGUI::icon(QStringLiteral("arrowDownBlueAll.png"));
}

QIcon
GtGUI::Icon::arrowUpBlueAll()
{
    return GtGUI::icon(QStringLiteral("arrowUpBlueAll.png"));
}

QIcon
GtGUI::Icon::infoBlue()
{
    return GtGUI::icon(QStringLiteral("infoBlueIcon.png"));
}

QIcon
GtGUI::Icon::error()
{
    return GtGUI::icon(QStringLiteral("errorIcon.png"));
}

QIcon
GtGUI::Icon::jumpTo()
{
    return GtGUI::icon(QStringLiteral("jumpToIcon.png"));
}

QPixmap
GtGUI::Pixmap::printHeadline()
{
    return {":/pixmaps/print-headline.png"};
}

QIcon
GtGUI::Icon::inconsistentProject16()
{
    return GtGUI::icon(QStringLiteral("inconsistentProjectIcon_16.png"));
}
