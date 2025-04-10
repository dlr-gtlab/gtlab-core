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
#include <QUrl>

#include <gt_logging.h>

namespace gt
{
namespace gui
{


class GtIcon
{
public:
    GtIcon() = default;

    explicit GtIcon(const QString& path)
        : m_icon(path), m_path(path) {}


    // Construct from QIconEngine* (no automatic path unless known)
    explicit GtIcon(QIconEngine* engine, const QString& path)
        : m_icon(engine), m_path(path) {}

    // Implicit conversion to QIcon
    operator const QIcon&() const { return m_icon; }
    operator QVariant() const { return m_icon; }

    // Optional explicit getter
    const QIcon& icon() const { return m_icon; }

    const QString& path() const { return m_path; }
    QUrl url() const { return QUrl(m_path); }

    QPixmap pixmap(const QSize &size, QIcon::Mode mode = QIcon::Normal,
                   QIcon::State state = QIcon::Off) const
    {
        return m_icon.pixmap(size, mode, state);
    }

    QPixmap pixmap(int w, int h, QIcon::Mode mode = QIcon::Normal,
                   QIcon::State state = QIcon::Off) const
    {
        return m_icon.pixmap(w, h, mode, state);
    }

    bool isNull() const { return m_icon.isNull(); }

    // Optional equality comparison
    bool operator==(const GtIcon& other) const {
        return m_path == other.m_path; // or compare m_icon if needed
    }

