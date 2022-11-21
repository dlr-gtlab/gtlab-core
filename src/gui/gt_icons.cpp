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
gt::gui::getIcon(QString const& iconPath)
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
gt::gui::icon::add16()
{
    return gt::gui::getIcon(QStringLiteral("addIcon_16.png"));
}

QIcon
gt::gui::icon::delete16()
{
    return gt::gui::getIcon(QStringLiteral("closeIcon_16.png"));
}

QIcon
gt::gui::icon::error16()
{
    return gt::gui::getIcon(QStringLiteral("errorIcon_16.png"));
}

QIcon
gt::gui::icon::export16()
{
    return gt::gui::getIcon(QStringLiteral("exportIcon_16.png"));
}

QIcon
gt::gui::icon::import16()
{
    return gt::gui::getIcon(QStringLiteral("importIcon_16.png"));
}

QIcon
gt::gui::icon::saveProject16()
{
    return gt::gui::getIcon(QStringLiteral("saveProjectIcon_16.png"));
}

QIcon
gt::gui::icon::info16()
{
    return gt::gui::getIcon(QStringLiteral("infoIcon_16.png"));
}

QIcon
gt::gui::icon::infoBlue16()
{
    return gt::gui::getIcon(QStringLiteral("infoBlueIcon_16.png"));
}

QIcon
gt::gui::icon::runProcess16()
{
    return gt::gui::getIcon(QStringLiteral("runProcessIcon_16.png"));
}

QIcon
gt::gui::icon::stop16()
{
    return gt::gui::getIcon(QStringLiteral("stopIcon_16.png"));
}

QIcon
gt::gui::icon::addProcess16()
{
    return gt::gui::getIcon(QStringLiteral("addProcessIcon_16.png"));
}

QIcon
gt::gui::icon::check16()
{
    return gt::gui::getIcon(QStringLiteral("checkIcon_16.png"));
}

QIcon
gt::gui::icon::clear16()
{
    return gt::gui::getIcon(QStringLiteral("clearIcon_16.png"));
}

QIcon
gt::gui::icon::config16()
{
    return gt::gui::getIcon(QStringLiteral("configIcon_16.png"));
}

QIcon
gt::gui::icon::input16()
{
    return gt::gui::getIcon(QStringLiteral("inputIcon_16.png"));
}

QIcon
gt::gui::icon::plugin16()
{
    return gt::gui::getIcon(QStringLiteral("pluginIcon_16.png"));
}

QIcon
gt::gui::icon::plugin()
{
    return gt::gui::getIcon(QStringLiteral("pluginIcon.png"));
}

QIcon
gt::gui::icon::session16()
{
    return gt::gui::getIcon(QStringLiteral("sessionIcon_16.png"));
}

QIcon
gt::gui::icon::session()
{
    return gt::gui::getIcon(QStringLiteral("sessionIcon.png"));
}


QIcon
gt::gui::icon::login16()
{
    return gt::gui::getIcon(QStringLiteral("loginIcon_16.png"));
}

QIcon
gt::gui::icon::update16()
{
    return gt::gui::getIcon(QStringLiteral("updateIcon_16.png"));
}

QIcon
gt::gui::icon::results16()
{
    return gt::gui::getIcon(QStringLiteral("resultsIcon_16.png"));
}

QIcon
gt::gui::icon::file16()
{
    return gt::gui::getIcon(QStringLiteral("fileIcon_16.png"));
}

QIcon
gt::gui::icon::folder16()
{
    return gt::gui::getIcon(QStringLiteral("folder_16.png"));
}

QIcon
gt::gui::icon::calculator16()
{
    return gt::gui::getIcon(QStringLiteral("calculatorIcon_16.png"));
}

QIcon
gt::gui::icon::copy16()
{
    return gt::gui::getIcon(QStringLiteral("copyIcon_16.png"));
}

QIcon
gt::gui::icon::paste16()
{
    return gt::gui::getIcon(QStringLiteral("pasteIcon_16.png"));
}

QIcon
gt::gui::icon::cut16()
{
    return gt::gui::getIcon(QStringLiteral("cutIcon_16.png"));
}

QIcon
gt::gui::icon::clone16()
{
    return gt::gui::getIcon(QStringLiteral("cloneIcon_16.png"));
}

QIcon
gt::gui::icon::connection16()
{
    return gt::gui::getIcon(QStringLiteral("connectionIcon_16.png"));
}

QIcon
gt::gui::icon::empty16()
{
    return gt::gui::getIcon(QStringLiteral("emptyIcon_16.png"));
}

QIcon
gt::gui::icon::deleteProject16()
{
    return gt::gui::getIcon(QStringLiteral("deleteProjectIcon_16.png"));
}

