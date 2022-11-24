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
gt::gui::icon::closeProject()
{
    return gt::gui::getIcon(QStringLiteral("closeProjectIcon.png"));
}

QIcon
gt::gui::icon::closeProject16()
{
    return gt::gui::getIcon(QStringLiteral("closeProjectIcon_16.png"));
}

QIcon
gt::gui::icon::closeProject24()
{
    return gt::gui::getIcon(QStringLiteral("closeProjectIcon_24.png"));
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
gt::gui::icon::map()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon.png"));
}

QIcon
gt::gui::icon::map24()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon_24.png"));
}

QIcon
gt::gui::icon::noMap()
{
    return gt::gui::getIcon(QStringLiteral("noMapIcon.png"));
}

QIcon
gt::gui::icon::exportMap()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon.png"));
}

QIcon
gt::gui::icon::exportMap16()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon_16.png"));
}

QIcon
gt::gui::icon::exportMap24()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon_24.png"));
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

QIcon
gt::gui::icon::inconsistentProject16()
{
    return gt::gui::getIcon(QStringLiteral("inconsistentProjectIcon_16.png"));
}

QIcon
gt::gui::icon::blades()
{
    return gt::gui::getIcon(QStringLiteral("bladesIcon.png"));
}

QIcon
gt::gui::icon::gas()
{
    return gt::gui::getIcon(QStringLiteral("gasIcon.png"));
}

QIcon
gt::gui::icon::gas16()
{
    return gt::gui::getIcon(QStringLiteral("gasIcon_16.png"));
}

QIcon
gt::gui::icon::gas24()
{
    return gt::gui::getIcon(QStringLiteral("gasIcon_24.png"));
}

QIcon
gt::gui::icon::gasTypes()
{
    return gt::gui::getIcon(QStringLiteral("gastypesIcon.png"));
}

QIcon
gt::gui::icon::gasTypes16()
{
    return gt::gui::getIcon(QStringLiteral("gastypesIcon_16.png"));
}

QIcon
gt::gui::icon::gasTypes24()
{
    return gt::gui::getIcon(QStringLiteral("gastypesIcon_24.png"));
}

QIcon
gt::gui::icon::grid()
{
    return gt::gui::getIcon(QStringLiteral("gridIcon.png"));
}

QIcon
gt::gui::icon::gridSnap()
{
    return gt::gui::getIcon(QStringLiteral("gridsnapIcon.png"));
}

QIcon
gt::gui::icon::showText()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon.png"));
}

QIcon
gt::gui::icon::showText16()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon_16.png"));
}

QIcon
gt::gui::icon::showText24()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon_24.png"));
}

QIcon
gt::gui::icon::move()
{
    return gt::gui::getIcon(QStringLiteral("moveIcon.png"));
}

QIcon
gt::gui::icon::move16()
{
    return gt::gui::getIcon(QStringLiteral("moveIcon_16.png"));
}

QIcon
gt::gui::icon::move24()
{
    return gt::gui::getIcon(QStringLiteral("moveIcon_24.png"));
}

QIcon
gt::gui::icon::editPen()
{
    return gt::gui::getIcon(QStringLiteral("own_editPen.png"));
}

QIcon
gt::gui::icon::resize()
{
    return gt::gui::getIcon(QStringLiteral("own_resize.png"));
}

QIcon
gt::gui::icon::plus()
{
    return gt::gui::getIcon(QStringLiteral("plusIcon2.png"));
}

QIcon
gt::gui::icon::minus()
{
    return gt::gui::getIcon(QStringLiteral("minusIcon2.png"));
}

QIcon
gt::gui::icon::multiply()
{
    return gt::gui::getIcon(QStringLiteral("multiplyIcon.png"));
}

QIcon
gt::gui::icon::divide()
{
    return gt::gui::getIcon(QStringLiteral("divideIcon.png"));
}

QIcon
gt::gui::icon::square()
{
    return gt::gui::getIcon(QStringLiteral("squareIcon.png"));
}

