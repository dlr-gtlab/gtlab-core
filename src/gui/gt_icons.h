/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_icons.h
 *
 *  Created on: 04.04.2022
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTICONS_H
#define GTICONS_H

#include "gt_gui_exports.h"
#include "gt_svgiconengine.h"

#include <QIcon>

namespace gt
{
namespace gui
{

/**
 *  @brief Creates a new icon from a resource file.
 *  Svg files (ending with .svg) will be colored according to the theme used.
 *  @param iconPath Relative or custom resource path to a icon file.
 */
GT_GUI_EXPORT QIcon getIcon(const QString& iconPath);

/**
 *  @brief Creates a new icon from an exisiting icon and colorizes it.
 *  Only works for svg based icons.
 *
 *  Usage:
 *      // OR by function ptr
 *      gt::gui::colorize(gt::gui::icon::add(),
 *                       gt::gui::color::highlight);
 *      // OR by functor
 *      gt::gui::colorize(gt::gui::icon::add(),
 *                       []() -> QColor {
 *          return gtApp->inDarkMode() ? Qt::red : Qt::blue;
 *      });
 *      // OR by color
 *      gt::gui::colorize(gt::gui::icon::add(), Qt::red);
 *
 *
 *  @param icon Icon to colorize.
 *  @param colorData Color data for the icon modes.
 * @return Icon
 */
GT_GUI_EXPORT QIcon colorize(const QIcon& icon, SvgColorData colorData);

/**
 *  @brief Overload. Accepts two function pointers for the active and
 *  disabled color.
 *  NOTE: Prefere using SvgColorData
 *  @param icon Icon to colorize.
 *  @param getActiveColor Color function pointer for an active/normal icon.
 *  @param getDisabledColor Color function pointer for a disabled icon.
 * @return Icon
 */
GT_GUI_EXPORT QIcon colorize(const QIcon& icon,
                             ColorFunctionPtr getActiveColor,
                             ColorFunctionPtr getDisabledColor = {});

namespace icon
{

/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

GT_GUI_EXPORT QIcon add();

GT_GUI_EXPORT QIcon application();
GT_GUI_EXPORT QIcon applicationSettings();
GT_GUI_EXPORT QIcon applicationVar();

GT_GUI_EXPORT QIcon arrowDown();
GT_GUI_EXPORT QIcon arrowUp();

GT_GUI_EXPORT QIcon arrowRight();
GT_GUI_EXPORT QIcon arrowLeft();

GT_GUI_EXPORT QIcon backspace();
GT_GUI_EXPORT QIcon backspaceFlipped();

GT_GUI_EXPORT QIcon binaryTree();

GT_GUI_EXPORT QIcon blades();

GT_GUI_EXPORT QIcon brush();

GT_GUI_EXPORT QIcon bug();
GT_GUI_EXPORT QIcon bugColorized();
GT_GUI_EXPORT QIcon bugPlay();

GT_GUI_EXPORT QIcon bulb();

GT_GUI_EXPORT QIcon calculator();

GT_GUI_EXPORT QIcon cancel();

GT_GUI_EXPORT QIcon chain();
GT_GUI_EXPORT QIcon chainOff();

GT_GUI_EXPORT QIcon chartBar();
GT_GUI_EXPORT QIcon chartBell();
GT_GUI_EXPORT QIcon chartCurve();
GT_GUI_EXPORT QIcon chartLine();
GT_GUI_EXPORT QIcon chartScatter();
GT_GUI_EXPORT QIcon chartXY();

GT_GUI_EXPORT QIcon check();
GT_GUI_EXPORT QIcon clear();
GT_GUI_EXPORT QIcon clone();
GT_GUI_EXPORT QIcon close();

GT_GUI_EXPORT QIcon cog();
GT_GUI_EXPORT QIcon cogPlus();

GT_GUI_EXPORT QIcon collapsed();
GT_GUI_EXPORT QIcon collapsedColorized();
GT_GUI_EXPORT QIcon uncollapsed();
GT_GUI_EXPORT QIcon uncollapsedColorized();

GT_GUI_EXPORT QIcon collection();

GT_GUI_EXPORT QIcon comment();

GT_GUI_EXPORT QIcon config();

GT_GUI_EXPORT QIcon connection();
GT_GUI_EXPORT QIcon connection16();

GT_GUI_EXPORT QIcon convergence();

GT_GUI_EXPORT QIcon copy();

GT_GUI_EXPORT QIcon cross();

GT_GUI_EXPORT QIcon cursor();
GT_GUI_EXPORT QIcon cursorMove();

GT_GUI_EXPORT QIcon cut();

GT_GUI_EXPORT QIcon data();
GT_GUI_EXPORT QIcon dataArrowUp();
GT_GUI_EXPORT QIcon dataEye();
GT_GUI_EXPORT QIcon dataLock();
GT_GUI_EXPORT QIcon dataSingle();

GT_GUI_EXPORT QIcon delete_();

GT_GUI_EXPORT QIcon disk();
GT_GUI_EXPORT QIcon diskGradient();

GT_GUI_EXPORT QIcon devices();

GT_GUI_EXPORT QIcon dock();

GT_GUI_EXPORT QIcon dots();

GT_GUI_EXPORT QIcon download();

GT_GUI_EXPORT QIcon duplicate();

GT_GUI_EXPORT QIcon error();
GT_GUI_EXPORT QIcon errorColorized();

GT_GUI_EXPORT QIcon examples();

GT_GUI_EXPORT QIcon exclamationmark();

GT_GUI_EXPORT QIcon export_();

GT_GUI_EXPORT QIcon eye();
GT_GUI_EXPORT QIcon eyeOff();

GT_GUI_EXPORT QIcon fatal();
GT_GUI_EXPORT QIcon fatalColorized();


GT_GUI_EXPORT QIcon file();
GT_GUI_EXPORT QIcon fileAdd();
GT_GUI_EXPORT QIcon fileClock();
GT_GUI_EXPORT QIcon fileCode();
GT_GUI_EXPORT QIcon fileDelete();
GT_GUI_EXPORT QIcon fileDoc();
GT_GUI_EXPORT QIcon fileEdit();
GT_GUI_EXPORT QIcon fileEye();
GT_GUI_EXPORT QIcon fileImport();

GT_GUI_EXPORT QIcon folder();
GT_GUI_EXPORT QIcon folderAdd();
GT_GUI_EXPORT QIcon folderEdit();
GT_GUI_EXPORT QIcon folderEye();
GT_GUI_EXPORT QIcon folderOpen();
GT_GUI_EXPORT QIcon folderSearch();

GT_GUI_EXPORT QIcon font();

GT_GUI_EXPORT QIcon gas();

GT_GUI_EXPORT QIcon global();

GT_GUI_EXPORT QIcon grid();
GT_GUI_EXPORT QIcon gridSnap();

GT_GUI_EXPORT QIcon help();
GT_GUI_EXPORT QIcon hdf5();
GT_GUI_EXPORT QIcon histogram();
GT_GUI_EXPORT QIcon history();

GT_GUI_EXPORT QIcon home();
GT_GUI_EXPORT QIcon homeAccount();

GT_GUI_EXPORT QIcon import();

GT_GUI_EXPORT QIcon info();
GT_GUI_EXPORT QIcon infoColorized();
GT_GUI_EXPORT QIcon info2();

GT_GUI_EXPORT QIcon input();
GT_GUI_EXPORT QIcon inputForm();
GT_GUI_EXPORT QIcon inputSettings();

GT_GUI_EXPORT QIcon jumpTo();

GT_GUI_EXPORT QIcon json();

GT_GUI_EXPORT QIcon label();

GT_GUI_EXPORT QIcon language();

GT_GUI_EXPORT QIcon layers();

GT_GUI_EXPORT QIcon list();
GT_GUI_EXPORT QIcon listFormatted();

GT_GUI_EXPORT QIcon log();
GT_GUI_EXPORT QIcon login();

GT_GUI_EXPORT QIcon loop();
GT_GUI_EXPORT QIcon magnet();

GT_GUI_EXPORT QIcon mass();

GT_GUI_EXPORT QIcon mathDivision();
GT_GUI_EXPORT QIcon mathExponent();
GT_GUI_EXPORT QIcon mathMinus();
GT_GUI_EXPORT QIcon mathMultiplication();
GT_GUI_EXPORT QIcon mathPlus();
GT_GUI_EXPORT QIcon mathRoot();

GT_GUI_EXPORT QIcon minimize();

GT_GUI_EXPORT QIcon network();

GT_GUI_EXPORT QIcon noteEdit();

GT_GUI_EXPORT QIcon notificationUnread();

GT_GUI_EXPORT QIcon objectCombustor();
GT_GUI_EXPORT QIcon objectEmpty();
GT_GUI_EXPORT QIcon objectEngine();
GT_GUI_EXPORT QIcon objectFreestyleComponent();
GT_GUI_EXPORT QIcon objectInvalid();
GT_GUI_EXPORT QIcon objectUnknown();
GT_GUI_EXPORT QIcon objectTurbine();
GT_GUI_EXPORT QIcon objectCompressor();

GT_GUI_EXPORT QIcon open();

GT_GUI_EXPORT QIcon palette();

GT_GUI_EXPORT QIcon paramStudy();

GT_GUI_EXPORT QIcon paste();

GT_GUI_EXPORT QIcon pause();

GT_GUI_EXPORT QIcon pdf();

GT_GUI_EXPORT QIcon perfDesignPoint();
GT_GUI_EXPORT QIcon perfMap();
GT_GUI_EXPORT QIcon perfMapExport();
GT_GUI_EXPORT QIcon perfNoMap();
GT_GUI_EXPORT QIcon perfModel();
GT_GUI_EXPORT QIcon perfOperatingPoint();
GT_GUI_EXPORT QIcon perfStageMap();
GT_GUI_EXPORT QIcon perfSpeedLine();
GT_GUI_EXPORT QIcon perfTsDiagram();
GT_GUI_EXPORT QIcon perfWorkingLine();

GT_GUI_EXPORT QIcon perspectives();
GT_GUI_EXPORT QIcon perspectivesEdit();

GT_GUI_EXPORT QIcon play();

GT_GUI_EXPORT QIcon plugin();
GT_GUI_EXPORT QIcon pluginSettings();

GT_GUI_EXPORT QIcon print();
GT_GUI_EXPORT QIcon process();
GT_GUI_EXPORT QIcon processAdd();
GT_GUI_EXPORT QIcon processRun();

GT_GUI_EXPORT QIcon projectAdd();
GT_GUI_EXPORT QIcon projectClose();
GT_GUI_EXPORT QIcon projectClosed();
GT_GUI_EXPORT QIcon projectDelete();
GT_GUI_EXPORT QIcon projectImport();
GT_GUI_EXPORT QIcon projectOpen();
GT_GUI_EXPORT QIcon projectRemove();

GT_GUI_EXPORT QIcon property();
GT_GUI_EXPORT QIcon prp();

GT_GUI_EXPORT QIcon puzzle();

GT_GUI_EXPORT QIcon python();

GT_GUI_EXPORT QIcon questionmark();

GT_GUI_EXPORT QIcon queue();
GT_GUI_EXPORT QIcon queueAdd();

GT_GUI_EXPORT QIcon redo();

GT_GUI_EXPORT QIcon reload();

GT_GUI_EXPORT QIcon remove();

GT_GUI_EXPORT QIcon rename();

GT_GUI_EXPORT QIcon repeat();

GT_GUI_EXPORT QIcon resize();

GT_GUI_EXPORT QIcon results();

GT_GUI_EXPORT QIcon revert();

GT_GUI_EXPORT QIcon ruler();

GT_GUI_EXPORT QIcon save();

GT_GUI_EXPORT QIcon schedules();
GT_GUI_EXPORT QIcon schedules2();

GT_GUI_EXPORT QIcon search();

GT_GUI_EXPORT QIcon select();

GT_GUI_EXPORT QIcon server();

GT_GUI_EXPORT QIcon session();

GT_GUI_EXPORT QIcon showText();

GT_GUI_EXPORT QIcon skip();
GT_GUI_EXPORT QIcon unskip();

GT_GUI_EXPORT QIcon sleep();
GT_GUI_EXPORT QIcon sleepOff();

GT_GUI_EXPORT QIcon square();

GT_GUI_EXPORT QIcon sort();
GT_GUI_EXPORT QIcon sortAsc();
GT_GUI_EXPORT QIcon sortDesc();

GT_GUI_EXPORT QIcon stop();

GT_GUI_EXPORT QIcon stepFile();

GT_GUI_EXPORT QIcon stretch();

GT_GUI_EXPORT QIcon swap();

GT_GUI_EXPORT QIcon sync();

GT_GUI_EXPORT QIcon table();

GT_GUI_EXPORT QIcon text();

GT_GUI_EXPORT QIcon timer();
GT_GUI_EXPORT QIcon timerSand();

GT_GUI_EXPORT QIcon trace();
GT_GUI_EXPORT QIcon traceColorized();

GT_GUI_EXPORT QIcon tree();

GT_GUI_EXPORT QIcon triangleDown();
GT_GUI_EXPORT QIcon triangleLeft();
GT_GUI_EXPORT QIcon triangleRight();
GT_GUI_EXPORT QIcon triangleRightGroup();
GT_GUI_EXPORT QIcon triangleUp();
GT_GUI_EXPORT QIcon triangleSmallDown();
GT_GUI_EXPORT QIcon triangleSmallLeft();
GT_GUI_EXPORT QIcon triangleSmallRight();
GT_GUI_EXPORT QIcon triangleSmallUp();

GT_GUI_EXPORT QIcon undo();

GT_GUI_EXPORT QIcon update();

GT_GUI_EXPORT QIcon upload();

GT_GUI_EXPORT QIcon variable();

GT_GUI_EXPORT QIcon vectorBezier();
GT_GUI_EXPORT QIcon vectorBezier2();
GT_GUI_EXPORT QIcon vectorCurve();
GT_GUI_EXPORT QIcon vectorLine();
GT_GUI_EXPORT QIcon vectorPoint();
GT_GUI_EXPORT QIcon vectorPointEdit();
GT_GUI_EXPORT QIcon vectorPoly();
GT_GUI_EXPORT QIcon vectorSimple();
GT_GUI_EXPORT QIcon vectorSquare();
GT_GUI_EXPORT QIcon vectorSquareEdit();
GT_GUI_EXPORT QIcon vectorTriangle();

GT_GUI_EXPORT QIcon warning();
GT_GUI_EXPORT QIcon warningColorized();

GT_GUI_EXPORT QIcon web();

GT_GUI_EXPORT QIcon xml();

GT_GUI_EXPORT QIcon zoomMinus();
GT_GUI_EXPORT QIcon zoomPlus();


/** OTHER **/

GT_GUI_EXPORT QIcon engineInstallation();

GT_GUI_EXPORT QIcon carpetPlot();

GT_GUI_EXPORT QIcon bleedInPort();
GT_GUI_EXPORT QIcon bleedInPort16();
GT_GUI_EXPORT QIcon bleedInPort24();

GT_GUI_EXPORT QIcon bleedOutPort();
GT_GUI_EXPORT QIcon bleedOutPort16();
GT_GUI_EXPORT QIcon bleedOutPort24();

GT_GUI_EXPORT QIcon bleedPortGroup();
GT_GUI_EXPORT QIcon bleedPortGroup16();
GT_GUI_EXPORT QIcon bleedPortGroup24();

GT_GUI_EXPORT QIcon fluidPort();
GT_GUI_EXPORT QIcon fluidPort16();
GT_GUI_EXPORT QIcon fluidPort24();

GT_GUI_EXPORT QIcon fluidPortGroup();
GT_GUI_EXPORT QIcon fluidPortGroup16();
GT_GUI_EXPORT QIcon fluidPortGroup24();

GT_GUI_EXPORT QIcon shaftPort();
GT_GUI_EXPORT QIcon shaftPort16();
GT_GUI_EXPORT QIcon shaftPort24();

GT_GUI_EXPORT QIcon processRunningIcon(int progress);

/// namespace for letter icons (standard is blue)
namespace letter {

GT_GUI_EXPORT QIcon a();
GT_GUI_EXPORT QIcon aSmall();

GT_GUI_EXPORT QIcon b();
GT_GUI_EXPORT QIcon bSmall();

GT_GUI_EXPORT QIcon c();
GT_GUI_EXPORT QIcon cSmall();

GT_GUI_EXPORT QIcon d();
GT_GUI_EXPORT QIcon dSmall();

GT_GUI_EXPORT QIcon e();
GT_GUI_EXPORT QIcon eSmall();
GT_GUI_EXPORT QIcon eColorized();
GT_GUI_EXPORT QIcon eSmallColorized();

GT_GUI_EXPORT QIcon f();
GT_GUI_EXPORT QIcon fSmall();

GT_GUI_EXPORT QIcon g();
GT_GUI_EXPORT QIcon gSmall();

GT_GUI_EXPORT QIcon h();
GT_GUI_EXPORT QIcon hSmall();

GT_GUI_EXPORT QIcon i();
GT_GUI_EXPORT QIcon iSmall();

GT_GUI_EXPORT QIcon j();
GT_GUI_EXPORT QIcon jSmall();

GT_GUI_EXPORT QIcon k();
GT_GUI_EXPORT QIcon kSmall();

GT_GUI_EXPORT QIcon l();
GT_GUI_EXPORT QIcon lSmall();

GT_GUI_EXPORT QIcon m();
GT_GUI_EXPORT QIcon mSmall();

GT_GUI_EXPORT QIcon n();
GT_GUI_EXPORT QIcon nSmall();

GT_GUI_EXPORT QIcon o();
GT_GUI_EXPORT QIcon oSmall();

GT_GUI_EXPORT QIcon p();
GT_GUI_EXPORT QIcon pSmall();

GT_GUI_EXPORT QIcon q();
GT_GUI_EXPORT QIcon qSmall();

GT_GUI_EXPORT QIcon r();
GT_GUI_EXPORT QIcon rSmall();

GT_GUI_EXPORT QIcon s();
GT_GUI_EXPORT QIcon sSmall();

GT_GUI_EXPORT QIcon t();
GT_GUI_EXPORT QIcon tSmall();

GT_GUI_EXPORT QIcon u();
GT_GUI_EXPORT QIcon uSmall();

GT_GUI_EXPORT QIcon v();
GT_GUI_EXPORT QIcon vSmall();

GT_GUI_EXPORT QIcon w();
GT_GUI_EXPORT QIcon wSmall();

GT_GUI_EXPORT QIcon x();
GT_GUI_EXPORT QIcon xSmall();

GT_GUI_EXPORT QIcon y();
GT_GUI_EXPORT QIcon ySmall();

GT_GUI_EXPORT QIcon z();
GT_GUI_EXPORT QIcon zSmall();

} // namespace letters

} // namespace icon

namespace pixmap
{

/// path to background pixmap
GT_GUI_EXPORT QString backgroundPath();

GT_GUI_EXPORT QPixmap background();

GT_GUI_EXPORT QPixmap splash();

GT_GUI_EXPORT QPixmap accessLogo();

GT_GUI_EXPORT QPixmap updateLogo();

GT_GUI_EXPORT QPixmap changeLogLogo();

GT_GUI_EXPORT QPixmap logo();

GT_GUI_EXPORT QString logoString();

GT_GUI_EXPORT QPixmap downloaderLogo();

GT_GUI_EXPORT QPixmap printHeadline();

} // namespace pixmap

} // namespace gui

} // namespace gt

#endif // GTICONS_H