QIcon
gt::gui::icon::processFailed16()
{
    return gt::gui::getIcon(QStringLiteral("processFailedIcon_16.png"));
}

QIcon
gt::gui::icon::property16()
{
    return gt::gui::getIcon(QStringLiteral("propertyIcon_16.png"));
}

QIcon
gt::gui::icon::log16()
{
    return gt::gui::getIcon(QStringLiteral("logIcon_16.png"));
}

QIcon
gt::gui::icon::open16()
{
    return gt::gui::getIcon(QStringLiteral("openIcon_16.png"));
}

QIcon
gt::gui::icon::open2_16()
{
    return gt::gui::getIcon(QStringLiteral("openIcon2_16.png"));
}

QIcon
gt::gui::icon::open2_24()
{
    return gt::gui::getIcon(QStringLiteral("openIcon2_24.png"));
}

QIcon
gt::gui::icon::open2()
{
    return gt::gui::getIcon(QStringLiteral("openIcon2.png"));
}

QIcon
gt::gui::icon::skip16()
{
    return gt::gui::getIcon(QStringLiteral("skipIcon_16.png"));
}

QIcon
gt::gui::icon::process16()
{
    return gt::gui::getIcon(QStringLiteral("processIcon_16.png"));
}

QIcon
gt::gui::icon::emptyElement()
{
    return gt::gui::getIcon(QStringLiteral("emptyElementIcon.png"));
}

QIcon
gt::gui::icon::arrowRight()
{
    return gt::gui::getIcon(QStringLiteral("arrowrightIcon.png"));
}

QIcon
gt::gui::icon::arrowRightBlue()
{
    return gt::gui::getIcon(QStringLiteral("arrowrightBlueIcon.png"));
}

QIcon
gt::gui::icon::global()
{
    return gt::gui::getIcon(QStringLiteral("globalIcon.png"));
}

QPixmap
gt::gui::pixmap::splash()
{
    return QPixmap(":pixmaps/splash.png");
}

QPixmap
gt::gui::pixmap::accessLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/accessLogo.png"));
}

QPixmap
gt::gui::pixmap::updateLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/updateLogo.png"));
}

QPixmap
gt::gui::pixmap::changeLogLogo()
{
    return QPixmap(QStringLiteral(":/pixmaps/changelogLogo.png"));
}

QIcon
gt::gui::icon::loop16()
{
    return gt::gui::getIcon(QStringLiteral("loopIcon_16.png"));
}

QIcon
gt::gui::icon::bug()
{
    return gt::gui::getIcon(QStringLiteral("bugIcon.png"));
}

QIcon
gt::gui::icon::fatal16()
{
    return gt::gui::getIcon(QStringLiteral("fatalIcon_16.png"));
}

QIcon
gt::gui::icon::prp16()
{
    return gt::gui::getIcon(QStringLiteral("prpIcon_16.png"));
}

QIcon
gt::gui::icon::examples16()
{
    return gt::gui::getIcon(QStringLiteral("examplesIcon_16.png"));
}

QIcon
gt::gui::icon::double16()
{
    return gt::gui::getIcon(QStringLiteral("doubleIcon_16.png"));
}

QIcon
gt::gui::icon::int16()
{
    return gt::gui::getIcon(QStringLiteral("intIcon_16.png"));
}

QIcon
gt::gui::icon::o16()
{
    return gt::gui::getIcon(QStringLiteral("oIcon_16.png"));
}

QIcon
gt::gui::icon::string16()
{
    return gt::gui::getIcon(QStringLiteral("stringIcon_16.png"));
}

QIcon
gt::gui::icon::sleep16()
{
    return gt::gui::getIcon(QStringLiteral("sleepIcon_16.png"));
}

QIcon
gt::gui::icon::inProgress16()
{
    return gt::gui::getIcon(QStringLiteral("inProgressIcon_16.png"));
}

QIcon
gt::gui::icon::network16()
{
    return gt::gui::getIcon(QStringLiteral("networkIcon_16.png"));
}

QIcon
gt::gui::icon::undo24()
{
    return gt::gui::getIcon(QStringLiteral("undoIcon_24.png"));
}

QIcon
gt::gui::icon::redo24()
{
    return gt::gui::getIcon(QStringLiteral("redoIcon_24.png"));
}

QIcon
gt::gui::icon::history16()
{
    return gt::gui::getIcon(QStringLiteral("historyIcon_16.png"));
}

QIcon
gt::gui::icon::queue16()
{
    return gt::gui::getIcon(QStringLiteral("queueIcon_16.png"));
}

