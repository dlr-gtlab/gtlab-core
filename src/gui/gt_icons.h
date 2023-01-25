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
GT_GUI_EXPORT QIcon getIcon(const QString &iconPath);

namespace icon
{

/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

/**
     * @brief add16
     * @return add icon i the size of 16x16
     */
GT_GUI_EXPORT QIcon add16();

GT_GUI_EXPORT QIcon delete16();

GT_GUI_EXPORT QIcon error16();

GT_GUI_EXPORT QIcon error();

GT_GUI_EXPORT QIcon export16();

GT_GUI_EXPORT QIcon import16();

GT_GUI_EXPORT QIcon saveProject16();

GT_GUI_EXPORT QIcon deleteProject16();

GT_GUI_EXPORT QIcon addProject();

GT_GUI_EXPORT QIcon closeProject();
GT_GUI_EXPORT QIcon closeProject16();
GT_GUI_EXPORT QIcon closeProject24();

GT_GUI_EXPORT QIcon closedProject16();

GT_GUI_EXPORT QIcon inconsistentProject16();

GT_GUI_EXPORT QIcon openProject16();

GT_GUI_EXPORT QIcon help();

GT_GUI_EXPORT QIcon info();

GT_GUI_EXPORT QIcon info16();

GT_GUI_EXPORT QIcon infoBlue();

GT_GUI_EXPORT QIcon infoBlue16();

GT_GUI_EXPORT QIcon runProcess16();

GT_GUI_EXPORT QIcon stop16();

GT_GUI_EXPORT QIcon addProcess16();

GT_GUI_EXPORT QIcon processFailed16();

GT_GUI_EXPORT QIcon skip16();

GT_GUI_EXPORT QIcon process16();

GT_GUI_EXPORT QIcon checkSmall16();

GT_GUI_EXPORT QIcon check16();

GT_GUI_EXPORT QIcon check24();

GT_GUI_EXPORT QIcon clear16();

GT_GUI_EXPORT QIcon clear2_16();

GT_GUI_EXPORT QIcon config16();

GT_GUI_EXPORT QIcon input16();

GT_GUI_EXPORT QIcon plugin16();

GT_GUI_EXPORT QIcon plugin();

GT_GUI_EXPORT QIcon session16();

GT_GUI_EXPORT QIcon session();

GT_GUI_EXPORT QIcon login16();

GT_GUI_EXPORT QIcon update16();

GT_GUI_EXPORT QIcon results16();

GT_GUI_EXPORT QIcon file16();

GT_GUI_EXPORT QIcon folder16();

GT_GUI_EXPORT QIcon calculator16();

GT_GUI_EXPORT QIcon copy16();

GT_GUI_EXPORT QIcon paste16();

GT_GUI_EXPORT QIcon cut16();

GT_GUI_EXPORT QIcon clone16();

GT_GUI_EXPORT QIcon connection16();

GT_GUI_EXPORT QIcon empty16();

GT_GUI_EXPORT QIcon property16();

GT_GUI_EXPORT QIcon log16();

GT_GUI_EXPORT QIcon open16();

GT_GUI_EXPORT QIcon open2_16();

GT_GUI_EXPORT QIcon open2_24();

GT_GUI_EXPORT QIcon open2();

GT_GUI_EXPORT QIcon emptyElement();

GT_GUI_EXPORT QIcon arrowRight();

GT_GUI_EXPORT QIcon arrowRightBlue();

GT_GUI_EXPORT QIcon arrowLeft();

GT_GUI_EXPORT QIcon arrowUp();

GT_GUI_EXPORT QIcon arrowDown();

GT_GUI_EXPORT QIcon arrowDownBlue();

GT_GUI_EXPORT QIcon arrowUpBlueAll();

GT_GUI_EXPORT QIcon arrowDownBlueAll();

GT_GUI_EXPORT QIcon global();

GT_GUI_EXPORT QIcon globals16();

GT_GUI_EXPORT QIcon loop16();

GT_GUI_EXPORT QIcon loop2_16();

GT_GUI_EXPORT QIcon bug();

GT_GUI_EXPORT QIcon fatal16();

GT_GUI_EXPORT QIcon prp16();

GT_GUI_EXPORT QIcon examples16();

GT_GUI_EXPORT QIcon double16();

GT_GUI_EXPORT QIcon int16();

GT_GUI_EXPORT QIcon o16();

GT_GUI_EXPORT QIcon string16();

GT_GUI_EXPORT QIcon sleep16();

GT_GUI_EXPORT QIcon inProgress16();

GT_GUI_EXPORT QIcon network16();

GT_GUI_EXPORT QIcon undo24();

GT_GUI_EXPORT QIcon redo24();

GT_GUI_EXPORT QIcon history16();

GT_GUI_EXPORT QIcon queue16();

GT_GUI_EXPORT QIcon stopRequest16();

GT_GUI_EXPORT QIcon addElement16();

GT_GUI_EXPORT QIcon application();

GT_GUI_EXPORT QIcon perspectives();

GT_GUI_EXPORT QIcon input2();

GT_GUI_EXPORT QIcon pluginSettings();

GT_GUI_EXPORT QIcon language();

GT_GUI_EXPORT QIcon login();

GT_GUI_EXPORT QIcon pathSettings();

GT_GUI_EXPORT QIcon restore16();

GT_GUI_EXPORT QIcon frame();

GT_GUI_EXPORT QIcon unknown();

GT_GUI_EXPORT QIcon exclamation16();

GT_GUI_EXPORT QIcon search16();

GT_GUI_EXPORT QIcon close24();

GT_GUI_EXPORT QIcon dropdown();

GT_GUI_EXPORT QIcon data16();

GT_GUI_EXPORT QIcon minimize();

GT_GUI_EXPORT QIcon components16();

GT_GUI_EXPORT QIcon perspectives16();

GT_GUI_EXPORT QIcon examples();

GT_GUI_EXPORT QIcon chain16();

GT_GUI_EXPORT QIcon post();

GT_GUI_EXPORT QIcon labels();

GT_GUI_EXPORT QIcon comment();

GT_GUI_EXPORT QIcon stack();

GT_GUI_EXPORT QIcon collection16();

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

GT_GUI_EXPORT QIcon histogram16();

GT_GUI_EXPORT QIcon printer();

GT_GUI_EXPORT QIcon printPDF();

GT_GUI_EXPORT QIcon list16();

GT_GUI_EXPORT QIcon question16();

GT_GUI_EXPORT QIcon zoom16();

GT_GUI_EXPORT QIcon cross16();

GT_GUI_EXPORT QIcon jumpTo();

GT_GUI_EXPORT QIcon upgradeProjectData();

GT_GUI_EXPORT QIcon duplicate();

GT_GUI_EXPORT QIcon blades();

GT_GUI_EXPORT QIcon gas();

GT_GUI_EXPORT QIcon gas16();

GT_GUI_EXPORT QIcon gas24();

GT_GUI_EXPORT QIcon gasTypes();

GT_GUI_EXPORT QIcon gasTypes16();

GT_GUI_EXPORT QIcon gasTypes24();

GT_GUI_EXPORT QIcon grid();

GT_GUI_EXPORT QIcon gridSnap();

GT_GUI_EXPORT QIcon showText();

GT_GUI_EXPORT QIcon showText16();

GT_GUI_EXPORT QIcon showText24();

GT_GUI_EXPORT QIcon move();

GT_GUI_EXPORT QIcon move16();

GT_GUI_EXPORT QIcon move24();

GT_GUI_EXPORT QIcon editPen();

GT_GUI_EXPORT QIcon resize();

GT_GUI_EXPORT QIcon plus();

GT_GUI_EXPORT QIcon minus();

GT_GUI_EXPORT QIcon multiply();

GT_GUI_EXPORT QIcon divide();

GT_GUI_EXPORT QIcon square();

GT_GUI_EXPORT QIcon squareRoot();

GT_GUI_EXPORT QIcon convergence();

GT_GUI_EXPORT QIcon convergence16();

GT_GUI_EXPORT QIcon convergence24();

GT_GUI_EXPORT QIcon eye();

GT_GUI_EXPORT QIcon eye16();

GT_GUI_EXPORT QIcon eye24();

GT_GUI_EXPORT QIcon eyeOff();

GT_GUI_EXPORT QIcon eyeOff16();

GT_GUI_EXPORT QIcon eyeOff24();

GT_GUI_EXPORT QIcon stepFile();

GT_GUI_EXPORT QIcon stepFile16();

GT_GUI_EXPORT QIcon stepFile24();

GT_GUI_EXPORT QIcon table();

GT_GUI_EXPORT QIcon table16();

GT_GUI_EXPORT QIcon python16();

GT_GUI_EXPORT QIcon performanceModel();

GT_GUI_EXPORT QIcon reload();

GT_GUI_EXPORT QIcon ruler();

GT_GUI_EXPORT QIcon textNote24();

GT_GUI_EXPORT QIcon fileIn();

GT_GUI_EXPORT QIcon fileIn16();

GT_GUI_EXPORT QIcon fileIn24();

GT_GUI_EXPORT QIcon bezierCurve();

GT_GUI_EXPORT QIcon polyCurve();

GT_GUI_EXPORT QIcon simpleCurve();

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

GT_GUI_EXPORT QIcon dataBaseArrowUp();

GT_GUI_EXPORT QIcon dataBaseArrowUp16();

GT_GUI_EXPORT QIcon dataBaseArrowUp24();

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

GT_GUI_EXPORT QIcon brush();

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

GT_GUI_EXPORT QPixmap jumpTo();

GT_GUI_EXPORT QPixmap downloaderLogo();

GT_GUI_EXPORT QPixmap question24();

GT_GUI_EXPORT QPixmap printHeadline();

} // namespace pixmap

} // namespace gui

} // namespace gt

#endif // GTICONS_H