QIcon
gt::gui::icon::squareRoot()
{
    return gt::gui::getIcon(QStringLiteral("squareRootIcon.png"));
}

QIcon
gt::gui::icon::convergence()
{
    return gt::gui::getIcon(QStringLiteral("convIcon.png"));
}

QIcon
gt::gui::icon::convergence16()
{
    return gt::gui::getIcon(QStringLiteral("convIcon_16.png"));
}

QIcon
gt::gui::icon::convergence24()
{
    return gt::gui::getIcon(QStringLiteral("convIcon_24.png"));
}

QIcon
gt::gui::icon::eye()
{
    return gt::gui::getIcon(QStringLiteral("eyeIcon.png"));
}

QIcon
gt::gui::icon::eye16()
{
    return gt::gui::getIcon(QStringLiteral("eyeIcon_16.png"));
}

QIcon
gt::gui::icon::eye24()
{
    return gt::gui::getIcon(QStringLiteral("eyeIcon_24.png"));
}

QIcon
gt::gui::icon::eyeOff()
{
    return gt::gui::getIcon(QStringLiteral("eyeOffIcon.png"));
}

QIcon
gt::gui::icon::eyeOff16()
{
    return gt::gui::getIcon(QStringLiteral("eyeOffIcon_16.png"));
}

QIcon
gt::gui::icon::eyeOff24()
{
    return gt::gui::getIcon(QStringLiteral("eyeOffIcon_24.png"));
}

QIcon
gt::gui::icon::stepFile()
{
    return gt::gui::getIcon(QStringLiteral("stepIcon.png"));
}

QIcon
gt::gui::icon::stepFile16()
{
    return gt::gui::getIcon(QStringLiteral("stepIcon_16.png"));
}

QIcon
gt::gui::icon::stepFile24()
{
    return gt::gui::getIcon(QStringLiteral("stepIcon_24.png"));
}

QIcon
gt::gui::icon::table()
{
    return gt::gui::getIcon(QStringLiteral("prpIcon.png"));
}

QIcon
gt::gui::icon::table16()
{
    return gt::gui::getIcon(QStringLiteral("prpIcon_16.png"));
}

QIcon
gt::gui::icon::python16()
{
    return gt::gui::getIcon(QStringLiteral("pythonIcon_16.png"));
}

QIcon
gt::gui::icon::performanceModel()
{
    return gt::gui::getIcon(QStringLiteral("performanceModel_own.png"));
}

QIcon
gt::gui::icon::reload()
{
    return gt::gui::getIcon(QStringLiteral("reloadIcon.png"));
}

QIcon
gt::gui::icon::ruler()
{
    return gt::gui::getIcon(QStringLiteral("rulerBlackIcon.png"));
}

QIcon
gt::gui::icon::textNote24()
{
    return gt::gui::getIcon(QStringLiteral("textNote_24.png"));
}

QIcon
gt::gui::icon::fileIn()
{
    return gt::gui::getIcon(QStringLiteral("fileinIcon.png"));
}

QIcon
gt::gui::icon::fileIn16()
{
    return gt::gui::getIcon(QStringLiteral("fileinIcon_16.png"));
}

QIcon
gt::gui::icon::fileIn24()
{
    return gt::gui::getIcon(QStringLiteral("fileinIcon_24.png"));
}

QIcon
gt::gui::icon::bezierCurve()
{
    return gt::gui::getIcon(QStringLiteral("bezierCurve.png"));
}

QIcon
gt::gui::icon::polyCurve()
{
    return gt::gui::getIcon(QStringLiteral("polyCurve.png"));
}

QIcon
gt::gui::icon::simpleCurve()
{
    return gt::gui::getIcon(QStringLiteral("simpleCurve.png"));
}

QIcon
gt::gui::icon::bleedInPort()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon.png"));
}

QIcon
gt::gui::icon::bleedInPort16()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon_16.png"));
}

QIcon
gt::gui::icon::bleedInPort24()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon_24.png"));
}

QIcon
gt::gui::icon::bleedOutPort()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon.png"));
}

QIcon
gt::gui::icon::bleedOutPort16()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon_16.png"));
}