QIcon
gt::gui::icon::stopRequest16()
{
    return gt::gui::getIcon(QStringLiteral("stopRequestIcon_16.png"));
}

QIcon
gt::gui::icon::addElement16()
{
    return gt::gui::getIcon(QStringLiteral("addElementIcon_16.png"));
}

QString
gt::gui::pixmap::logoString()
{
    return {":/pixmaps/gt-logo.png"};
}

QPixmap
gt::gui::pixmap::jumpTo()
{
    return QPixmap(QStringLiteral(":/icons/jumpToIcon.png"));
}

QIcon
gt::gui::icon::application()
{
    return gt::gui::getIcon(QStringLiteral("applicationIcon.png"));
}

QIcon
gt::gui::icon::perspectives()
{
    return gt::gui::getIcon(QStringLiteral("perspectivesIcon.png"));
}

QIcon
gt::gui::icon::input2()
{
    return gt::gui::getIcon(QStringLiteral("input2Icon.png"));
}

QIcon
gt::gui::icon::pluginSettings()
{
    return gt::gui::getIcon(QStringLiteral("pluginSettingsIcon.png"));
}

QIcon
gt::gui::icon::language()
{
    return gt::gui::getIcon(QStringLiteral("languageIcon.png"));
}

QIcon
gt::gui::icon::login()
{
    return gt::gui::getIcon(QStringLiteral("loginIcon.png"));
}

QIcon
gt::gui::icon::pathSettings()
{
    return gt::gui::getIcon(QStringLiteral("pathSettingsIcon.png"));
}

QIcon
gt::gui::icon::restore16()
{
    return gt::gui::getIcon(QStringLiteral("restoreIcon_16.png"));
}

QIcon
gt::gui::icon::frame()
{
    return gt::gui::getIcon(QStringLiteral("frameIcon.png"));
}

QIcon
gt::gui::icon::unknown()
{
    return gt::gui::getIcon(QStringLiteral("unknownIcon.png"));
}

QIcon
gt::gui::icon::exclamation16()
{
    return gt::gui::getIcon(QStringLiteral("exclamationIcon_16.png"));
}

QIcon
gt::gui::icon::search16()
{
    return gt::gui::getIcon(QStringLiteral("searchIcon_16.png"));
}

QIcon
gt::gui::icon::clear2_16()
{
    return gt::gui::getIcon(QStringLiteral("clearIcon2_16.png"));
}

QIcon
gt::gui::icon::check24()
{
    return gt::gui::getIcon(QStringLiteral("checkIcon_24.png"));
}

QIcon
gt::gui::icon::close24()
{
    return gt::gui::getIcon(QStringLiteral("closeIcon_24.png"));
}

QPixmap
gt::gui::pixmap::downloaderLogo()
{
    return {QStringLiteral(":/pixmaps/downloaderLogo.png")};
}

QIcon
gt::gui::icon::dropdown()
{
    return gt::gui::getIcon(QStringLiteral("dropDownIcon.png"));
}

QIcon
gt::gui::icon::data16()
{
    return gt::gui::getIcon(QStringLiteral("dataIcon_16.png"));
}

QIcon
gt::gui::icon::loop2_16()
{
    return gt::gui::getIcon(QStringLiteral("loopIcon2_16.png"));
}

QIcon
gt::gui::icon::arrowLeft()
{
    return gt::gui::getIcon(QStringLiteral("arrowleftIcon.png"));
}

QIcon
gt::gui::icon::minimize()
{
    return gt::gui::getIcon(QStringLiteral("minimizeIcons.png"));
}

QIcon
gt::gui::icon::components16()
{
    return gt::gui::getIcon(QStringLiteral("componentsIcon_16.png"));
}

QIcon
gt::gui::icon::perspectives16()
{
    return gt::gui::getIcon(QStringLiteral("perspectivesIcon_16.png"));
}

QIcon
gt::gui::icon::addProject()
{
    return gt::gui::getIcon(QStringLiteral("addProjectIcon.png"));
}

QIcon
gt::gui::icon::examples()
{
    return gt::gui::getIcon(QStringLiteral("examplesIcon.png"));
}

QIcon
gt::gui::icon::help()
{
    return gt::gui::getIcon(QStringLiteral("questionIcon.png"));
}

QIcon
gt::gui::icon::info()
{
    return gt::gui::getIcon(QStringLiteral("infoIcon.png"));
}

QIcon
gt::gui::icon::arrowUp()
{
    return gt::gui::getIcon(QStringLiteral("arrowupIcon.png"));
}

QIcon
gt::gui::icon::arrowDown()
{
    return gt::gui::getIcon(QStringLiteral("arrowdownIcon.png"));
}

