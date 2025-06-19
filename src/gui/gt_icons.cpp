/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_icons.cpp
 *
 *  Created on: 04.04.2022
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_application.h"
#include "gt_logging.h"
#include "gt_svgiconengine.h"

#include <QPainter>
#include <QScreen>
#include <QApplication>

namespace
{

static bool initialized = [](){
    qRegisterMetaType<gt::gui::Icon>("gt::gui::Icon");

    QMetaType::registerConverter<gt::gui::Icon, QIcon>(
        [](const gt::gui::Icon& icon) { return static_cast<QIcon>(icon); });

    return true;
}();

}

/// helper macro for creating an icon:
/// Implements FUNCTION using
/// a static getter function named NAME to reduce code duplication.
/// Opens the resource at PATH
#define GT_ICON_IMPL(NAME, FUNCTION, PATH) \
    gt::gui::Icon const& NAME##_impl() { \
        static gt::gui::Icon ic = gt::gui::getIcon(resourcePath(QStringLiteral(PATH))); \
        return ic; \
    } \
    gt::gui::Icon gt::gui::icon::FUNCTION() { \
        return NAME##_impl(); \
    }

/// same as above but for coloring icons
#define GT_ICON_IMPL_COLORED(NAME, FUNCTION, PATH, ACTIVE) \
    gt::gui::Icon const& NAME##_impl() { \
        static gt::gui::Icon  ic = getSvgIconHelper(resourcePath(QStringLiteral(PATH)), \
                                           {gt::gui::ACTIVE}); \
        return ic; \
    } \
    gt::gui::Icon gt::gui::icon::FUNCTION() { \
        return NAME##_impl(); \
    }

/// Defines an icon implementation
#define GT_DEF_ICON(FUNCTION, PATH) \
    GT_ICON_IMPL(FUNCTION, FUNCTION, PATH)

#define GT_DEF_ICON_COLORED(FUNCTION, PATH, ACTIVE) \
    GT_ICON_IMPL_COLORED(FUNCTION, FUNCTION, PATH, ACTIVE)