QIcon
gt::gui::icon::bleedOutPort24()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon_24.png"));
}

QIcon
gt::gui::icon::bleedPortGroup()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon.png"));
}

QIcon
gt::gui::icon::bleedPortGroup16()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon_16.png"));
}

QIcon
gt::gui::icon::bleedPortGroup24()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon_24.png"));
}

QIcon
gt::gui::icon::engine()
{
    return gt::gui::getIcon(QStringLiteral("engineIcon.png"));
}

QIcon
gt::gui::icon::engine16()
{
    return gt::gui::getIcon(QStringLiteral("engineIcon_16.png"));
}

QIcon
gt::gui::icon::engineInstallation16()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_16.png"));
}

QIcon
gt::gui::icon::engineInstallation24()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_24.png"));
}

QIcon
gt::gui::icon::engineInstallation32()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_32.png"));
}

QIcon
gt::gui::icon::dataBaseArrowUp()
{
    return gt::gui::getIcon(QStringLiteral("database-arrow-up-icon_own.png"));
}

QIcon
gt::gui::icon::dataBaseArrowUp16()
{
    return gt::gui::getIcon(QStringLiteral("database-arrow-up-icon_own_16.png"));
}

QIcon
gt::gui::icon::dataBaseArrowUp24()
{
    return gt::gui::getIcon(QStringLiteral("database-arrow-up-icon_own_24.png"));
}

QIcon
gt::gui::icon::performanceDesign()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator.png"));
}

QIcon
gt::gui::icon::performanceDesign16()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator_16.png"));
}

QIcon
gt::gui::icon::performanceDesign24()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator_24.png"));
}

QIcon
gt::gui::icon::performanceOffDesign()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint.png"));
}

QIcon
gt::gui::icon::performanceOffDesign16()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint_16.png"));
}

QIcon
gt::gui::icon::performanceOffDesign24()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint_24.png"));
}

QIcon
gt::gui::icon::fluidPort()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon.png"));
}

QIcon
gt::gui::icon::fluidPort16()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon_16.png"));
}

QIcon
gt::gui::icon::fluidPort24()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon_24.png"));
}

QIcon
gt::gui::icon::fluidPortGroup()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon.png"));
}

QIcon
gt::gui::icon::fluidPortGroup16()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon_16.png"));
}

QIcon
gt::gui::icon::fluidPortGroup24()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon_24.png"));
}

QIcon
gt::gui::icon::shaftPort()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon.png"));
}

QIcon
gt::gui::icon::shaftPort16()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon_16.png"));
}

QIcon
gt::gui::icon::shaftPort24()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon_24.png"));
}

QIcon
gt::gui::icon::speedLine()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_32.png"));
}

QIcon
gt::gui::icon::speedLine16()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_16.png"));
}

QIcon
gt::gui::icon::speedLine24()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_24.png"));
}

QIcon
gt::gui::icon::brush()
{
    return gt::gui::getIcon(QStringLiteral("brushIcon.png"));
}

QIcon
gt::gui::icon::compressor()
{
    return gt::gui::getIcon(QStringLiteral("compressorIcon.png"));
}

QIcon
gt::gui::icon::compressor16()
{
    return gt::gui::getIcon(QStringLiteral("compressorIcon_16.png"));
}

QIcon
gt::gui::icon::turbine()
{
    return gt::gui::getIcon(QStringLiteral("turbineIcon.png"));
}

QIcon
gt::gui::icon::turbine16()
{
    return gt::gui::getIcon(QStringLiteral("turbineIcon_16.png"));
}

QIcon
gt::gui::icon::combustor()
{
    return gt::gui::getIcon(QStringLiteral("combustorIcon.png"));
}

QIcon
gt::gui::icon::combustor16()
{
    return gt::gui::getIcon(QStringLiteral("combustorIcon_16.png"));
}

QIcon
gt::gui::icon::paramStudy()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon.png"));
}

QIcon
gt::gui::icon::paramStudy16()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon_16.png"));
}

QIcon
gt::gui::icon::paramStudy24()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon_24.png"));
}