QIcon
gt::gui::icon::arrowDownBlue()
{
    return gt::gui::getIcon(QStringLiteral("arrowdownBlueIcon.png"));
}

QIcon
gt::gui::icon::chain16()
{
   return gt::gui::getIcon(QStringLiteral("chainIcon_16.png"));
}

QIcon
gt::gui::icon::post()
{
   return gt::gui::getIcon(QStringLiteral("postIcon.png"));
}

QIcon
gt::gui::icon::labels()
{
    return gt::gui::getIcon(QStringLiteral("labelsIcon.png"));
}

QIcon
gt::gui::icon::closedProject16()
{
    return gt::gui::getIcon(QStringLiteral("closedProjectIcon_16.png"));
}

QIcon
gt::gui::icon::comment()
{
    return gt::gui::getIcon(QStringLiteral("commentIcon.png"));
}

QPixmap
gt::gui::pixmap::logo()
{
    return {":/pixmaps/gt-logo.png"};
}

QIcon
gt::gui::icon::stack()
{
    return gt::gui::getIcon(QStringLiteral("stackIcon.png"));
}

QIcon
gt::gui::icon::collection16()
{
    return gt::gui::getIcon(QStringLiteral("collectionIcon_16.png"));
}

QIcon
gt::gui::icon::globals16()
{
    return gt::gui::getIcon(QStringLiteral("globalsIcon_16.png"));
}

QIcon
gt::gui::icon::map16()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon_16.png"));
}

QIcon
gt::gui::icon::carpetPlot()
{
    return gt::gui::getIcon(QStringLiteral("carpetPlotIcon2.png"));
}

QIcon
gt::gui::icon::xyPlot16()
{
    return gt::gui::getIcon(QStringLiteral("schedules4Icon_16.png"));
}

QIcon
gt::gui::icon::tsDiagram16()
{
    return gt::gui::getIcon(QStringLiteral("tsDiagramm_16.png"));
}

QIcon
gt::gui::icon::histogram16()
{
    return gt::gui::getIcon(QStringLiteral("histogramIcon_16.png"));
}

QIcon
gt::gui::icon::printer()
{
    return gt::gui::getIcon(QStringLiteral("printerIcon_16.png"));
}

QIcon
gt::gui::icon::printPDF()
{
    return gt::gui::getIcon(QStringLiteral("printPDFIcon.png"));
}

QIcon
gt::gui::icon::openProject16()
{
    return gt::gui::getIcon(QStringLiteral("openProjectIcon_16.png"));
}

QIcon
gt::gui::icon::list16()
{
    return gt::gui::getIcon(QStringLiteral("listIcon_16.png"));
}

QIcon
gt::gui::icon::question16()
{
    return gt::gui::getIcon(QStringLiteral("questionIcon_16.png"));
}

QIcon
gt::gui::icon::zoom16()
{
    return gt::gui::getIcon(QStringLiteral("zoomIcon_16.png"));
}

QPixmap
gt::gui::pixmap::question24()
{
    return QPixmap(":/icons/questionIcon_24.png");
}

QIcon
gt::gui::icon::cross16()
{
    return gt::gui::getIcon(QStringLiteral("crossIcon_16.png"));
}

QIcon
gt::gui::icon::checkSmall16()
{
    return gt::gui::getIcon(QStringLiteral("checkSmallIcon_16.png"));
}

QIcon
gt::gui::icon::arrowDownBlueAll()
{
    return gt::gui::getIcon(QStringLiteral("arrowDownBlueAll.png"));
}

QIcon
gt::gui::icon::arrowUpBlueAll()
{
    return gt::gui::getIcon(QStringLiteral("arrowUpBlueAll.png"));
}

QIcon
gt::gui::icon::infoBlue()
{
    return gt::gui::getIcon(QStringLiteral("infoBlueIcon.png"));
}

QIcon
gt::gui::icon::error()
{
    return gt::gui::getIcon(QStringLiteral("errorIcon.png"));
}

QIcon
gt::gui::icon::jumpTo()
{
    return gt::gui::getIcon(QStringLiteral("jumpToIcon.png"));
}

QPixmap
gt::gui::pixmap::printHeadline()
{
    return {":/pixmaps/print-headline.png"};
}

QIcon
gt::gui::icon::inconsistentProject16()
{
    return gt::gui::getIcon(QStringLiteral("inconsistentProjectIcon_16.png"));
}

QIcon
gt::gui::icon::upgradeProjectData()
{
    return gt::gui::getIcon(QStringLiteral("upgradeProjectDataIcon.png"));
}

QIcon
gt::gui::icon::duplicate()
{
    return gt::gui::getIcon(QStringLiteral("duplicateIcon_16.png"));
}
