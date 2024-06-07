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
#include <QDesktopWidget>

/// helper macro for creating an icon:
/// Implements FUNCTION using
/// a static getter function named NAME to reduce code duplication.
/// Opens the resource at PATH
#define GT_ICON_IMPL(NAME, FUNCTION, PATH) \
    QIcon const& NAME##_impl() { \
        static QIcon ic = gt::gui::getIcon(QStringLiteral(PATH)); \
        return ic; \
    } \
    QIcon gt::gui::icon::FUNCTION() { \
        return NAME##_impl(); \
    }

/// same as above but for coloring icons
#define GT_ICON_IMPL_COLORED(NAME, FUNCTION, PATH, ACTIVE) \
    QIcon const& NAME##_impl() { \
        static QIcon ic = getSvgIconHelper(QStringLiteral(PATH), \
                                           {gt::gui::ACTIVE}); \
        return ic; \
    } \
    QIcon gt::gui::icon::FUNCTION() { \
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
    QIcon gt::gui::icon::FUNCTION() { \
        return TARGET(); \
    }

/// Defines a letter alias
#define GT_DEF_ICON_LETTER_ALIAS(FUNCTION, TARGET) \
    QIcon gt::gui::icon::letter::FUNCTION() { \
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

QIcon
getSvgIconHelper(QString const& iconPath, gt::gui::SvgColorData colorData = {})
{
    auto* iconEngine = new GtSvgIconEngine(resourcePath(iconPath),
                                           std::move(colorData));
    QIcon icon(iconEngine);
    if (icon.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return icon;
}

QIcon
getIconHelper(QString const& iconPath)
{
    if (iconPath.endsWith(QStringLiteral(".svg")))
    {
        return getSvgIconHelper(iconPath);
    }

    QIcon icon(resourcePath(iconPath));
    if (icon.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return icon;
}

QIcon
gt::gui::getIcon(QString const& iconPath)
{
    if (iconPath.endsWith(QStringLiteral(".svg")))
    {
        return getSvgIconHelper(iconPath);
    }

    QIcon ic(resourcePath(iconPath));
    if (ic.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found!").arg(iconPath);
    }
    return ic;
}

QIcon
gt::gui::colorize(const QIcon& icon,
                  ColorFunctionPtr getActiveColor,
                  ColorFunctionPtr getDisabledColor)
{
    gt::gui::SvgColorData colorData{
        getActiveColor,
        getDisabledColor
    };
    return getSvgIconHelper(icon.name(), std::move(colorData));
}

QIcon
gt::gui::colorize(const QIcon& icon, SvgColorData colorData)
{
    return getSvgIconHelper(icon.name(), std::move(colorData));
}

QSize
gt::gui::icon::standardSizeSmall()
{
    return QSize(16, 16) * qApp->desktop()->devicePixelRatio();
}

GT_DEF_ICON(add, "add.svg")

GT_DEF_ICON(application, "application.svg")
GT_DEF_ICON(applicationSettings, "applicationSettings.svg")
GT_DEF_ICON(applicationVar, "applicationVar.svg")

GT_DEF_ICON(arrowDown, "arrowDown.svg")
GT_DEF_ICON(arrowRight, "arrowRight_custom.svg")
GT_DEF_ICON(arrowLeft, "arrowLeft_custom.svg")
GT_DEF_ICON(arrowUp, "arrowUp.svg")

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

GT_DEF_ICON(cancel, "cancel.svg")

GT_DEF_ICON(chain, "chain.svg")
GT_DEF_ICON(chainOff, "chainOff.svg")

GT_DEF_ICON(chartBar, "chartBar.svg")
GT_DEF_ICON(chartBell, "chartBell.svg")
GT_DEF_ICON(chartCurve, "chartCurve.svg")
GT_DEF_ICON(chartLine, "chartLine.svg")
GT_DEF_ICON(chartScatter, "chartScatter.svg")
GT_DEF_ICON(chartXY, "chartXY_custom.svg")

GT_DEF_ICON(check, "check.svg")

GT_DEF_ICON(clear, "clear.svg")

GT_DEF_ICON(clone, "clone.svg")

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

GT_DEF_ICON(comment, "comment.svg")

GT_DEF_ICON(config, "config.svg")

GT_DEF_ICON(connection, "connection.svg")
GT_DEF_ICON_ALIAS(connection16, connection);

GT_DEF_ICON(convergence, "convergence_custom.svg")

GT_DEF_ICON(copy, "copy.svg")

GT_DEF_ICON(cross, "cross.svg")

GT_DEF_ICON(cursor, "cursor.svg")
GT_DEF_ICON(cursorMove, "cursorMove.svg")

GT_DEF_ICON(cut, "cut.svg")

GT_DEF_ICON(data, "data.svg")
GT_DEF_ICON(dataArrowUp, "dataArrowUp.svg")
GT_DEF_ICON(dataEye, "dataEye.svg")
GT_DEF_ICON(dataLock, "dataLock_custom.svg")
GT_DEF_ICON(dataSingle, "dataSingle_custom.svg")

GT_DEF_ICON(delete_, "delete.svg")

GT_DEF_ICON(disk, "disk_custom.svg")
GT_DEF_ICON(diskGradient, "diskGradient_custom.svg")

GT_DEF_ICON(devices, "devices.svg")

GT_DEF_ICON(dock, "dock.svg")

GT_DEF_ICON(dots, "dots.svg")

GT_DEF_ICON(download, "download.svg")

GT_DEF_ICON_ALIAS(duplicate, clone);

GT_DEF_ICON(error, "error.svg")
GT_DEF_ICON_COLORED(errorColorized, "error.svg", color::errorText)

GT_DEF_ICON_ALIAS(examples, bulb)

GT_DEF_ICON(exclamationmark, "exclamationmark.svg")

GT_DEF_ICON(export_, "export.svg")

GT_DEF_ICON(eye, "eye.svg")
GT_DEF_ICON(eyeOff, "eyeOff.svg")

GT_DEF_ICON(fatal, "fatal.svg")
GT_DEF_ICON_COLORED(fatalColorized, "fatal.svg", color::fatalText)

GT_DEF_ICON(file, "file.svg")
GT_DEF_ICON(fileAdd, "fileAdd.svg")
GT_DEF_ICON(fileClock, "fileClock.svg")
GT_DEF_ICON(fileCode, "fileCode.svg")
GT_DEF_ICON(fileDelete, "fileDelete.svg")
GT_DEF_ICON(fileDoc, "fileDoc.svg")
GT_DEF_ICON(fileEdit, "fileEdit.svg")
GT_DEF_ICON(fileEye, "fileEye.svg")
GT_DEF_ICON(fileImport, "fileImport_custom.svg")

GT_DEF_ICON(folder, "folder.svg")
GT_DEF_ICON(folderAdd, "folderAdd.svg")
GT_DEF_ICON(folderEdit, "folderEdit.svg")
GT_DEF_ICON(folderEye, "folderEye.svg")
GT_DEF_ICON(folderOpen, "folderOpen.svg")
GT_DEF_ICON(folderSearch, "folderSearch.svg")

GT_DEF_ICON(font, "font.svg")

GT_DEF_ICON(gas, "gas.svg")

GT_DEF_ICON_ALIAS(global, web)

GT_DEF_ICON(grid, "grid.svg")
GT_DEF_ICON(gridSnap, "gridSnap_custom.svg")

GT_DEF_ICON(help, "help.svg")

GT_DEF_ICON(hdf5, "hdf5_custom.svg")

GT_DEF_ICON(histogram, "histogram.svg")

GT_DEF_ICON(history, "history.svg")

GT_DEF_ICON(home, "home.svg")
GT_DEF_ICON(homeAccount, "homeAccount.svg")

GT_DEF_ICON(import, "import.svg")

GT_DEF_ICON(info, "info.svg")
GT_DEF_ICON_COLORED(infoColorized, "info.svg", color::infoText)
GT_DEF_ICON(info2, "info2.svg")

GT_DEF_ICON(input, "input.svg")
GT_DEF_ICON(inputForm, "inputForm.svg")
GT_DEF_ICON(inputSettings, "inputSettings.svg")

GT_DEF_ICON(jumpTo, "jumpTo_custom.svg")

GT_DEF_ICON(json, "json.svg")

GT_DEF_ICON(label, "label.svg")

GT_DEF_ICON(language, "language.svg")

GT_DEF_ICON(layers, "layers.svg")

GT_DEF_ICON(list, "list.svg")
GT_DEF_ICON(listFormatted, "listFormatted.svg")

GT_DEF_ICON_ALIAS(log, fileClock)

GT_DEF_ICON(login, "login.svg")

GT_DEF_ICON_ALIAS(loop, repeat)

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

GT_DEF_ICON(open, "open.svg")

GT_DEF_ICON(palette, "palette.svg")

GT_DEF_ICON(paramStudy, "paramStudy_custom.svg")

GT_DEF_ICON(paste, "paste.svg")

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

GT_DEF_ICON(play, "play.svg")

GT_DEF_ICON_ALIAS(plugin, puzzle)
GT_DEF_ICON_COLORED(pluginSettings, "puzzle.svg", color::highlight)

GT_DEF_ICON(print, "print.svg")

GT_DEF_ICON_ALIAS(process, cog)
GT_DEF_ICON_ALIAS(processAdd, cogPlus)
GT_DEF_ICON_ALIAS(processRun, play)

GT_DEF_ICON(projectAdd, "projectAdd.svg")
GT_DEF_ICON(projectClose, "projectClose.svg")
GT_DEF_ICON(projectClosed, "projectClosed.svg")
GT_DEF_ICON(projectDelete, "projectDelete.svg")
GT_DEF_ICON(projectImport, "projectImport_custom.svg")
GT_DEF_ICON(projectOpen, "projectOpen.svg")
GT_DEF_ICON(projectRemove, "projectRemove.svg")

GT_DEF_ICON_ALIAS(property, list)

GT_DEF_ICON_ALIAS(prp, table)

GT_DEF_ICON(puzzle, "puzzle.svg")

GT_DEF_ICON(python, "python.svg")

GT_DEF_ICON(questionmark, "questionmark.svg")

GT_DEF_ICON(queue, "queue.svg")
GT_DEF_ICON(queueAdd, "queueAdd.svg")

GT_DEF_ICON(redo, "redo.svg")

GT_DEF_ICON(reload, "reload.svg")

GT_DEF_ICON(remove, "remove.svg")

GT_DEF_ICON(rename, "rename.svg")

GT_DEF_ICON(repeat, "repeat.svg")

GT_DEF_ICON(resize, "resize.svg")

GT_DEF_ICON_ALIAS(results, table)

GT_DEF_ICON(revert, "revert.svg")

GT_DEF_ICON(ruler, "ruler.svg")

GT_DEF_ICON(save, "save.svg")

GT_DEF_ICON(schedules, "schedules_custom.svg")
GT_DEF_ICON(schedules2, "schedules2_custom.svg")

GT_DEF_ICON(search, "search.svg")

GT_DEF_ICON(select, "select.svg")

GT_DEF_ICON(server, "server.svg")

GT_DEF_ICON(session, "session.svg")

GT_DEF_ICON(showText, "showText_custom.svg")

GT_DEF_ICON(skip, "skip.svg")
GT_DEF_ICON(unskip, "unskip_custom.svg")

GT_DEF_ICON(sleep, "sleep.svg")
GT_DEF_ICON(sleepOff, "sleepOff_custom.svg")

GT_DEF_ICON_ALIAS(square, stop)

GT_DEF_ICON(sort, "sort.svg")
GT_DEF_ICON(sortAsc, "sortAsc.svg")
GT_DEF_ICON(sortDesc, "sortDesc.svg")

GT_DEF_ICON(stop, "stop.svg")

GT_DEF_ICON(stepFile, "stepFile_custom.svg")

GT_DEF_ICON(stretch, "stretch.svg")

GT_DEF_ICON(swap, "swap.svg")

GT_DEF_ICON(sync, "sync.svg")

GT_DEF_ICON(table, "table.svg")

GT_DEF_ICON(text, "text.svg")

GT_DEF_ICON(timer, "timer.svg")
GT_DEF_ICON(timerSand, "timerSand.svg")

GT_DEF_ICON(trace, "trace.svg")
GT_DEF_ICON_COLORED(traceColorized, "trace.svg", color::debugText)

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


GT_DEF_ICON(undo, "undo.svg")

GT_DEF_ICON(update, "update.svg")

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

GT_DEF_ICON(web, "web.svg")

GT_DEF_ICON(xml, "xml.svg")

GT_DEF_ICON(zoomMinus, "zoomMinus.svg")
GT_DEF_ICON(zoomPlus, "zoomPlus.svg")


GT_DEF_ICON(engineInstallation, "engineInstallation_custom.svg");

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
gt::gui::pixmap::background()
{
    return QPixmap(backgroundPath());
}

QPixmap
gt::gui::pixmap::logo()
{
    return QPixmap(logoString());
}