QString
gt::gui::pixmap::logoString()
{
    return {":/pixmaps/gt-logo.png"};
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

QPixmap
gt::gui::pixmap::question24()
{
    return QPixmap(":/icons/questionIcon_24.png");
}

QPixmap
gt::gui::pixmap::printHeadline()
{
    return {":/pixmaps/print-headline.png"};
}

QPixmap
gt::gui::pixmap::jumpTo()
{
    return QPixmap(QStringLiteral(":/icons/jumpToIcon.png"));
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

QPixmap
gt::gui::pixmap::logo()
{
    return {":/pixmaps/gt-logo.png"};
}

QPixmap
gt::gui::pixmap::downloaderLogo()
{
    return {QStringLiteral(":/pixmaps/downloaderLogo.png")};
}

QIcon
gt::gui::icon::letter::a()
{
    return gt::gui::getIcon(QStringLiteral("letters/aLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::aSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/aLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::b()
{
    return gt::gui::getIcon(QStringLiteral("letters/bLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::bSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/bLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::c()
{
    return gt::gui::getIcon(QStringLiteral("letters/cLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::cSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/cLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::d()
{
    return gt::gui::getIcon(QStringLiteral("letters/dLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::dSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/dLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::e()
{
    return gt::gui::getIcon(QStringLiteral("letters/eLetterIconBlue.png"));
}

QIcon
gt::gui::icon::letter::eSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/eLetterIconSmallBlue.png"));
}

QIcon
gt::gui::icon::letter::eBrown()
{
    return gt::gui::getIcon(QStringLiteral("letters/eLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::eBrownSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/eLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::f()
{
    return gt::gui::getIcon(QStringLiteral("letters/fLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::fSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/fLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::g()
{
    return gt::gui::getIcon(QStringLiteral("letters/gLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::gSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/gLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::h()
{
    return gt::gui::getIcon(QStringLiteral("letters/hLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::hSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/hLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::i()
{
    return gt::gui::getIcon(QStringLiteral("letters/iLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::iSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/iLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::j()
{
    return gt::gui::getIcon(QStringLiteral("letters/jLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::jSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/jLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::k()
{
    return gt::gui::getIcon(QStringLiteral("letters/kLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::kSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/kLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::l()
{
    return gt::gui::getIcon(QStringLiteral("letters/lLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::lSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/lLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::m()
{
    return gt::gui::getIcon(QStringLiteral("letters/mLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::mSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/mLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::n()
{
    return gt::gui::getIcon(QStringLiteral("letters/nLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::nSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/nLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::o()
{
    return gt::gui::getIcon(QStringLiteral("letters/oLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::oSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/oLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::p()
{
    return gt::gui::getIcon(QStringLiteral("letters/pLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::pSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/pLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::q()
{
    return gt::gui::getIcon(QStringLiteral("letters/qLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::qSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/qLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::r()
{
    return gt::gui::getIcon(QStringLiteral("letters/rLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::rSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/rLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::s()
{
    return gt::gui::getIcon(QStringLiteral("letters/sLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::sSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/sLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::t()
{
    return gt::gui::getIcon(QStringLiteral("letters/tLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::tSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/tLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::u()
{
    return gt::gui::getIcon(QStringLiteral("letters/uLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::uSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/uLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::v()
{
    return gt::gui::getIcon(QStringLiteral("letters/vLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::vSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/vLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::w()
{
    return gt::gui::getIcon(QStringLiteral("letters/wLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::wSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/wLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::x()
{
    return gt::gui::getIcon(QStringLiteral("letters/xLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::xSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/xLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::y()
{
    return gt::gui::getIcon(QStringLiteral("letters/yLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::ySmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/yLetterIconSmall.png"));
}

QIcon
gt::gui::icon::letter::z()
{
    return gt::gui::getIcon(QStringLiteral("letters/zLetterIcon.png"));
}

QIcon
gt::gui::icon::letter::zSmall()
{
    return gt::gui::getIcon(QStringLiteral("letters/zLetterIconSmall.png"));
}