/// Defines an letter icon implementation
#define GT_DEF_ICON_LETTER(FUNCTION, PATH) \
    GT_ICON_IMPL(letter_##FUNCTION, letter::FUNCTION, "letters/" PATH)

#define GT_DEF_ICON_LETTER_COLORED(FUNCTION, PATH, ACTIVE) \
    GT_ICON_IMPL_COLORED(letter_##FUNCTION, letter::FUNCTION, "letters/" PATH, ACTIVE)

/// Defines an icon alias
#define GT_DEF_ICON_ALIAS(FUNCTION, TARGET) \
    gt::gui::Icon gt::gui::icon::FUNCTION() { \
        return TARGET(); \
    }

/// Defines a letter alias
#define GT_DEF_ICON_LETTER_ALIAS(FUNCTION, TARGET) \
    gt::gui::Icon gt::gui::icon::letter::FUNCTION() { \
        return letter::TARGET(); \
    }

/// Defines  pixmap implementation
#define GT_DEF_PIXMAP(FUNCTION, PATH) \
    QPixmap gt::gui::pixmap::FUNCTION() { \
        return {QStringLiteral(":/pixmaps/" PATH)}; \
    }

QString
resourcePath(QString const& iconPath)
{
    if (!iconPath.startsWith(QStringLiteral(":/")))
    {
        return QStringLiteral(":/icons/") + iconPath;
    }
    return iconPath;
}

gt::gui::Icon
getSvgIconHelper(QString const& iconPath, gt::gui::SvgColorData colorData = {})
{
    auto* iconEngine = new GtSvgIconEngine(resourcePath(iconPath),
                                           std::move(colorData));
    gt::gui::Icon icon(iconEngine, iconPath);
    if (icon.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return icon;
}

gt::gui::Icon
getIconHelper(QString const& iconPath)
{
    if (iconPath.endsWith(QStringLiteral(".svg")))
    {
        return getSvgIconHelper(iconPath);
    }

    gt::gui::Icon icon(resourcePath(iconPath));
    if (icon.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return icon;
}

gt::gui::Icon
gt::gui::getIcon(QString const& iconPath)
{
    if (iconPath.endsWith(QStringLiteral(".svg")))
    {
        return getSvgIconHelper(iconPath);
    }

    Icon ic(resourcePath(iconPath));
    if (ic.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return ic;
}

QIcon
gt::gui::colorize(const gt::gui::Icon& icon,
                  ColorFunctionPtr getActiveColor,
                  ColorFunctionPtr getDisabledColor)
{
    gt::gui::SvgColorData colorData{
        getActiveColor,
        getDisabledColor
    };
    return getSvgIconHelper(icon.path(), std::move(colorData));
}

QIcon
gt::gui::colorize(const gt::gui::Icon& icon, SvgColorData colorData)
{
    return getSvgIconHelper(icon.path(), std::move(colorData));
}

QSize
gt::gui::icon::standardSizeSmall()
{
    return QSize(16, 16) * qApp->primaryScreen()->devicePixelRatio();
}

GT_DEF_ICON(add, "add.svg")
GT_DEF_ICON_ALIAS(add16, add)

GT_DEF_ICON_ALIAS(addProject, projectAdd)

GT_DEF_ICON(application, "application.svg")
GT_DEF_ICON(applicationSettings, "applicationSettings.svg")
GT_DEF_ICON(applicationVar, "applicationVar.svg")

GT_DEF_ICON(arrowDown, "arrowDown.svg")
GT_DEF_ICON_ALIAS(arrowDownBlue, uncollapsedColorized)
GT_DEF_ICON_ALIAS(arrowDownBlueAll, arrowDown)
GT_DEF_ICON(arrowRight, "arrowRight_custom.svg")
GT_DEF_ICON_ALIAS(arrowRightBlue, collapsedColorized)
GT_DEF_ICON(arrowLeft, "arrowLeft_custom.svg")
GT_DEF_ICON(arrowUp, "arrowUp.svg")
GT_DEF_ICON_ALIAS(arrowUpBlueAll, arrowUp)

GT_DEF_ICON(backspace, "backspace.svg")
GT_DEF_ICON(backspaceFlipped, "backspaceFlipped.svg")

GT_DEF_ICON(binaryTree, "binaryTree.svg")

GT_DEF_ICON(blades, "blades_custom.svg")

GT_DEF_ICON(brush, "brush.svg")

GT_DEF_ICON(bug, "bug.svg")
GT_DEF_ICON_COLORED(bugColorized, "bug.svg", color::debugText)
GT_DEF_ICON(bugPlay, "bugPlay.svg")

GT_DEF_ICON(bulb, "bulb.svg")

GT_DEF_ICON(calculator, "calculator.svg")
GT_DEF_ICON_ALIAS(calculator16, calculator)

GT_DEF_ICON(cancel, "cancel.svg")

GT_DEF_ICON(chain, "chain.svg")
GT_DEF_ICON_ALIAS(chain16, chain)
GT_DEF_ICON(chainOff, "chainOff.svg")

GT_DEF_ICON(chartBar, "chartBar.svg")
GT_DEF_ICON(chartBell, "chartBell.svg")
GT_DEF_ICON(chartCurve, "chartCurve.svg")
GT_DEF_ICON(chartLine, "chartLine.svg")
GT_DEF_ICON(chartScatter, "chartScatter.svg")
GT_DEF_ICON(chartXY, "chartXY_custom.svg")

GT_DEF_ICON(check, "check.svg")
GT_DEF_ICON_ALIAS(check16, check)
GT_DEF_ICON_ALIAS(check24, check)
GT_DEF_ICON_ALIAS(checkSmall16, check)

GT_DEF_ICON(clear, "clear.svg")
GT_DEF_ICON_ALIAS(clear16, clear);
GT_DEF_ICON_ALIAS(clear2_16, backspaceFlipped);

GT_DEF_ICON(clone, "clone.svg")
GT_DEF_ICON_ALIAS(clone16, clone);

GT_DEF_ICON_ALIAS(closeProject, projectClose);
GT_DEF_ICON_ALIAS(closeProject16, projectClose);
GT_DEF_ICON_ALIAS(closeProject24, projectClose);

GT_DEF_ICON_ALIAS(closedProject16, projectClosed);

GT_DEF_ICON_ALIAS(close, cross)

GT_DEF_ICON(cog, "cog.svg")
GT_DEF_ICON(cogPlus, "cogPlus_custom.svg")

GT_DEF_ICON(collapsed, "collapsed_custom.svg")
GT_DEF_ICON_COLORED(collapsedColorized, "collapsed_custom.svg",
                    color::highlight)
GT_DEF_ICON(uncollapsed, "uncollapsed_custom.svg")
GT_DEF_ICON_COLORED(uncollapsedColorized, "uncollapsed_custom.svg",
                    color::highlight)

GT_DEF_ICON(collection, "collection_custom.svg");
GT_DEF_ICON_ALIAS(collection16, collection);

GT_DEF_ICON(comment, "comment.svg")

GT_DEF_ICON(config, "config.svg")
GT_DEF_ICON_ALIAS(config16, config);

GT_DEF_ICON(connection, "connection.svg")
GT_DEF_ICON_ALIAS(connection16, connection);

GT_DEF_ICON(convergence, "convergence_custom.svg")
GT_DEF_ICON_ALIAS(convergence16, convergence);
GT_DEF_ICON_ALIAS(convergence24, convergence);

GT_DEF_ICON(copy, "copy.svg")
GT_DEF_ICON_ALIAS(copy16, copy);

GT_DEF_ICON(cross, "cross.svg")
GT_DEF_ICON_ALIAS(cross16, cross);

GT_DEF_ICON(cursor, "cursor.svg")
GT_DEF_ICON(cursorMove, "cursorMove.svg")

GT_DEF_ICON(cut, "cut.svg")
GT_DEF_ICON_ALIAS(cut16, cut);

GT_DEF_ICON(data, "data.svg")
GT_DEF_ICON_ALIAS(data16, data);
GT_DEF_ICON(dataArrowUp, "dataArrowUp.svg")
GT_DEF_ICON(dataEye, "dataEye.svg")
GT_DEF_ICON(dataLock, "dataLock_custom.svg")
GT_DEF_ICON(dataSingle, "dataSingle_custom.svg")

GT_DEF_ICON(delete_, "delete.svg")
GT_DEF_ICON_ALIAS(delete16, delete_);

GT_DEF_ICON(disk, "disk_custom.svg")
GT_DEF_ICON(diskGradient, "diskGradient_custom.svg")

GT_DEF_ICON_ALIAS(deleteProject16, projectDelete);

GT_DEF_ICON(devices, "devices.svg")

GT_DEF_ICON(dock, "dock.svg")

GT_DEF_ICON(dots, "dots.svg")

GT_DEF_ICON(download, "download.svg")

GT_DEF_ICON_ALIAS(duplicate, clone);

GT_DEF_ICON_ALIAS(empty16, objectEmpty);

GT_DEF_ICON(error, "error.svg")
GT_DEF_ICON_COLORED(errorColorized, "error.svg", color::errorText)
GT_DEF_ICON_ALIAS(error16, error);

GT_DEF_ICON_ALIAS(examples, bulb)
GT_DEF_ICON_ALIAS(examples16, examples)

GT_DEF_ICON(exclamationmark, "exclamationmark.svg")
GT_DEF_ICON_ALIAS(exclamation16, exclamationmark);

GT_DEF_ICON(export_, "export.svg")
GT_DEF_ICON_ALIAS(export16, export_);

GT_DEF_ICON(eye, "eye.svg")
GT_DEF_ICON_ALIAS(eye16, eye);
GT_DEF_ICON_ALIAS(eye24, eye);
GT_DEF_ICON(eyeOff, "eyeOff.svg")
GT_DEF_ICON_ALIAS(eyeOff16, eyeOff);
GT_DEF_ICON_ALIAS(eyeOff24, eyeOff);

GT_DEF_ICON(fatal, "fatal.svg")
GT_DEF_ICON_COLORED(fatalColorized, "fatal.svg", color::fatalText)
GT_DEF_ICON_ALIAS(fatal16, fatal);

GT_DEF_ICON(file, "file.svg")
GT_DEF_ICON_ALIAS(file16, file)
GT_DEF_ICON(fileAdd, "fileAdd.svg")
GT_DEF_ICON(fileClock, "fileClock.svg")
GT_DEF_ICON(fileCode, "fileCode.svg")
GT_DEF_ICON(fileDelete, "fileDelete.svg")
GT_DEF_ICON(fileDoc, "fileDoc.svg")
GT_DEF_ICON(fileEdit, "fileEdit.svg")
GT_DEF_ICON(fileEye, "fileEye.svg")
GT_DEF_ICON(fileImport, "fileImport_custom.svg")
GT_DEF_ICON_ALIAS(fileStep, stepFile)

GT_DEF_ICON(folder, "folder.svg")
GT_DEF_ICON_ALIAS(folder16, folder)
GT_DEF_ICON(folderAdd, "folderAdd.svg")
GT_DEF_ICON(folderEdit, "folderEdit.svg")
GT_DEF_ICON(folderEye, "folderEye.svg")
GT_DEF_ICON(folderOpen, "folderOpen.svg")
GT_DEF_ICON(folderSearch, "folderSearch.svg")

GT_DEF_ICON(font, "font.svg")

GT_DEF_ICON(gas, "gas.svg")
GT_DEF_ICON_ALIAS(gas16, gas)
GT_DEF_ICON_ALIAS(gas24, gas)
GT_DEF_ICON_ALIAS(gasTypes, gas)
GT_DEF_ICON_ALIAS(gasTypes16, gas)
GT_DEF_ICON_ALIAS(gasTypes24, gas)

GT_DEF_ICON_ALIAS(global, web)

GT_DEF_ICON(grid, "grid.svg")
GT_DEF_ICON(gridSnap, "gridSnap_custom.svg")

GT_DEF_ICON(help, "help.svg")

GT_DEF_ICON(hdf5, "hdf5_custom.svg")

GT_DEF_ICON(histogram, "histogram.svg")
GT_DEF_ICON(histogram16, "histogram16.svg")

GT_DEF_ICON(history, "history.svg")
GT_DEF_ICON(history16, "history16.svg")

GT_DEF_ICON(home, "home.svg")
GT_DEF_ICON(homeAccount, "homeAccount.svg")

GT_DEF_ICON(import, "import.svg")
GT_DEF_ICON_ALIAS(import16, import)

GT_DEF_ICON_ALIAS(inconsistentProject16, objectInvalid)

GT_DEF_ICON(info, "info.svg")
GT_DEF_ICON_COLORED(infoColorized, "info.svg", color::infoText)
GT_DEF_ICON(info2, "info2.svg")

GT_DEF_ICON(input, "input.svg")
GT_DEF_ICON(inputForm, "inputForm.svg")
GT_DEF_ICON_ALIAS(input16, rename)
GT_DEF_ICON_ALIAS(input2, inputSettings)
GT_DEF_ICON(inputSettings, "inputSettings.svg")

GT_DEF_ICON(jumpTo, "jumpTo_custom.svg")

GT_DEF_ICON_ALIAS(info16, info)
GT_DEF_ICON_ALIAS(infoBlue, info2)
GT_DEF_ICON_ALIAS(infoBlue16, info2)

GT_DEF_ICON(json, "json.svg")

GT_DEF_ICON(label, "label.svg")

GT_DEF_ICON(language, "language.svg")

GT_DEF_ICON(layers, "layers.svg")

GT_DEF_ICON(list, "list.svg")
GT_DEF_ICON_ALIAS(list16, list)
GT_DEF_ICON(listFormatted, "listFormatted.svg")

GT_DEF_ICON_ALIAS(log, fileClock)
GT_DEF_ICON_ALIAS(log16, log)

GT_DEF_ICON(login, "login.svg")
GT_DEF_ICON_ALIAS(login16, login)

GT_DEF_ICON_ALIAS(loop, repeat)
GT_DEF_ICON_ALIAS(loop16, loop)
GT_DEF_ICON_ALIAS(loop2_16, loop)

GT_DEF_ICON(magnet, "magnet.svg")

GT_DEF_ICON(mass, "mass.svg")

GT_DEF_ICON(mathDivision, "mathDivision.svg")
GT_DEF_ICON(mathExponent, "mathExponent.svg")
GT_DEF_ICON(mathMinus, "mathMinus.svg")
GT_DEF_ICON(mathMultiplication, "mathMultiplication.svg")
GT_DEF_ICON(mathPlus, "mathPlus.svg")
GT_DEF_ICON(mathRoot, "mathRoot.svg")

GT_DEF_ICON_ALIAS(minimize, mathMinus)

GT_DEF_ICON(network, "network.svg")
GT_DEF_ICON_ALIAS(network16, network)

GT_DEF_ICON_ALIAS(noteEdit, fileEdit)

GT_DEF_ICON(notificationUnread, "notificationUnread.svg")

GT_DEF_ICON(objectCombustor, "objectCombustor_custom.svg")
GT_DEF_ICON(objectCompressor, "objectCompressor_custom.svg")
GT_DEF_ICON(objectEmpty, "objectEmpty.svg")
GT_DEF_ICON(objectEngine, "objectEngine.svg")
GT_DEF_ICON(objectFreestyleComponent, "objectFreestyleComponent_custom.svg")
GT_DEF_ICON(objectInvalid, "objectInvalid.svg")
GT_DEF_ICON(objectUnknown, "objectUnknown.svg")
GT_DEF_ICON(objectTurbine, "objectTurbine_custom.svg")
GT_DEF_ICON_ALIAS(objectUnkown, objectUnknown)

GT_DEF_ICON(open, "open.svg")
GT_DEF_ICON_ALIAS(open16, open)
GT_DEF_ICON_ALIAS(open2, open)
GT_DEF_ICON_ALIAS(open2_16, open)
GT_DEF_ICON_ALIAS(open2_24, open)

GT_DEF_ICON_ALIAS(openProject16, projectOpen)

GT_DEF_ICON(palette, "palette.svg")

GT_DEF_ICON(paramStudy, "paramStudy_custom.svg")
GT_DEF_ICON_ALIAS(paramStudy16, paramStudy)
GT_DEF_ICON_ALIAS(paramStudy24, paramStudy)

GT_DEF_ICON(paste, "paste.svg")
GT_DEF_ICON_ALIAS(paste16, paste)

GT_DEF_ICON(pause, "pause.svg")

GT_DEF_ICON(pdf, "pdf_custom.svg")

GT_DEF_ICON(perfDesignPoint, "perfDesignPoint_custom.svg")
GT_DEF_ICON(perfMap, "perfMap_custom.svg")
GT_DEF_ICON(perfMapExport, "perfMapExport_custom.svg")
GT_DEF_ICON(perfNoMap, "perfNoMap_custom.svg")
GT_DEF_ICON(perfModel, "perfModel_custom.svg")
GT_DEF_ICON(perfOperatingPoint, "perfOperatingPoint_custom.svg")
GT_DEF_ICON(perfStageMap, "perfStageMap_custom.svg")
GT_DEF_ICON(perfSpeedLine, "perfSpeedLine_custom.svg")
GT_DEF_ICON(perfTsDiagram, "perfTsDiagram_custom.svg")
GT_DEF_ICON(perfWorkingLine, "perfWorkingLine_custom.svg")

GT_DEF_ICON_ALIAS(perspectives, devices)
GT_DEF_ICON(perspectivesEdit, "devicesEdit.svg")
GT_DEF_ICON_ALIAS(perspectives16, perspectives)

GT_DEF_ICON(play, "play.svg")

GT_DEF_ICON_ALIAS(plugin, puzzle)
GT_DEF_ICON_ALIAS(plugin16, plugin)
GT_DEF_ICON_COLORED(pluginSettings, "puzzle.svg", color::highlight)

GT_DEF_ICON(print, "print.svg")
GT_DEF_ICON_ALIAS(printer, print)

GT_DEF_ICON_ALIAS(process, cog)
GT_DEF_ICON_ALIAS(process16, process)
GT_DEF_ICON_ALIAS(processAdd, cogPlus)
GT_DEF_ICON_ALIAS(processRun, play)

GT_DEF_ICON_ALIAS(processFailed16, warning)

GT_DEF_ICON(projectAdd, "projectAdd.svg")
GT_DEF_ICON(projectClose, "projectClose.svg")
GT_DEF_ICON(projectClosed, "projectClosed.svg")
GT_DEF_ICON(projectDelete, "projectDelete.svg")
GT_DEF_ICON(projectImport, "projectImport_custom.svg")
GT_DEF_ICON(projectOpen, "projectOpen.svg")
GT_DEF_ICON(projectRemove, "projectRemove.svg")

GT_DEF_ICON_ALIAS(property, list)
GT_DEF_ICON_ALIAS(property16, property)

GT_DEF_ICON_ALIAS(prp, table)
GT_DEF_ICON_ALIAS(prp16, prp)

GT_DEF_ICON(puzzle, "puzzle.svg")

GT_DEF_ICON(python, "python.svg")
GT_DEF_ICON_ALIAS(python16, python)

GT_DEF_ICON(questionmark, "questionmark.svg")
GT_DEF_ICON_ALIAS(question16, questionmark)

GT_DEF_ICON(queue, "queue.svg")
GT_DEF_ICON_ALIAS(queue16, queue)
GT_DEF_ICON(queueAdd, "queueAdd.svg")

GT_DEF_ICON(redo, "redo.svg")
GT_DEF_ICON_ALIAS(redo24, redo)

GT_DEF_ICON(reload, "reload.svg")

GT_DEF_ICON(remove, "remove.svg")

GT_DEF_ICON(rename, "rename.svg")

GT_DEF_ICON(repeat, "repeat.svg")

GT_DEF_ICON(resize, "resize.svg")

GT_DEF_ICON_ALIAS(results, table)
GT_DEF_ICON_ALIAS(results16, results)

GT_DEF_ICON(revert, "revert.svg")
GT_DEF_ICON_ALIAS(restore16, revert)

GT_DEF_ICON(ruler, "ruler.svg")

GT_DEF_ICON_ALIAS(runProcess16, play)

GT_DEF_ICON(save, "save.svg")
GT_DEF_ICON_ALIAS(saveProject16, save)

GT_DEF_ICON(schedules, "schedules_custom.svg")
GT_DEF_ICON(schedules2, "schedules2_custom.svg")

GT_DEF_ICON(search, "search.svg")
GT_DEF_ICON_ALIAS(search16, search)

GT_DEF_ICON(select, "select.svg")

GT_DEF_ICON(server, "server.svg")

GT_DEF_ICON(session, "session.svg")
GT_DEF_ICON_ALIAS(session16, session)

GT_DEF_ICON(showText, "showText_custom.svg")
GT_DEF_ICON_ALIAS(showText16, showText)
GT_DEF_ICON_ALIAS(showText24, showText)

GT_DEF_ICON(skip, "skip.svg")
GT_DEF_ICON(unskip, "unskip_custom.svg")
GT_DEF_ICON_ALIAS(skip16, skip)

GT_DEF_ICON(sleep, "sleep.svg")
GT_DEF_ICON_ALIAS(sleep16, sleep)
GT_DEF_ICON(sleepOff, "sleepOff_custom.svg")

GT_DEF_ICON_ALIAS(square, stop)

GT_DEF_ICON(sort, "sort.svg")
GT_DEF_ICON(sortAsc, "sortAsc.svg")
GT_DEF_ICON(sortDesc, "sortDesc.svg")

GT_DEF_ICON(stop, "stop.svg")
GT_DEF_ICON_ALIAS(stop16, stop)

GT_DEF_ICON(stepFile, "stepFile_custom.svg")
GT_DEF_ICON_ALIAS(stepFile16, stepFile)
GT_DEF_ICON_ALIAS(stepFile24, stepFile)

GT_DEF_ICON(stretch, "stretch.svg")

GT_DEF_ICON(swap, "swap.svg")

GT_DEF_ICON(sync, "sync.svg")

GT_DEF_ICON(table, "table.svg")
GT_DEF_ICON_ALIAS(table16, table)

GT_DEF_ICON(text, "text.svg")

GT_DEF_ICON(timer, "timer.svg")
GT_DEF_ICON(timerSand, "timerSand.svg")

GT_DEF_ICON(trace, "trace.svg")
GT_DEF_ICON_COLORED(traceColorized, "trace.svg", color::debugText)
GT_DEF_ICON_ALIAS(trace16, trace)

GT_DEF_ICON(tree, "tree.svg")

GT_DEF_ICON(triangleDown, "triangleDown_custom.svg")
GT_DEF_ICON(triangleLeft, "triangleLeft_custom.svg")
GT_DEF_ICON(triangleRight, "triangleRight_custom.svg")
GT_DEF_ICON(triangleRightGroup, "triangleRightGroup_custom.svg")
GT_DEF_ICON(triangleUp, "triangleUp_custom.svg")
GT_DEF_ICON(triangleSmallDown, "triangleSmallDown.svg")
GT_DEF_ICON(triangleSmallLeft, "triangleSmallLeft.svg")
GT_DEF_ICON(triangleSmallRight, "triangleSmallRight.svg")
GT_DEF_ICON(triangleSmallUp, "triangleSmallUp.svg")

GT_DEF_ICON_ALIAS(tsDiagram16, perfTsDiagram)

GT_DEF_ICON_ALIAS(turbine, objectTurbine)
GT_DEF_ICON_ALIAS(turbine16, objectTurbine)

GT_DEF_ICON(undo, "undo.svg")
GT_DEF_ICON_ALIAS(undo24, undo)

GT_DEF_ICON(update, "update.svg")
GT_DEF_ICON_ALIAS(update16, update)

GT_DEF_ICON(upload, "upload.svg")

GT_DEF_ICON(variable, "variable.svg")

GT_DEF_ICON(vectorBezier, "vectorBezier.svg")
GT_DEF_ICON(vectorBezier2, "vectorBezier2_custom.svg")
GT_DEF_ICON(vectorCurve, "vectorCurve.svg")
GT_DEF_ICON(vectorLine, "vectorLine.svg")
GT_DEF_ICON(vectorPoint, "vectorPoint.svg")
GT_DEF_ICON(vectorPointEdit, "vectorPointEdit.svg")
GT_DEF_ICON(vectorPoly, "vectorPoly_custom.svg")
GT_DEF_ICON(vectorSimple, "vectorSimple_custom.svg")
GT_DEF_ICON(vectorSquare, "vectorSquare.svg")
GT_DEF_ICON(vectorSquareEdit, "vectorSquareEdit.svg")
GT_DEF_ICON(vectorTriangle, "vectorTriangle.svg")

GT_DEF_ICON(warning, "warning.svg")
GT_DEF_ICON_COLORED(warningColorized, "warning.svg", color::warningText)
GT_DEF_ICON_ALIAS(warning16, warning)

GT_DEF_ICON(web, "web.svg")

GT_DEF_ICON(xml, "xml.svg")

GT_DEF_ICON(zoomMinus, "zoomMinus.svg")
GT_DEF_ICON(zoomPlus, "zoomPlus.svg")
GT_DEF_ICON_ALIAS(zoom16, zoomPlus)



GT_DEF_ICON_ALIAS(emptyElement, objectEmpty)

GT_DEF_ICON_ALIAS(inProgress16, timerSand)

GT_DEF_ICON_ALIAS(stopRequest16, stop)

GT_DEF_ICON_ALIAS(pathSettings, applicationVar)

GT_DEF_ICON_ALIAS(frame, layers)

GT_DEF_ICON_ALIAS(unknown, objectUnknown)

GT_DEF_ICON_ALIAS(close24, cross)

GT_DEF_ICON_ALIAS(components16, projectOpen)

GT_DEF_ICON_ALIAS(post, chartLine)

GT_DEF_ICON_ALIAS(labels, label)

GT_DEF_ICON_ALIAS(stack, layers)

GT_DEF_ICON_ALIAS(printPDF, pdf)

GT_DEF_ICON_ALIAS(upgradeProjectData, update)

GT_DEF_ICON_ALIAS(move, cursorMove)
GT_DEF_ICON_ALIAS(move16, cursorMove)
GT_DEF_ICON_ALIAS(move24, cursorMove)

GT_DEF_ICON_ALIAS(plus, mathPlus)
GT_DEF_ICON_ALIAS(minus, mathMinus)
GT_DEF_ICON_ALIAS(multiply, mathMultiplication)
GT_DEF_ICON_ALIAS(divide, mathDivision)
GT_DEF_ICON_ALIAS(squareRoot, mathRoot)

GT_DEF_ICON_ALIAS(fileIn, fileImport)
GT_DEF_ICON_ALIAS(fileIn16, fileImport)
GT_DEF_ICON_ALIAS(fileIn24, fileImport)

GT_DEF_ICON_ALIAS(textNote24, text)

GT_DEF_ICON_ALIAS(polyCurve, vectorTriangle)
GT_DEF_ICON_ALIAS(bezierCurve, vectorBezier)
GT_DEF_ICON_ALIAS(simpleCurve, vectorCurve)

GT_DEF_ICON_ALIAS(dataBaseArrowUp, dataArrowUp)
GT_DEF_ICON_ALIAS(dataBaseArrowUp16, dataArrowUp)
GT_DEF_ICON_ALIAS(dataBaseArrowUp24, dataArrowUp)

GT_DEF_ICON_ALIAS(editPen, rename)

GT_DEF_ICON_ALIAS(addProcess16, processAdd)
GT_DEF_ICON_ALIAS(addElement16, processAdd)

GT_DEF_ICON_ALIAS(dropdown, download)

GT_DEF_ICON_ALIAS(globals16, dataSingle)

GT_DEF_ICON_ALIAS(xyPlot16, chartXY)

GT_DEF_ICON_ALIAS(engine, objectEngine)
GT_DEF_ICON_ALIAS(engine16, objectEngine)

GT_DEF_ICON_ALIAS(combustor, objectCombustor)
GT_DEF_ICON_ALIAS(combustor16, objectCombustor)

GT_DEF_ICON_ALIAS(compressor, objectCompressor)
GT_DEF_ICON_ALIAS(compressor16, objectCompressor)

GT_DEF_ICON_ALIAS(speedLine, perfSpeedLine)
GT_DEF_ICON_ALIAS(speedLine16, perfSpeedLine)
GT_DEF_ICON_ALIAS(speedLine24, perfSpeedLine)

GT_DEF_ICON_ALIAS(performanceOffDesign, perfOperatingPoint)
GT_DEF_ICON_ALIAS(performanceOffDesign16, perfOperatingPoint)
GT_DEF_ICON_ALIAS(performanceOffDesign24, perfOperatingPoint)

GT_DEF_ICON_ALIAS(map, perfMap)
GT_DEF_ICON_ALIAS(map16, perfMap)
GT_DEF_ICON_ALIAS(map24, perfMap)

GT_DEF_ICON_ALIAS(noMap, perfNoMap)

GT_DEF_ICON_ALIAS(exportMap, perfMapExport)
GT_DEF_ICON_ALIAS(exportMap16, perfMapExport)
GT_DEF_ICON_ALIAS(exportMap24, perfMapExport)

GT_DEF_ICON_ALIAS(double16, letter::d)
GT_DEF_ICON_ALIAS(int16, letter::i)
GT_DEF_ICON_ALIAS(o16, letter::o)
GT_DEF_ICON_ALIAS(string16, letter::s)

GT_DEF_ICON_ALIAS(performanceModel, perfModel);

GT_DEF_ICON_ALIAS(performanceDesign, perfDesignPoint);
GT_DEF_ICON_ALIAS(performanceDesign16, perfDesignPoint);
GT_DEF_ICON_ALIAS(performanceDesign24, perfDesignPoint);

GT_DEF_ICON(engineInstallation, "engineInstallation_custom.svg");
GT_DEF_ICON_ALIAS(engineInstallation16, engineInstallation);
GT_DEF_ICON_ALIAS(engineInstallation24, engineInstallation);
GT_DEF_ICON_ALIAS(engineInstallation32, engineInstallation);

GT_DEF_ICON(carpetPlot, "carpetPlot_custom.svg");

GT_DEF_ICON_ALIAS(bleedInPort, triangleDown);
GT_DEF_ICON_ALIAS(bleedInPort16, bleedInPort);
GT_DEF_ICON_ALIAS(bleedInPort24, bleedInPort);

GT_DEF_ICON_ALIAS(bleedOutPort, triangleUp);
GT_DEF_ICON_ALIAS(bleedOutPort16, bleedOutPort);
GT_DEF_ICON_ALIAS(bleedOutPort24, bleedOutPort);

GT_DEF_ICON_ALIAS(bleedPortGroup, triangleRightGroup);
GT_DEF_ICON_ALIAS(bleedPortGroup16, bleedPortGroup);
GT_DEF_ICON_ALIAS(bleedPortGroup24, bleedPortGroup);


GT_DEF_ICON_ALIAS(fluidPort, triangleRight);
GT_DEF_ICON_ALIAS(fluidPort16, fluidPort);
GT_DEF_ICON_ALIAS(fluidPort24, fluidPort);

GT_DEF_ICON_ALIAS(fluidPortGroup, triangleRightGroup);
GT_DEF_ICON_ALIAS(fluidPortGroup16, fluidPortGroup);
GT_DEF_ICON_ALIAS(fluidPortGroup24, fluidPortGroup);

GT_DEF_ICON_ALIAS(shaftPort, square);
GT_DEF_ICON_ALIAS(shaftPort16, shaftPort);
GT_DEF_ICON_ALIAS(shaftPort24, shaftPort);

GT_DEF_ICON_LETTER(a, "a_capital.svg")
GT_DEF_ICON_LETTER(aSmall, "a_lower.svg")
GT_DEF_ICON_LETTER(b, "b_capital.svg")
GT_DEF_ICON_LETTER(bSmall, "b_lower.svg")
GT_DEF_ICON_LETTER(c, "c_capital.svg")
GT_DEF_ICON_LETTER(cSmall, "c_lower.svg")
GT_DEF_ICON_LETTER(d, "d_capital.svg")
GT_DEF_ICON_LETTER(dSmall, "d_lower.svg")
GT_DEF_ICON_LETTER(e, "e_capital.svg")
GT_DEF_ICON_LETTER(eSmall, "e_lower.svg")
GT_DEF_ICON_LETTER_COLORED(eColorized, "e_capital.svg", color::highlight)
GT_DEF_ICON_LETTER_COLORED(eSmallColorized, "e_lower.svg", color::highlight)
GT_DEF_ICON_LETTER_ALIAS(eBrown, eColorized)
GT_DEF_ICON_LETTER_ALIAS(eBrownSmall, eSmallColorized)
GT_DEF_ICON_LETTER(f, "f_capital.svg")
GT_DEF_ICON_LETTER(fSmall, "f_lower.svg")
GT_DEF_ICON_LETTER(g, "g_capital.svg")
GT_DEF_ICON_LETTER(gSmall, "g_lower.svg")
GT_DEF_ICON_LETTER(h, "h_capital.svg")
GT_DEF_ICON_LETTER(hSmall, "h_lower.svg")
GT_DEF_ICON_LETTER(i, "i_capital.svg")
GT_DEF_ICON_LETTER(iSmall, "i_lower.svg")
GT_DEF_ICON_LETTER(j, "j_capital.svg")
GT_DEF_ICON_LETTER(jSmall, "j_lower.svg")
GT_DEF_ICON_LETTER(k, "k_capital.svg")
GT_DEF_ICON_LETTER(kSmall, "k_lower.svg")
GT_DEF_ICON_LETTER(l, "l_capital.svg")
GT_DEF_ICON_LETTER(lSmall, "l_lower.svg")
GT_DEF_ICON_LETTER(m, "m_capital.svg")
GT_DEF_ICON_LETTER(mSmall, "m_lower.svg")
GT_DEF_ICON_LETTER(n, "n_capital.svg")
GT_DEF_ICON_LETTER(nSmall, "n_lower.svg")
GT_DEF_ICON_LETTER(o, "o_capital.svg")
GT_DEF_ICON_LETTER(oSmall, "o_lower.svg")
GT_DEF_ICON_LETTER(p, "p_capital.svg")
GT_DEF_ICON_LETTER(pSmall, "p_lower.svg")
GT_DEF_ICON_LETTER(q, "q_capital.svg")
GT_DEF_ICON_LETTER(qSmall, "q_lower.svg")
GT_DEF_ICON_LETTER(r, "r_capital.svg")
GT_DEF_ICON_LETTER(rSmall, "r_lower.svg")
GT_DEF_ICON_LETTER(s, "s_capital.svg")
GT_DEF_ICON_LETTER(sSmall, "s_lower.svg")
GT_DEF_ICON_LETTER(t, "t_capital.svg")
GT_DEF_ICON_LETTER(tSmall, "t_lower.svg")
GT_DEF_ICON_LETTER(u, "u_capital.svg")
GT_DEF_ICON_LETTER(uSmall, "u_lower.svg")
GT_DEF_ICON_LETTER(v, "v_capital.svg")
GT_DEF_ICON_LETTER(vSmall, "v_lower.svg")
GT_DEF_ICON_LETTER(w, "w_capital.svg")
GT_DEF_ICON_LETTER(wSmall, "w_lower.svg")
GT_DEF_ICON_LETTER(x, "x_capital.svg")
GT_DEF_ICON_LETTER(xSmall, "x_lower.svg")
GT_DEF_ICON_LETTER(y, "y_capital.svg")
GT_DEF_ICON_LETTER(ySmall, "y_lower.svg")
GT_DEF_ICON_LETTER(z, "z_capital.svg")
GT_DEF_ICON_LETTER(zSmall, "z_lower.svg")


QIcon
gt::gui::icon::processRunningIcon(int progress)
{
    if (progress == 0)
    {
        return gt::gui::icon::timerSand();
    }

    QRgb progressDone = qRgb(0, 255, 0);
    QRgb progressToDo = qRgb(255, 255, 255);

    int w = 100;
    int h = 100;
    QImage img(w, h, QImage::Format_Indexed8);

    img.setColorCount(2);
    img.setColor(0, progressDone);
    img.setColor(1, progressToDo);
    img.fill(1);

    double currentFillWidth = 1.0 * w * progress / 100.0;

    for (int i = 0; i < currentFillWidth; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            img.setPixel(i, j, 0);
        }
    }

    /// Note: as the current overlay does not improve the design it is
    /// removed but the code should remain to enable the usage of another
    /// overly easily
    QIcon retValIcon;
    QPixmap comboPixmap(100, 100);
    //QPixmap overlayImage(gt::gui::icon::inProgress16().pixmap(16, 16));
    QPixmap backGroundImage;
    backGroundImage.convertFromImage(img, Qt::ColorOnly);

    QPainter painter(&comboPixmap);
    painter.drawPixmap(0, 0, backGroundImage);
    //painter.drawPixmap(0, 0, overlayImage);

    retValIcon.addPixmap(comboPixmap);

    return retValIcon;
}

QString
gt::gui::pixmap::backgroundPath()
{
    if (gtApp->inDarkMode())
    {
        return QStringLiteral(":/pixmaps/startup-background_dark.png");
    }

    return QStringLiteral(":/pixmaps/startup-background.png");
}

QString
gt::gui::pixmap::logoString()
{
    if (gtApp->inDarkMode())
    {
        return QStringLiteral(":/pixmaps/gt-logo-dark.png");
    }
    return QStringLiteral(":/pixmaps/gt-logo.png");
}

GT_DEF_PIXMAP(splash, "splash.png");

GT_DEF_PIXMAP(accessLogo, "accessLogo.png");

GT_DEF_PIXMAP(updateLogo, "updateLogo.png");

GT_DEF_PIXMAP(changeLogLogo, "changelogLogo.png");

GT_DEF_PIXMAP(printHeadline, "print-headline.png");

GT_DEF_PIXMAP(downloaderLogo, "print-downloaderLogo.png");

QPixmap
gt::gui::pixmap::question24()
{
    return icon::questionmark().pixmap(24, 24);
}

QPixmap
gt::gui::pixmap::jumpTo()
{
    return icon::jumpTo().pixmap(32, 32);
}

QPixmap
gt::gui::pixmap::background()
{
    return QPixmap(backgroundPath());
}

QPixmap
gt::gui::pixmap::logo()
{
    return QPixmap(logoString());
}