    bool operator!=(const GtIcon& other) const {
        return !(*this == other);
    }

private:
    QIcon m_icon;
    QString m_path;
};

/**
 *  @brief Creates a new icon from a resource file.
 *  Svg files (ending with .svg) will be colored according to the theme used.
 *  @param iconPath Relative or custom resource path to a icon file.
 */
GT_GUI_EXPORT GtIcon getIcon(const QString& iconPath);

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
GT_GUI_EXPORT QIcon colorize(const gt::gui::GtIcon& icon, SvgColorData colorData);

/**
 *  @brief Overload. Accepts two function pointers for the active and
 *  disabled color.
 *  NOTE: Prefere using SvgColorData
 *  @param icon Icon to colorize.
 *  @param getActiveColor Color function pointer for an active/normal icon.
 *  @param getDisabledColor Color function pointer for a disabled icon.
 * @return Icon
 */
GT_GUI_EXPORT QIcon colorize(const gt::gui::GtIcon& icon,
                             ColorFunctionPtr getActiveColor,
                             ColorFunctionPtr getDisabledColor = {});

namespace icon
{

/**
 * @brief Returns the url of an icon
 *
 * Usage:
 *
 * auto iconurl = icon::url(icon::copy);
 *
 * @return
 */
template <typename IconGetter>
QUrl url(IconGetter&& getIcon)
{
    return "qrc:/icons/" + getIcon().path();
}

/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

GT_GUI_EXPORT GtIcon add();
[[deprecated("Use add instead")]]
GT_GUI_EXPORT GtIcon add16();
[[deprecated("Use projectAdd instead")]]
GT_GUI_EXPORT GtIcon addProject();

GT_GUI_EXPORT GtIcon application();
GT_GUI_EXPORT GtIcon applicationSettings();
GT_GUI_EXPORT GtIcon applicationVar();

GT_GUI_EXPORT GtIcon arrowDown();
GT_GUI_EXPORT GtIcon arrowUp();

GT_GUI_EXPORT GtIcon arrowRight();
[[deprecated("Use arrowRight instead")]]
GT_GUI_EXPORT GtIcon arrowRightBlue();
GT_GUI_EXPORT GtIcon arrowLeft();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT GtIcon arrowDownBlue();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT GtIcon arrowDownBlueAll();
[[deprecated("Use arrowUp instead")]]
GT_GUI_EXPORT GtIcon arrowUpBlueAll();

GT_GUI_EXPORT GtIcon backspace();
GT_GUI_EXPORT GtIcon backspaceFlipped();

GT_GUI_EXPORT GtIcon binaryTree();

GT_GUI_EXPORT GtIcon blades();

GT_GUI_EXPORT GtIcon brush();

GT_GUI_EXPORT GtIcon bug();
GT_GUI_EXPORT GtIcon bugColorized();
GT_GUI_EXPORT GtIcon bugPlay();

GT_GUI_EXPORT GtIcon bulb();

GT_GUI_EXPORT GtIcon calculator();
[[deprecated("Use calculator instead")]]
GT_GUI_EXPORT GtIcon calculator16();

GT_GUI_EXPORT GtIcon cancel();

GT_GUI_EXPORT GtIcon chain();
[[deprecated("Use chain instead")]]
GT_GUI_EXPORT GtIcon chain16();
GT_GUI_EXPORT GtIcon chainOff();

GT_GUI_EXPORT GtIcon chartBar();
GT_GUI_EXPORT GtIcon chartBell();
GT_GUI_EXPORT GtIcon chartCurve();
GT_GUI_EXPORT GtIcon chartLine();
GT_GUI_EXPORT GtIcon chartScatter();
GT_GUI_EXPORT GtIcon chartXY();

GT_GUI_EXPORT GtIcon check();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT GtIcon check16();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT GtIcon check24();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT GtIcon checkSmall16();

GT_GUI_EXPORT GtIcon clear();
[[deprecated("Use clear instead")]]
GT_GUI_EXPORT GtIcon clear16();
[[deprecated("Use backspaceFlipped instead")]]
GT_GUI_EXPORT GtIcon clear2_16();

GT_GUI_EXPORT GtIcon clone();
[[deprecated("Use clone instead")]]
GT_GUI_EXPORT GtIcon clone16();

[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT GtIcon closeProject();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT GtIcon closeProject16();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT GtIcon closeProject24();

[[deprecated("Use projectClosed instead")]]
GT_GUI_EXPORT GtIcon closedProject16();

GT_GUI_EXPORT GtIcon close();

GT_GUI_EXPORT GtIcon cog();
GT_GUI_EXPORT GtIcon cogPlus();

GT_GUI_EXPORT GtIcon collapsed();
GT_GUI_EXPORT GtIcon collapsedColorized();
GT_GUI_EXPORT GtIcon uncollapsed();
GT_GUI_EXPORT GtIcon uncollapsedColorized();

GT_GUI_EXPORT GtIcon collection();
[[deprecated("Use collection instead")]]
GT_GUI_EXPORT GtIcon collection16();

GT_GUI_EXPORT GtIcon comment();

GT_GUI_EXPORT GtIcon config();
[[deprecated("Use config instead")]]
GT_GUI_EXPORT GtIcon config16();

GT_GUI_EXPORT GtIcon connection();
GT_GUI_EXPORT GtIcon connection16();

GT_GUI_EXPORT GtIcon convergence();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT GtIcon convergence16();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT GtIcon convergence24();

GT_GUI_EXPORT GtIcon copy();
[[deprecated("Use copy instead")]]
GT_GUI_EXPORT GtIcon copy16();

GT_GUI_EXPORT GtIcon cross();
[[deprecated("Use cross instead")]]
GT_GUI_EXPORT GtIcon cross16();

GT_GUI_EXPORT GtIcon cursor();
GT_GUI_EXPORT GtIcon cursorMove();

GT_GUI_EXPORT GtIcon cut();
[[deprecated("Use cut instead")]]
GT_GUI_EXPORT GtIcon cut16();

GT_GUI_EXPORT GtIcon data();
[[deprecated("Use data instead")]]
GT_GUI_EXPORT GtIcon data16();
GT_GUI_EXPORT GtIcon dataArrowUp();
GT_GUI_EXPORT GtIcon dataEye();
GT_GUI_EXPORT GtIcon dataLock();
GT_GUI_EXPORT GtIcon dataSingle();

GT_GUI_EXPORT GtIcon delete_();
[[deprecated("Use delete_ instead")]]
GT_GUI_EXPORT GtIcon delete16();

GT_GUI_EXPORT GtIcon disk();
GT_GUI_EXPORT GtIcon diskGradient();

[[deprecated("Use projectDelete instead")]]
GT_GUI_EXPORT GtIcon deleteProject16();

GT_GUI_EXPORT GtIcon devices();

GT_GUI_EXPORT GtIcon dock();

GT_GUI_EXPORT GtIcon dots();

GT_GUI_EXPORT GtIcon download();

GT_GUI_EXPORT GtIcon duplicate();

[[deprecated("Use object instead")]]
GT_GUI_EXPORT GtIcon empty16();

GT_GUI_EXPORT GtIcon error();
GT_GUI_EXPORT GtIcon errorColorized();
[[deprecated("Use error instead")]]
GT_GUI_EXPORT GtIcon error16();

GT_GUI_EXPORT GtIcon examples();
[[deprecated("Use examples instead")]]
GT_GUI_EXPORT GtIcon examples16();

GT_GUI_EXPORT GtIcon exclamationmark();
[[deprecated("Use exclamationmark instead")]]
GT_GUI_EXPORT GtIcon exclamation16();

GT_GUI_EXPORT GtIcon export_();
[[deprecated("Use export_ instead")]]
GT_GUI_EXPORT GtIcon export16();

GT_GUI_EXPORT GtIcon eye();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT GtIcon eye16();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT GtIcon eye24();
GT_GUI_EXPORT GtIcon eyeOff();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT GtIcon eyeOff16();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT GtIcon eyeOff24();

GT_GUI_EXPORT GtIcon fatal();
GT_GUI_EXPORT GtIcon fatalColorized();
[[deprecated("Use fatal instead")]]
GT_GUI_EXPORT GtIcon fatal16();

GT_GUI_EXPORT GtIcon file();
[[deprecated("Use file instead")]]
GT_GUI_EXPORT GtIcon file16();
GT_GUI_EXPORT GtIcon fileAdd();
GT_GUI_EXPORT GtIcon fileClock();
GT_GUI_EXPORT GtIcon fileCode();
GT_GUI_EXPORT GtIcon fileDelete();
GT_GUI_EXPORT GtIcon fileDoc();
GT_GUI_EXPORT GtIcon fileEdit();
GT_GUI_EXPORT GtIcon fileEye();
GT_GUI_EXPORT GtIcon fileImport();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT GtIcon fileStep();

GT_GUI_EXPORT GtIcon folder();
[[deprecated("Use folder instead")]]
GT_GUI_EXPORT GtIcon folder16();
GT_GUI_EXPORT GtIcon folderAdd();
GT_GUI_EXPORT GtIcon folderEdit();
GT_GUI_EXPORT GtIcon folderEye();
GT_GUI_EXPORT GtIcon folderOpen();
GT_GUI_EXPORT GtIcon folderSearch();

GT_GUI_EXPORT GtIcon font();

GT_GUI_EXPORT GtIcon gas();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT GtIcon gas16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT GtIcon gas24();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT GtIcon gasTypes();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT GtIcon gasTypes16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT GtIcon gasTypes24();

GT_GUI_EXPORT GtIcon global();

GT_GUI_EXPORT GtIcon grid();
GT_GUI_EXPORT GtIcon gridSnap();

GT_GUI_EXPORT GtIcon help();

GT_GUI_EXPORT GtIcon hdf5();

GT_GUI_EXPORT GtIcon histogram();
[[deprecated("Use histogram() instead")]]
GT_GUI_EXPORT GtIcon histogram16();

GT_GUI_EXPORT GtIcon history();
[[deprecated("Use history() instead")]]
GT_GUI_EXPORT GtIcon history16();

GT_GUI_EXPORT GtIcon home();
GT_GUI_EXPORT GtIcon homeAccount();

GT_GUI_EXPORT GtIcon import();
[[deprecated("Use import() instead")]]
GT_GUI_EXPORT GtIcon import16();

[[deprecated("Use objectInvalid instead")]]
GT_GUI_EXPORT GtIcon inconsistentProject16();

GT_GUI_EXPORT GtIcon info();
GT_GUI_EXPORT GtIcon infoColorized();
GT_GUI_EXPORT GtIcon info2();
[[deprecated("Use info instead")]]
GT_GUI_EXPORT GtIcon info16();

[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT GtIcon infoBlue();
[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT GtIcon infoBlue16();

GT_GUI_EXPORT GtIcon input();
GT_GUI_EXPORT GtIcon inputForm();
[[deprecated("Use input instead")]]
GT_GUI_EXPORT GtIcon input16();
[[deprecated("Use inputSettings instead")]]
GT_GUI_EXPORT GtIcon input2();
GT_GUI_EXPORT GtIcon inputSettings();

GT_GUI_EXPORT GtIcon jumpTo();

GT_GUI_EXPORT GtIcon json();

GT_GUI_EXPORT GtIcon label();

GT_GUI_EXPORT GtIcon language();

GT_GUI_EXPORT GtIcon layers();

GT_GUI_EXPORT GtIcon list();
[[deprecated("Use list instead")]]
GT_GUI_EXPORT GtIcon list16();
GT_GUI_EXPORT GtIcon listFormatted();

GT_GUI_EXPORT GtIcon log();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT GtIcon log16();

GT_GUI_EXPORT GtIcon login();
[[deprecated("Use login instead")]]
GT_GUI_EXPORT GtIcon login16();

GT_GUI_EXPORT GtIcon loop();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT GtIcon loop16();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT GtIcon loop2_16();

GT_GUI_EXPORT GtIcon magnet();

GT_GUI_EXPORT GtIcon mass();

GT_GUI_EXPORT GtIcon mathDivision();
GT_GUI_EXPORT GtIcon mathExponent();
GT_GUI_EXPORT GtIcon mathMinus();
GT_GUI_EXPORT GtIcon mathMultiplication();
GT_GUI_EXPORT GtIcon mathPlus();
GT_GUI_EXPORT GtIcon mathRoot();

GT_GUI_EXPORT GtIcon minimize();

GT_GUI_EXPORT GtIcon network();
[[deprecated("Use network instead")]]
GT_GUI_EXPORT GtIcon network16();

GT_GUI_EXPORT GtIcon noteEdit();

GT_GUI_EXPORT GtIcon notificationUnread();

GT_GUI_EXPORT GtIcon objectCombustor();
GT_GUI_EXPORT GtIcon objectEmpty();
GT_GUI_EXPORT GtIcon objectEngine();
GT_GUI_EXPORT GtIcon objectFreestyleComponent();
GT_GUI_EXPORT GtIcon objectInvalid();
GT_GUI_EXPORT GtIcon objectUnknown();
GT_GUI_EXPORT GtIcon objectTurbine();
GT_GUI_EXPORT GtIcon objectCompressor();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT GtIcon objectUnkown();

GT_GUI_EXPORT GtIcon open();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT GtIcon open16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT GtIcon open2();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT GtIcon open2_16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT GtIcon open2_24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT GtIcon openProject16();

GT_GUI_EXPORT GtIcon palette();

GT_GUI_EXPORT GtIcon paramStudy();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT GtIcon paramStudy16();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT GtIcon paramStudy24();

GT_GUI_EXPORT GtIcon paste();
[[deprecated("Use paste instead")]]
GT_GUI_EXPORT GtIcon paste16();

GT_GUI_EXPORT GtIcon pause();

GT_GUI_EXPORT GtIcon pdf();

GT_GUI_EXPORT GtIcon perfDesignPoint();
GT_GUI_EXPORT GtIcon perfMap();
GT_GUI_EXPORT GtIcon perfMapExport();
GT_GUI_EXPORT GtIcon perfNoMap();
GT_GUI_EXPORT GtIcon perfModel();
GT_GUI_EXPORT GtIcon perfOperatingPoint();
GT_GUI_EXPORT GtIcon perfStageMap();
GT_GUI_EXPORT GtIcon perfSpeedLine();
GT_GUI_EXPORT GtIcon perfTsDiagram();
GT_GUI_EXPORT GtIcon perfWorkingLine();

GT_GUI_EXPORT GtIcon perspectives();
[[deprecated("Use perspectives instead")]]
GT_GUI_EXPORT GtIcon perspectives16();
GT_GUI_EXPORT GtIcon perspectivesEdit();

GT_GUI_EXPORT GtIcon play();

GT_GUI_EXPORT GtIcon plugin();
[[deprecated("Use plugin instead")]]
GT_GUI_EXPORT GtIcon plugin16();
GT_GUI_EXPORT GtIcon pluginSettings();

GT_GUI_EXPORT GtIcon print();
[[deprecated("Use print instead")]]
GT_GUI_EXPORT GtIcon printer();

GT_GUI_EXPORT GtIcon process();
[[deprecated("Use process instead")]]
GT_GUI_EXPORT GtIcon process16();
GT_GUI_EXPORT GtIcon processAdd();
GT_GUI_EXPORT GtIcon processRun();

[[deprecated("Use warning instead")]]
GT_GUI_EXPORT GtIcon processFailed16();

GT_GUI_EXPORT GtIcon projectAdd();
GT_GUI_EXPORT GtIcon projectClose();
GT_GUI_EXPORT GtIcon projectClosed();
GT_GUI_EXPORT GtIcon projectDelete();
GT_GUI_EXPORT GtIcon projectImport();
GT_GUI_EXPORT GtIcon projectOpen();
GT_GUI_EXPORT GtIcon projectRemove();

GT_GUI_EXPORT GtIcon property();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT GtIcon property16();

GT_GUI_EXPORT GtIcon prp();
[[deprecated("Use prp instead")]]
GT_GUI_EXPORT GtIcon prp16();

GT_GUI_EXPORT GtIcon puzzle();

GT_GUI_EXPORT GtIcon python();
[[deprecated("Use python instead")]]
GT_GUI_EXPORT GtIcon python16();

GT_GUI_EXPORT GtIcon questionmark();
[[deprecated("Use questionmark instead")]]
GT_GUI_EXPORT GtIcon question16();

GT_GUI_EXPORT GtIcon queue();
[[deprecated("Use queue instead")]]
GT_GUI_EXPORT GtIcon queue16();
GT_GUI_EXPORT GtIcon queueAdd();

GT_GUI_EXPORT GtIcon redo();
[[deprecated("Use redo instead")]]
GT_GUI_EXPORT GtIcon redo24();

GT_GUI_EXPORT GtIcon reload();

GT_GUI_EXPORT GtIcon remove();

GT_GUI_EXPORT GtIcon rename();

GT_GUI_EXPORT GtIcon repeat();

GT_GUI_EXPORT GtIcon resize();

GT_GUI_EXPORT GtIcon results();
[[deprecated("Use results instead")]]
GT_GUI_EXPORT GtIcon results16();

GT_GUI_EXPORT GtIcon revert();
[[deprecated("Use revert instead")]]
GT_GUI_EXPORT GtIcon restore16();

GT_GUI_EXPORT GtIcon ruler();

[[deprecated("Use processRun instead")]]
GT_GUI_EXPORT GtIcon runProcess16();

GT_GUI_EXPORT GtIcon save();
[[deprecated("Use save instead")]]
GT_GUI_EXPORT GtIcon saveProject16();

GT_GUI_EXPORT GtIcon schedules();
GT_GUI_EXPORT GtIcon schedules2();

GT_GUI_EXPORT GtIcon search();
[[deprecated("Use search instead")]]
GT_GUI_EXPORT GtIcon search16();

GT_GUI_EXPORT GtIcon select();

GT_GUI_EXPORT GtIcon server();

GT_GUI_EXPORT GtIcon session();
[[deprecated("Use session instead")]]
GT_GUI_EXPORT GtIcon session16();

GT_GUI_EXPORT GtIcon showText();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT GtIcon showText16();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT GtIcon showText24();

GT_GUI_EXPORT GtIcon skip();
[[deprecated("Use skip instead")]]
GT_GUI_EXPORT GtIcon skip16();
GT_GUI_EXPORT GtIcon unskip();

GT_GUI_EXPORT GtIcon sleep();
[[deprecated("Use sleep instead")]]
GT_GUI_EXPORT GtIcon sleep16();
GT_GUI_EXPORT GtIcon sleepOff();

GT_GUI_EXPORT GtIcon square();

GT_GUI_EXPORT GtIcon sort();
GT_GUI_EXPORT GtIcon sortAsc();
GT_GUI_EXPORT GtIcon sortDesc();

GT_GUI_EXPORT GtIcon stop();
[[deprecated("Use stop instead")]]
GT_GUI_EXPORT GtIcon stop16();

GT_GUI_EXPORT GtIcon stepFile();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT GtIcon stepFile16();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT GtIcon stepFile24();

GT_GUI_EXPORT GtIcon stretch();

GT_GUI_EXPORT GtIcon swap();

GT_GUI_EXPORT GtIcon sync();

GT_GUI_EXPORT GtIcon table();
[[deprecated("Use table instead")]]
GT_GUI_EXPORT GtIcon table16();

GT_GUI_EXPORT GtIcon text();

GT_GUI_EXPORT GtIcon timer();
GT_GUI_EXPORT GtIcon timerSand();

GT_GUI_EXPORT GtIcon trace();
GT_GUI_EXPORT GtIcon traceColorized();
[[deprecated("Use trace instead")]]
GT_GUI_EXPORT GtIcon trace16();

GT_GUI_EXPORT GtIcon tree();

GT_GUI_EXPORT GtIcon triangleDown();
GT_GUI_EXPORT GtIcon triangleLeft();
GT_GUI_EXPORT GtIcon triangleRight();
GT_GUI_EXPORT GtIcon triangleRightGroup();
GT_GUI_EXPORT GtIcon triangleUp();
GT_GUI_EXPORT GtIcon triangleSmallDown();
GT_GUI_EXPORT GtIcon triangleSmallLeft();
GT_GUI_EXPORT GtIcon triangleSmallRight();
GT_GUI_EXPORT GtIcon triangleSmallUp();

[[deprecated("Use perfTsDiagram instead")]]
GT_GUI_EXPORT GtIcon tsDiagram16();

[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT GtIcon turbine();
[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT GtIcon turbine16();

GT_GUI_EXPORT GtIcon undo();
[[deprecated("Use undo instead")]]
GT_GUI_EXPORT GtIcon undo24();

GT_GUI_EXPORT GtIcon update();
[[deprecated("Use update instead")]]
GT_GUI_EXPORT GtIcon update16();

GT_GUI_EXPORT GtIcon upload();

GT_GUI_EXPORT GtIcon variable();

GT_GUI_EXPORT GtIcon vectorBezier();
GT_GUI_EXPORT GtIcon vectorBezier2();
GT_GUI_EXPORT GtIcon vectorCurve();
GT_GUI_EXPORT GtIcon vectorLine();
GT_GUI_EXPORT GtIcon vectorPoint();
GT_GUI_EXPORT GtIcon vectorPointEdit();
GT_GUI_EXPORT GtIcon vectorPoly();
GT_GUI_EXPORT GtIcon vectorSimple();
GT_GUI_EXPORT GtIcon vectorSquare();
GT_GUI_EXPORT GtIcon vectorSquareEdit();
GT_GUI_EXPORT GtIcon vectorTriangle();

GT_GUI_EXPORT GtIcon warning();
GT_GUI_EXPORT GtIcon warningColorized();
[[deprecated("Use warning instead")]]
GT_GUI_EXPORT GtIcon warning16();

GT_GUI_EXPORT GtIcon web();

GT_GUI_EXPORT GtIcon xml();

GT_GUI_EXPORT GtIcon zoomMinus();
GT_GUI_EXPORT GtIcon zoomPlus();
[[deprecated("Use zoomPlus/zoomMinus instead")]]
GT_GUI_EXPORT GtIcon zoom16();



/** DEPRECATED **/
[[deprecated("Use objectEmpty instead")]]
GT_GUI_EXPORT GtIcon emptyElement();

[[deprecated("Use timerSand instead")]]
GT_GUI_EXPORT GtIcon inProgress16();

[[deprecated("Use timer instead")]]
GT_GUI_EXPORT GtIcon stopRequest16();

[[deprecated("Use applicationVar instead")]]
GT_GUI_EXPORT GtIcon pathSettings();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT GtIcon frame();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT GtIcon unknown();

[[deprecated("Use close instead")]]
GT_GUI_EXPORT GtIcon close24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT GtIcon components16();

[[deprecated("Use chartLine instead")]]
GT_GUI_EXPORT GtIcon post();

[[deprecated("Use label instead")]]
GT_GUI_EXPORT GtIcon labels();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT GtIcon stack();

[[deprecated("Use pdf instead")]]
GT_GUI_EXPORT GtIcon printPDF();

[[deprecated("Use update instead")]]
GT_GUI_EXPORT GtIcon upgradeProjectData();

[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT GtIcon move();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT GtIcon move16();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT GtIcon move24();

[[deprecated("Use mathPlus instead")]]
GT_GUI_EXPORT GtIcon plus();
[[deprecated("Use mathMinus instead")]]
GT_GUI_EXPORT GtIcon minus();
[[deprecated("Use mathMultiplication instead")]]
GT_GUI_EXPORT GtIcon multiply();
[[deprecated("Use mathDivision instead")]]
GT_GUI_EXPORT GtIcon divide();
[[deprecated("Use mathRoot instead")]]
GT_GUI_EXPORT GtIcon squareRoot();

[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT GtIcon fileIn();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT GtIcon fileIn16();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT GtIcon fileIn24();

[[deprecated("Use text instead")]]
GT_GUI_EXPORT GtIcon textNote24();

[[deprecated("Use vectorTriangle instead")]]
GT_GUI_EXPORT GtIcon polyCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT GtIcon bezierCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT GtIcon simpleCurve();

[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT GtIcon dataBaseArrowUp();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT GtIcon dataBaseArrowUp16();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT GtIcon dataBaseArrowUp24();

[[deprecated("Use noteEdit instead")]]
GT_GUI_EXPORT GtIcon editPen();

[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT GtIcon addElement16();
[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT GtIcon addProcess16();

[[deprecated("use download instead")]]
GT_GUI_EXPORT GtIcon dropdown();

[[deprecated("Use dataSingle instead")]]
GT_GUI_EXPORT GtIcon globals16();

[[deprecated("Use chartXY instead")]]
GT_GUI_EXPORT GtIcon xyPlot16();

[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT GtIcon engine();
[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT GtIcon engine16();

[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT GtIcon combustor();
[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT GtIcon combustor16();

[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT GtIcon compressor();
[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT GtIcon compressor16();

[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT GtIcon speedLine();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT GtIcon speedLine16();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT GtIcon speedLine24();

[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT GtIcon performanceOffDesign();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT GtIcon performanceOffDesign16();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT GtIcon performanceOffDesign24();

[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT GtIcon map();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT GtIcon map16();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT GtIcon map24();

[[deprecated("Use perfNoMap instead")]]
GT_GUI_EXPORT GtIcon noMap();

[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT GtIcon exportMap();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT GtIcon exportMap16();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT GtIcon exportMap24();

[[deprecated("Use letter::d instead")]]
GT_GUI_EXPORT GtIcon double16();
[[deprecated("Use letter::i instead")]]
GT_GUI_EXPORT GtIcon int16();
[[deprecated("Use letter::o instead")]]
GT_GUI_EXPORT GtIcon o16();
[[deprecated("Use letter::s instead")]]
GT_GUI_EXPORT GtIcon string16();

/** OTHER **/
[[deprecated("Use perfModel instead")]]
GT_GUI_EXPORT GtIcon performanceModel();

[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT GtIcon performanceDesign();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT GtIcon performanceDesign16();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT GtIcon performanceDesign24();

GT_GUI_EXPORT GtIcon engineInstallation();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT GtIcon engineInstallation16();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT GtIcon engineInstallation24();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT GtIcon engineInstallation32();

GT_GUI_EXPORT GtIcon carpetPlot();

GT_GUI_EXPORT GtIcon bleedInPort();
GT_GUI_EXPORT GtIcon bleedInPort16();
GT_GUI_EXPORT GtIcon bleedInPort24();

GT_GUI_EXPORT GtIcon bleedOutPort();
GT_GUI_EXPORT GtIcon bleedOutPort16();
GT_GUI_EXPORT GtIcon bleedOutPort24();

GT_GUI_EXPORT GtIcon bleedPortGroup();
GT_GUI_EXPORT GtIcon bleedPortGroup16();
GT_GUI_EXPORT GtIcon bleedPortGroup24();

GT_GUI_EXPORT GtIcon fluidPort();
GT_GUI_EXPORT GtIcon fluidPort16();
GT_GUI_EXPORT GtIcon fluidPort24();

GT_GUI_EXPORT GtIcon fluidPortGroup();
GT_GUI_EXPORT GtIcon fluidPortGroup16();
GT_GUI_EXPORT GtIcon fluidPortGroup24();

GT_GUI_EXPORT GtIcon shaftPort();
GT_GUI_EXPORT GtIcon shaftPort16();
GT_GUI_EXPORT GtIcon shaftPort24();

GT_GUI_EXPORT QIcon processRunningIcon(int progress);

/// namespace for letter icons (standard is blue)
namespace letter {

GT_GUI_EXPORT GtIcon a();
GT_GUI_EXPORT GtIcon aSmall();

GT_GUI_EXPORT GtIcon b();
GT_GUI_EXPORT GtIcon bSmall();

GT_GUI_EXPORT GtIcon c();
GT_GUI_EXPORT GtIcon cSmall();

GT_GUI_EXPORT GtIcon d();
GT_GUI_EXPORT GtIcon dSmall();

GT_GUI_EXPORT GtIcon e();
GT_GUI_EXPORT GtIcon eSmall();
GT_GUI_EXPORT GtIcon eColorized();
GT_GUI_EXPORT GtIcon eSmallColorized();
[[deprecated("Use eColorized instead")]]
GT_GUI_EXPORT GtIcon eBrown();
[[deprecated("Use eSmallColorized instead")]]
GT_GUI_EXPORT GtIcon eBrownSmall();

GT_GUI_EXPORT GtIcon f();
GT_GUI_EXPORT GtIcon fSmall();

GT_GUI_EXPORT GtIcon g();
GT_GUI_EXPORT GtIcon gSmall();

GT_GUI_EXPORT GtIcon h();
GT_GUI_EXPORT GtIcon hSmall();

GT_GUI_EXPORT GtIcon i();
GT_GUI_EXPORT GtIcon iSmall();

GT_GUI_EXPORT GtIcon j();
GT_GUI_EXPORT GtIcon jSmall();

GT_GUI_EXPORT GtIcon k();
GT_GUI_EXPORT GtIcon kSmall();

GT_GUI_EXPORT GtIcon l();
GT_GUI_EXPORT GtIcon lSmall();

GT_GUI_EXPORT GtIcon m();
GT_GUI_EXPORT GtIcon mSmall();

GT_GUI_EXPORT GtIcon n();
GT_GUI_EXPORT GtIcon nSmall();

GT_GUI_EXPORT GtIcon o();
GT_GUI_EXPORT GtIcon oSmall();

GT_GUI_EXPORT GtIcon p();
GT_GUI_EXPORT GtIcon pSmall();

GT_GUI_EXPORT GtIcon q();
GT_GUI_EXPORT GtIcon qSmall();

GT_GUI_EXPORT GtIcon r();
GT_GUI_EXPORT GtIcon rSmall();

GT_GUI_EXPORT GtIcon s();
GT_GUI_EXPORT GtIcon sSmall();

GT_GUI_EXPORT GtIcon t();
GT_GUI_EXPORT GtIcon tSmall();

GT_GUI_EXPORT GtIcon u();
GT_GUI_EXPORT GtIcon uSmall();

GT_GUI_EXPORT GtIcon v();
GT_GUI_EXPORT GtIcon vSmall();

GT_GUI_EXPORT GtIcon w();
GT_GUI_EXPORT GtIcon wSmall();

GT_GUI_EXPORT GtIcon x();
GT_GUI_EXPORT GtIcon xSmall();

GT_GUI_EXPORT GtIcon y();
GT_GUI_EXPORT GtIcon ySmall();

GT_GUI_EXPORT GtIcon z();
GT_GUI_EXPORT GtIcon zSmall();

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

[[deprecated("Use icon::jumpTo().pixmap(...) instead")]]
GT_GUI_EXPORT QPixmap jumpTo();

GT_GUI_EXPORT QPixmap downloaderLogo();

[[deprecated("Use icon::questionmark().pixmap(...) instead")]]
GT_GUI_EXPORT QPixmap question24();

GT_GUI_EXPORT QPixmap printHeadline();

} // namespace pixmap

} // namespace gui

} // namespace gt

Q_DECLARE_METATYPE(gt::gui::GtIcon)

#endif // GTICONS_H
