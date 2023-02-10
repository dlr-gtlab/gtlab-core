/* GTlab - Gas Turbine laboratory
 * Source File: gt_icons.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.04.2022
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTICONS_H
#define GTICONS_H

#include "gt_gui_exports.h"
#include <QIcon>

namespace gt
{
namespace gui
{

/**
 *  @brief Creates a new icon from a resource file.
 *  @param iconPath Relative or custom resource path to a icon file.
 */
GT_GUI_EXPORT QIcon getIcon(const QString& iconPath);

namespace icon
{

/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

GT_GUI_EXPORT QIcon add();
[[deprecated("Use add instead")]]
GT_GUI_EXPORT QIcon add16();
[[deprecated("Use projectAdd instead")]]
GT_GUI_EXPORT QIcon addProject();

GT_GUI_EXPORT QIcon application();
GT_GUI_EXPORT QIcon applicationSettings();
GT_GUI_EXPORT QIcon applicationVar();

GT_GUI_EXPORT QIcon arrowDown();
GT_GUI_EXPORT QIcon arrowUp();

GT_GUI_EXPORT QIcon arrowRight();
[[deprecated("Use arrowRight instead")]]
GT_GUI_EXPORT QIcon arrowRightBlue();
GT_GUI_EXPORT QIcon arrowLeft();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT QIcon arrowDownBlue();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT QIcon arrowDownBlueAll();
[[deprecated("Use arrowUp instead")]]
GT_GUI_EXPORT QIcon arrowUpBlueAll();

GT_GUI_EXPORT QIcon backspace();
GT_GUI_EXPORT QIcon backspaceFlipped();

GT_GUI_EXPORT QIcon brush();

GT_GUI_EXPORT QIcon bug();
GT_GUI_EXPORT QIcon bugColorized();
GT_GUI_EXPORT QIcon bugPlay();

GT_GUI_EXPORT QIcon bulb();

GT_GUI_EXPORT QIcon calculator();
[[deprecated("Use calculator instead")]]
GT_GUI_EXPORT QIcon calculator16();

GT_GUI_EXPORT QIcon cancel();

GT_GUI_EXPORT QIcon chain();
[[deprecated("Use chain instead")]]
GT_GUI_EXPORT QIcon chain16();
GT_GUI_EXPORT QIcon chainOff();

GT_GUI_EXPORT QIcon chartBar();
GT_GUI_EXPORT QIcon chartBell();
GT_GUI_EXPORT QIcon chartCurve();
GT_GUI_EXPORT QIcon chartLine();
GT_GUI_EXPORT QIcon chartScatter();

GT_GUI_EXPORT QIcon check();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon check16();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon check24();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon checkSmall16();


GT_GUI_EXPORT QIcon clear();
[[deprecated("Use clear instead")]]
GT_GUI_EXPORT QIcon clear16();
[[deprecated("Use backspaceFlipped instead")]]
GT_GUI_EXPORT QIcon clear2_16();

GT_GUI_EXPORT QIcon clone();
[[deprecated("Use clone instead")]]
GT_GUI_EXPORT QIcon clone16();

[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject16();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject24();

[[deprecated("Use projectClosed instead")]]
GT_GUI_EXPORT QIcon closedProject16();

GT_GUI_EXPORT QIcon close();

GT_GUI_EXPORT QIcon cog();
GT_GUI_EXPORT QIcon cogPlus();

GT_GUI_EXPORT QIcon collapsed();
GT_GUI_EXPORT QIcon collapsedColorized();
GT_GUI_EXPORT QIcon uncollapsed();
GT_GUI_EXPORT QIcon uncollapsedColorized();

GT_GUI_EXPORT QIcon comment();

GT_GUI_EXPORT QIcon config();
[[deprecated("Use config instead")]]
GT_GUI_EXPORT QIcon config16();

GT_GUI_EXPORT QIcon connection();
GT_GUI_EXPORT QIcon connection16();

GT_GUI_EXPORT QIcon copy();
[[deprecated("Use copy instead")]]
GT_GUI_EXPORT QIcon copy16();

GT_GUI_EXPORT QIcon cross();
[[deprecated("Use cross instead")]]
GT_GUI_EXPORT QIcon cross16();

GT_GUI_EXPORT QIcon cursor();
GT_GUI_EXPORT QIcon cursorMove();

GT_GUI_EXPORT QIcon cut();
[[deprecated("Use cut instead")]]
GT_GUI_EXPORT QIcon cut16();

GT_GUI_EXPORT QIcon data();
[[deprecated("Use data instead")]]
GT_GUI_EXPORT QIcon data16();
GT_GUI_EXPORT QIcon dataArrowUp();
GT_GUI_EXPORT QIcon dataEye();

GT_GUI_EXPORT QIcon delete_();
[[deprecated("Use delete_ instead")]]
GT_GUI_EXPORT QIcon delete16();

[[deprecated("Use projectDelete instead")]]
GT_GUI_EXPORT QIcon deleteProject16();

GT_GUI_EXPORT QIcon devices();

GT_GUI_EXPORT QIcon dock();

GT_GUI_EXPORT QIcon dots();

GT_GUI_EXPORT QIcon download();

GT_GUI_EXPORT QIcon duplicate();

[[deprecated("Use object instead")]]
GT_GUI_EXPORT QIcon empty16();

GT_GUI_EXPORT QIcon error();
GT_GUI_EXPORT QIcon errorColorized();
[[deprecated("Use error instead")]]
GT_GUI_EXPORT QIcon error16();

GT_GUI_EXPORT QIcon examples();
[[deprecated("Use examples instead")]]
GT_GUI_EXPORT QIcon examples16();

GT_GUI_EXPORT QIcon exclamationmark();
[[deprecated("Use exclamation instead")]]
GT_GUI_EXPORT QIcon exclamation16();

GT_GUI_EXPORT QIcon export_();
[[deprecated("Use export_ instead")]]
GT_GUI_EXPORT QIcon export16();

GT_GUI_EXPORT QIcon eye();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT QIcon eye16();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT QIcon eye24();
GT_GUI_EXPORT QIcon eyeOff();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT QIcon eyeOff16();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT QIcon eyeOff24();

GT_GUI_EXPORT QIcon fatal();
GT_GUI_EXPORT QIcon fatalColorized();
[[deprecated("Use fatal instead")]]
GT_GUI_EXPORT QIcon fatal16();

GT_GUI_EXPORT QIcon file();
GT_GUI_EXPORT QIcon fileAdd();
GT_GUI_EXPORT QIcon fileClock();
GT_GUI_EXPORT QIcon fileCode();
GT_GUI_EXPORT QIcon fileDelete();
GT_GUI_EXPORT QIcon fileDoc();
GT_GUI_EXPORT QIcon fileEdit();
GT_GUI_EXPORT QIcon fileEye();
GT_GUI_EXPORT QIcon fileImport();
GT_GUI_EXPORT QIcon fileStep();

[[deprecated("Use file instead")]]
GT_GUI_EXPORT QIcon file16();

GT_GUI_EXPORT QIcon folder();
[[deprecated("Use folder instead")]]
GT_GUI_EXPORT QIcon folder16();
GT_GUI_EXPORT QIcon folderAdd();
GT_GUI_EXPORT QIcon folderEdit();
GT_GUI_EXPORT QIcon folderEye();
GT_GUI_EXPORT QIcon folderOpen();
GT_GUI_EXPORT QIcon folderSearch();

GT_GUI_EXPORT QIcon font();

GT_GUI_EXPORT QIcon gas();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gas16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gas24();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes24();

GT_GUI_EXPORT QIcon global();

GT_GUI_EXPORT QIcon grid();

GT_GUI_EXPORT QIcon help();

GT_GUI_EXPORT QIcon histogram();
[[deprecated("Use histogram() instead")]]
GT_GUI_EXPORT QIcon histogram16();

GT_GUI_EXPORT QIcon history();
[[deprecated("Use history() instead")]]
GT_GUI_EXPORT QIcon history16();

GT_GUI_EXPORT QIcon home();
GT_GUI_EXPORT QIcon homeAccount();

GT_GUI_EXPORT QIcon import();
[[deprecated("Use import() instead")]]
GT_GUI_EXPORT QIcon import16();

[[deprecated("Use objectInvalid instead")]]
GT_GUI_EXPORT QIcon inconsistentProject16();

GT_GUI_EXPORT QIcon info();
GT_GUI_EXPORT QIcon infoColorized();
GT_GUI_EXPORT QIcon info2();
[[deprecated("Use info instead")]]
GT_GUI_EXPORT QIcon info16();

[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT QIcon infoBlue();
[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT QIcon infoBlue16();

GT_GUI_EXPORT QIcon input();
GT_GUI_EXPORT QIcon inputForm();
[[deprecated("Use input instead")]]
GT_GUI_EXPORT QIcon input16();
[[deprecated("Use inputSettings instead")]]
GT_GUI_EXPORT QIcon input2();
GT_GUI_EXPORT QIcon inputSettings();

GT_GUI_EXPORT QIcon jumpTo();

GT_GUI_EXPORT QIcon json();

GT_GUI_EXPORT QIcon label();

GT_GUI_EXPORT QIcon language();

GT_GUI_EXPORT QIcon layers();

GT_GUI_EXPORT QIcon list();
[[deprecated("Use list instead")]]
GT_GUI_EXPORT QIcon list16();
GT_GUI_EXPORT QIcon listFormatted();

GT_GUI_EXPORT QIcon log();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT QIcon log16();

GT_GUI_EXPORT QIcon login();
[[deprecated("Use login instead")]]
GT_GUI_EXPORT QIcon login16();

GT_GUI_EXPORT QIcon loop();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT QIcon loop16();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT QIcon loop2_16();

GT_GUI_EXPORT QIcon mathDivision();
GT_GUI_EXPORT QIcon mathExponent();
GT_GUI_EXPORT QIcon mathMinus();
GT_GUI_EXPORT QIcon mathMultiplication();
GT_GUI_EXPORT QIcon mathPlus();
GT_GUI_EXPORT QIcon mathRoot();

GT_GUI_EXPORT QIcon network();
[[deprecated("Use network instead")]]
GT_GUI_EXPORT QIcon network16();

GT_GUI_EXPORT QIcon noteEdit();

GT_GUI_EXPORT QIcon notificationUnread();

GT_GUI_EXPORT QIcon objectEmpty();
GT_GUI_EXPORT QIcon objectInvalid();
GT_GUI_EXPORT QIcon objectUnkown();

GT_GUI_EXPORT QIcon open();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2_16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2_24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT QIcon openProject16();

GT_GUI_EXPORT QIcon palette();

GT_GUI_EXPORT QIcon paste();
[[deprecated("Use paste instead")]]
GT_GUI_EXPORT QIcon paste16();

GT_GUI_EXPORT QIcon pause();

GT_GUI_EXPORT QIcon perspectives();
[[deprecated("Use perspectives instead")]]
GT_GUI_EXPORT QIcon perspectives16();
GT_GUI_EXPORT QIcon perspectivesEdit();

GT_GUI_EXPORT QIcon pdf();

GT_GUI_EXPORT QIcon play();

GT_GUI_EXPORT QIcon plugin();
[[deprecated("Use plugin instead")]]
GT_GUI_EXPORT QIcon plugin16();

GT_GUI_EXPORT QIcon print();
[[deprecated("Use print instead")]]
GT_GUI_EXPORT QIcon printer();

GT_GUI_EXPORT QIcon process();
[[deprecated("Use process instead")]]
GT_GUI_EXPORT QIcon process16();
GT_GUI_EXPORT QIcon processAdd();
GT_GUI_EXPORT QIcon processRun();

[[deprecated("Use warning instead")]]
GT_GUI_EXPORT QIcon processFailed16();

GT_GUI_EXPORT QIcon projectAdd();
GT_GUI_EXPORT QIcon projectClose();
GT_GUI_EXPORT QIcon projectClosed();
GT_GUI_EXPORT QIcon projectDelete();
GT_GUI_EXPORT QIcon projectImport();
GT_GUI_EXPORT QIcon projectOpen();
GT_GUI_EXPORT QIcon projectRemove();

GT_GUI_EXPORT QIcon property();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT QIcon property16();

GT_GUI_EXPORT QIcon prp();
[[deprecated("Use prp instead")]]
GT_GUI_EXPORT QIcon prp16();

GT_GUI_EXPORT QIcon puzzle();

[[deprecated]]
GT_GUI_EXPORT QIcon python();
[[deprecated]]
GT_GUI_EXPORT QIcon python16();

GT_GUI_EXPORT QIcon questionmark();
[[deprecated("Use questionmark instead")]]
GT_GUI_EXPORT QIcon question16();

GT_GUI_EXPORT QIcon queue();
[[deprecated("Use queue instead")]]
GT_GUI_EXPORT QIcon queue16();
GT_GUI_EXPORT QIcon queueAdd();

GT_GUI_EXPORT QIcon redo();
[[deprecated("Use redo instead")]]
GT_GUI_EXPORT QIcon redo24();

GT_GUI_EXPORT QIcon reload();

GT_GUI_EXPORT QIcon remove();

GT_GUI_EXPORT QIcon rename();

GT_GUI_EXPORT QIcon repeat();

GT_GUI_EXPORT QIcon resize();

GT_GUI_EXPORT QIcon results();
[[deprecated("Use results instead")]]
GT_GUI_EXPORT QIcon results16();

GT_GUI_EXPORT QIcon revert();
[[deprecated("Use revert instead")]]
GT_GUI_EXPORT QIcon restore16();

GT_GUI_EXPORT QIcon ruler();

[[deprecated("Use processRun instead")]]
GT_GUI_EXPORT QIcon runProcess16();

GT_GUI_EXPORT QIcon save();
[[deprecated("Use save instead")]]
GT_GUI_EXPORT QIcon saveProject16();

GT_GUI_EXPORT QIcon search();
[[deprecated("Use search instead")]]
GT_GUI_EXPORT QIcon search16();

GT_GUI_EXPORT QIcon select();

GT_GUI_EXPORT QIcon server();

GT_GUI_EXPORT QIcon session();
[[deprecated("Use session instead")]]
GT_GUI_EXPORT QIcon session16();

GT_GUI_EXPORT QIcon skip();
[[deprecated("Use skip instead")]]
GT_GUI_EXPORT QIcon skip16();

GT_GUI_EXPORT QIcon sleep();
[[deprecated("Use sleep instead")]]
GT_GUI_EXPORT QIcon sleep16();

GT_GUI_EXPORT QIcon gridSnap();

GT_GUI_EXPORT QIcon sort();
GT_GUI_EXPORT QIcon sortAsc();
GT_GUI_EXPORT QIcon sortDesc();

GT_GUI_EXPORT QIcon stop();
[[deprecated("Use stop instead")]]
GT_GUI_EXPORT QIcon stop16();

GT_GUI_EXPORT QIcon stretch();

GT_GUI_EXPORT QIcon swap();

GT_GUI_EXPORT QIcon sync();

GT_GUI_EXPORT QIcon table();
[[deprecated("Use table instead")]]
GT_GUI_EXPORT QIcon table16();

GT_GUI_EXPORT QIcon text();

GT_GUI_EXPORT QIcon timer();
GT_GUI_EXPORT QIcon timerSand();

GT_GUI_EXPORT QIcon trace();
GT_GUI_EXPORT QIcon traceColorized();
[[deprecated("Use trace instead")]]
GT_GUI_EXPORT QIcon trace16();

GT_GUI_EXPORT QIcon tree();

GT_GUI_EXPORT QIcon triangleSmallDown();
GT_GUI_EXPORT QIcon triangleSmallLeft();
GT_GUI_EXPORT QIcon triangleSmallRight();
GT_GUI_EXPORT QIcon triangleSmallUp();

GT_GUI_EXPORT QIcon turbine();

GT_GUI_EXPORT QIcon undo();
[[deprecated("Use undo instead")]]
GT_GUI_EXPORT QIcon undo24();

GT_GUI_EXPORT QIcon update();
[[deprecated("Use update instead")]]
GT_GUI_EXPORT QIcon update16();

GT_GUI_EXPORT QIcon upload();

GT_GUI_EXPORT QIcon variable();

GT_GUI_EXPORT QIcon vectorBezier();
GT_GUI_EXPORT QIcon vectorCurve();
GT_GUI_EXPORT QIcon vectorLine();
GT_GUI_EXPORT QIcon vectorPoint();
GT_GUI_EXPORT QIcon vectorPointEdit();
GT_GUI_EXPORT QIcon vectorSquare();
GT_GUI_EXPORT QIcon vectorSquareEdit();
GT_GUI_EXPORT QIcon vectorTriangle();

GT_GUI_EXPORT QIcon warning();
GT_GUI_EXPORT QIcon warningColorized();
[[deprecated("Use warning instead")]]
GT_GUI_EXPORT QIcon warning16();

GT_GUI_EXPORT QIcon web();

GT_GUI_EXPORT QIcon xml();

GT_GUI_EXPORT QIcon zoomPlus();
[[deprecated("Use zoomPlus instead")]]
GT_GUI_EXPORT QIcon zoom16();



/** DEPRECATED **/
[[deprecated("Use objectEmpty instead")]]
GT_GUI_EXPORT QIcon emptyElement();

[[deprecated("Use timerSand instead")]]
GT_GUI_EXPORT QIcon inProgress16();

[[deprecated("Use timer instead")]]
GT_GUI_EXPORT QIcon stopRequest16();

[[deprecated("Use applicationVar instead")]]
GT_GUI_EXPORT QIcon pathSettings();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT QIcon frame();

[[deprecated("Use objectUnkown instead")]]
GT_GUI_EXPORT QIcon unknown();

[[deprecated("Use close instead")]]
GT_GUI_EXPORT QIcon close24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT QIcon components16();

[[deprecated("Use chartLine instead")]]
GT_GUI_EXPORT QIcon post();

[[deprecated("Use label instead")]]
GT_GUI_EXPORT QIcon labels();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT QIcon stack();

[[deprecated("Use server instead")]]
GT_GUI_EXPORT QIcon collection16();

[[deprecated("Use pdf instead")]]
GT_GUI_EXPORT QIcon printPDF();

[[deprecated("Use update instead")]]
GT_GUI_EXPORT QIcon upgradeProjectData();

[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move16();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move24();

[[deprecated("Use mathPlus instead")]]
GT_GUI_EXPORT QIcon plus();
[[deprecated("Use mathMinus instead")]]
GT_GUI_EXPORT QIcon minus();
[[deprecated("Use mathMultiplication instead")]]
GT_GUI_EXPORT QIcon multiply();
[[deprecated("Use mathDivision instead")]]
GT_GUI_EXPORT QIcon divide();
[[deprecated("Use mathExponent instead")]]
GT_GUI_EXPORT QIcon square();
[[deprecated("Use mathRoot instead")]]
GT_GUI_EXPORT QIcon squareRoot();

[[deprecated("Use fileStep instead")]]
GT_GUI_EXPORT QIcon stepFile();
[[deprecated("Use fileStep instead")]]
GT_GUI_EXPORT QIcon stepFile16();
[[deprecated("Use fileStep instead")]]
GT_GUI_EXPORT QIcon stepFile24();

[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn16();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn24();

[[deprecated("Use text instead")]]
GT_GUI_EXPORT QIcon textNote24();

[[deprecated("Use vectorTriangle instead")]]
GT_GUI_EXPORT QIcon polyCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT QIcon bezierCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT QIcon simpleCurve();

[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp16();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp24();

[[deprecated("Use noteEdit instead")]]
GT_GUI_EXPORT QIcon editPen();

[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT QIcon addElement16();
[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT QIcon addProcess16();

[[deprecated("use download instead")]]
GT_GUI_EXPORT QIcon dropdown();

[[deprecated("Use data instead")]]
GT_GUI_EXPORT QIcon globals16();

/** MISSING **/
GT_GUI_EXPORT QIcon pluginSettings();

GT_GUI_EXPORT QIcon minimize();

GT_GUI_EXPORT QIcon map();
GT_GUI_EXPORT QIcon map16();
GT_GUI_EXPORT QIcon map24();
GT_GUI_EXPORT QIcon noMap();
GT_GUI_EXPORT QIcon exportMap();
GT_GUI_EXPORT QIcon exportMap16();
GT_GUI_EXPORT QIcon exportMap24();
GT_GUI_EXPORT QIcon carpetPlot();
GT_GUI_EXPORT QIcon xyPlot16();
GT_GUI_EXPORT QIcon tsDiagram16();
GT_GUI_EXPORT QIcon blades();

GT_GUI_EXPORT QIcon showText();
GT_GUI_EXPORT QIcon showText16();
GT_GUI_EXPORT QIcon showText24();

GT_GUI_EXPORT QIcon convergence();
GT_GUI_EXPORT QIcon convergence16();
GT_GUI_EXPORT QIcon convergence24();

GT_GUI_EXPORT QIcon double16();
GT_GUI_EXPORT QIcon int16();
GT_GUI_EXPORT QIcon o16();
GT_GUI_EXPORT QIcon string16();


/** OTHER **/
GT_GUI_EXPORT QIcon performanceModel();

GT_GUI_EXPORT QIcon bleedInPort();
GT_GUI_EXPORT QIcon bleedInPort16();
GT_GUI_EXPORT QIcon bleedInPort24();

GT_GUI_EXPORT QIcon bleedOutPort();
GT_GUI_EXPORT QIcon bleedOutPort16();
GT_GUI_EXPORT QIcon bleedOutPort24();

GT_GUI_EXPORT QIcon bleedPortGroup();
GT_GUI_EXPORT QIcon bleedPortGroup16();
GT_GUI_EXPORT QIcon bleedPortGroup24();

GT_GUI_EXPORT QIcon engine();
GT_GUI_EXPORT QIcon engine16();

GT_GUI_EXPORT QIcon engineInstallation16();
GT_GUI_EXPORT QIcon engineInstallation24();
GT_GUI_EXPORT QIcon engineInstallation32();

GT_GUI_EXPORT QIcon performanceDesign();
GT_GUI_EXPORT QIcon performanceDesign16();
GT_GUI_EXPORT QIcon performanceDesign24();

GT_GUI_EXPORT QIcon performanceOffDesign();
GT_GUI_EXPORT QIcon performanceOffDesign16();
GT_GUI_EXPORT QIcon performanceOffDesign24();

GT_GUI_EXPORT QIcon fluidPort();
GT_GUI_EXPORT QIcon fluidPort16();
GT_GUI_EXPORT QIcon fluidPort24();

GT_GUI_EXPORT QIcon fluidPortGroup();
GT_GUI_EXPORT QIcon fluidPortGroup16();
GT_GUI_EXPORT QIcon fluidPortGroup24();

GT_GUI_EXPORT QIcon shaftPort();
GT_GUI_EXPORT QIcon shaftPort16();
GT_GUI_EXPORT QIcon shaftPort24();

GT_GUI_EXPORT QIcon speedLine();
GT_GUI_EXPORT QIcon speedLine16();
GT_GUI_EXPORT QIcon speedLine24();

GT_GUI_EXPORT QIcon compressor();
GT_GUI_EXPORT QIcon compressor16();

GT_GUI_EXPORT QIcon turbine();
GT_GUI_EXPORT QIcon turbine16();

GT_GUI_EXPORT QIcon combustor();
GT_GUI_EXPORT QIcon combustor16();

GT_GUI_EXPORT QIcon paramStudy();
GT_GUI_EXPORT QIcon paramStudy16();
GT_GUI_EXPORT QIcon paramStudy24();

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
GT_GUI_EXPORT QIcon eBrown();
GT_GUI_EXPORT QIcon eBrownSmall();

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

GT_GUI_EXPORT QPixmap splash();

GT_GUI_EXPORT QPixmap accessLogo();

GT_GUI_EXPORT QPixmap updateLogo();

GT_GUI_EXPORT QPixmap changeLogLogo();

GT_GUI_EXPORT QPixmap logo();

GT_GUI_EXPORT QString logoString();

[[deprecated("Use icon::jumpTo().pixmap(...) instead")]]
GT_GUI_EXPORT QPixmap jumpTo();

GT_GUI_EXPORT QPixmap downloaderLogo();

[[deprecated("Use icon::questionmark().pixmap(...) instead")]]
GT_GUI_EXPORT QPixmap question24();

GT_GUI_EXPORT QPixmap printHeadline();

} // namespace pixmap

} // namespace gui

} // namespace gt

#endif // GTICONS_H
