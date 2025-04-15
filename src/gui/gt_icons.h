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

#include <gt_logging.h>
#include <gt_svgiconengine.h>

#include <QIcon>
#include <QUrl>

namespace gt
{
namespace gui
{


class Icon
{
public:
    Icon() = default;

    explicit Icon(const QString& path)
        : m_icon(path), m_path(path) {}


    // Construct from QIconEngine* (no automatic path unless known)
    explicit Icon(GtSvgIconEngine* engine, const QString& path)
        : m_icon(engine), m_path(path) {}

    // Implicit conversion to QIcon
    operator const QIcon&() const { return m_icon; }
    operator QVariant() const { return m_icon; }

    // Optional explicit getter
    const QIcon& icon() const { return m_icon; }

    const QString& path() const { return m_path; }
    QUrl url() const { return QUrl(m_path); }

    [[deprecated ("Use gt::gui::Icon::path instead")]]
    const QString& name() const
    {
        return path();
    }

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
    bool operator==(const Icon& other) const {
        return m_path == other.m_path; // or compare m_icon if needed
    }

    bool operator!=(const Icon& other) const {
        return !(*this == other);
    }

    void paint(QPainter *painter, const QRect &rect,
               Qt::Alignment alignment = Qt::AlignCenter,
               QIcon::Mode mode = QIcon::Normal,
               QIcon::State state = QIcon::Off) const
    {
        m_icon.paint(painter, rect, alignment, mode, state);
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
GT_GUI_EXPORT Icon getIcon(const QString& iconPath);

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
GT_GUI_EXPORT QIcon colorize(const gt::gui::Icon& icon, SvgColorData colorData);

/**
 *  @brief Overload. Accepts two function pointers for the active and
 *  disabled color.
 *  NOTE: Prefere using SvgColorData
 *  @param icon Icon to colorize.
 *  @param getActiveColor Color function pointer for an active/normal icon.
 *  @param getDisabledColor Color function pointer for a disabled icon.
 * @return Icon
 */
GT_GUI_EXPORT QIcon colorize(const gt::gui::Icon& icon,
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

GT_GUI_EXPORT Icon add();
[[deprecated("Use add instead")]]
GT_GUI_EXPORT Icon add16();
[[deprecated("Use projectAdd instead")]]
GT_GUI_EXPORT Icon addProject();

GT_GUI_EXPORT Icon application();
GT_GUI_EXPORT Icon applicationSettings();
GT_GUI_EXPORT Icon applicationVar();

GT_GUI_EXPORT Icon arrowDown();
GT_GUI_EXPORT Icon arrowUp();

GT_GUI_EXPORT Icon arrowRight();
[[deprecated("Use arrowRight instead")]]
GT_GUI_EXPORT Icon arrowRightBlue();
GT_GUI_EXPORT Icon arrowLeft();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT Icon arrowDownBlue();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT Icon arrowDownBlueAll();
[[deprecated("Use arrowUp instead")]]
GT_GUI_EXPORT Icon arrowUpBlueAll();

GT_GUI_EXPORT Icon backspace();
GT_GUI_EXPORT Icon backspaceFlipped();

GT_GUI_EXPORT Icon binaryTree();

GT_GUI_EXPORT Icon blades();

GT_GUI_EXPORT Icon brush();

GT_GUI_EXPORT Icon bug();
GT_GUI_EXPORT Icon bugColorized();
GT_GUI_EXPORT Icon bugPlay();

GT_GUI_EXPORT Icon bulb();

GT_GUI_EXPORT Icon calculator();
[[deprecated("Use calculator instead")]]
GT_GUI_EXPORT Icon calculator16();

GT_GUI_EXPORT Icon cancel();

GT_GUI_EXPORT Icon chain();
[[deprecated("Use chain instead")]]
GT_GUI_EXPORT Icon chain16();
GT_GUI_EXPORT Icon chainOff();

GT_GUI_EXPORT Icon chartBar();
GT_GUI_EXPORT Icon chartBell();
GT_GUI_EXPORT Icon chartCurve();
GT_GUI_EXPORT Icon chartLine();
GT_GUI_EXPORT Icon chartScatter();
GT_GUI_EXPORT Icon chartXY();

GT_GUI_EXPORT Icon check();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT Icon check16();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT Icon check24();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT Icon checkSmall16();

GT_GUI_EXPORT Icon clear();
[[deprecated("Use clear instead")]]
GT_GUI_EXPORT Icon clear16();
[[deprecated("Use backspaceFlipped instead")]]
GT_GUI_EXPORT Icon clear2_16();

GT_GUI_EXPORT Icon clone();
[[deprecated("Use clone instead")]]
GT_GUI_EXPORT Icon clone16();

[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT Icon closeProject();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT Icon closeProject16();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT Icon closeProject24();

[[deprecated("Use projectClosed instead")]]
GT_GUI_EXPORT Icon closedProject16();

GT_GUI_EXPORT Icon close();

GT_GUI_EXPORT Icon cog();
GT_GUI_EXPORT Icon cogPlus();

GT_GUI_EXPORT Icon collapsed();
GT_GUI_EXPORT Icon collapsedColorized();
GT_GUI_EXPORT Icon uncollapsed();
GT_GUI_EXPORT Icon uncollapsedColorized();

GT_GUI_EXPORT Icon collection();
[[deprecated("Use collection instead")]]
GT_GUI_EXPORT Icon collection16();

GT_GUI_EXPORT Icon comment();

GT_GUI_EXPORT Icon config();
[[deprecated("Use config instead")]]
GT_GUI_EXPORT Icon config16();

GT_GUI_EXPORT Icon connection();
GT_GUI_EXPORT Icon connection16();

GT_GUI_EXPORT Icon convergence();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT Icon convergence16();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT Icon convergence24();

GT_GUI_EXPORT Icon copy();
[[deprecated("Use copy instead")]]
GT_GUI_EXPORT Icon copy16();

GT_GUI_EXPORT Icon cross();
[[deprecated("Use cross instead")]]
GT_GUI_EXPORT Icon cross16();

GT_GUI_EXPORT Icon cursor();
GT_GUI_EXPORT Icon cursorMove();

GT_GUI_EXPORT Icon cut();
[[deprecated("Use cut instead")]]
GT_GUI_EXPORT Icon cut16();

GT_GUI_EXPORT Icon data();
[[deprecated("Use data instead")]]
GT_GUI_EXPORT Icon data16();
GT_GUI_EXPORT Icon dataArrowUp();
GT_GUI_EXPORT Icon dataEye();
GT_GUI_EXPORT Icon dataLock();
GT_GUI_EXPORT Icon dataSingle();

GT_GUI_EXPORT Icon delete_();
[[deprecated("Use delete_ instead")]]
GT_GUI_EXPORT Icon delete16();

GT_GUI_EXPORT Icon disk();
GT_GUI_EXPORT Icon diskGradient();

[[deprecated("Use projectDelete instead")]]
GT_GUI_EXPORT Icon deleteProject16();

GT_GUI_EXPORT Icon devices();

GT_GUI_EXPORT Icon dock();

GT_GUI_EXPORT Icon dots();

GT_GUI_EXPORT Icon download();

GT_GUI_EXPORT Icon duplicate();

[[deprecated("Use object instead")]]
GT_GUI_EXPORT Icon empty16();

GT_GUI_EXPORT Icon error();
GT_GUI_EXPORT Icon errorColorized();
[[deprecated("Use error instead")]]
GT_GUI_EXPORT Icon error16();

GT_GUI_EXPORT Icon examples();
[[deprecated("Use examples instead")]]
GT_GUI_EXPORT Icon examples16();

GT_GUI_EXPORT Icon exclamationmark();
[[deprecated("Use exclamationmark instead")]]
GT_GUI_EXPORT Icon exclamation16();

GT_GUI_EXPORT Icon export_();
[[deprecated("Use export_ instead")]]
GT_GUI_EXPORT Icon export16();

GT_GUI_EXPORT Icon eye();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT Icon eye16();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT Icon eye24();
GT_GUI_EXPORT Icon eyeOff();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT Icon eyeOff16();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT Icon eyeOff24();

GT_GUI_EXPORT Icon fatal();
GT_GUI_EXPORT Icon fatalColorized();
[[deprecated("Use fatal instead")]]
GT_GUI_EXPORT Icon fatal16();

GT_GUI_EXPORT Icon file();
[[deprecated("Use file instead")]]
GT_GUI_EXPORT Icon file16();
GT_GUI_EXPORT Icon fileAdd();
GT_GUI_EXPORT Icon fileClock();
GT_GUI_EXPORT Icon fileCode();
GT_GUI_EXPORT Icon fileDelete();
GT_GUI_EXPORT Icon fileDoc();
GT_GUI_EXPORT Icon fileEdit();
GT_GUI_EXPORT Icon fileEye();
GT_GUI_EXPORT Icon fileImport();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT Icon fileStep();

GT_GUI_EXPORT Icon folder();
[[deprecated("Use folder instead")]]
GT_GUI_EXPORT Icon folder16();
GT_GUI_EXPORT Icon folderAdd();
GT_GUI_EXPORT Icon folderEdit();
GT_GUI_EXPORT Icon folderEye();
GT_GUI_EXPORT Icon folderOpen();
GT_GUI_EXPORT Icon folderSearch();

GT_GUI_EXPORT Icon font();

GT_GUI_EXPORT Icon gas();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT Icon gas16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT Icon gas24();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT Icon gasTypes();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT Icon gasTypes16();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT Icon gasTypes24();

GT_GUI_EXPORT Icon global();

GT_GUI_EXPORT Icon grid();
GT_GUI_EXPORT Icon gridSnap();

GT_GUI_EXPORT Icon help();

GT_GUI_EXPORT Icon hdf5();

GT_GUI_EXPORT Icon histogram();
[[deprecated("Use histogram() instead")]]
GT_GUI_EXPORT Icon histogram16();

GT_GUI_EXPORT Icon history();
[[deprecated("Use history() instead")]]
GT_GUI_EXPORT Icon history16();

GT_GUI_EXPORT Icon home();
GT_GUI_EXPORT Icon homeAccount();

GT_GUI_EXPORT Icon import();
[[deprecated("Use import() instead")]]
GT_GUI_EXPORT Icon import16();

[[deprecated("Use objectInvalid instead")]]
GT_GUI_EXPORT Icon inconsistentProject16();

GT_GUI_EXPORT Icon info();
GT_GUI_EXPORT Icon infoColorized();
GT_GUI_EXPORT Icon info2();
[[deprecated("Use info instead")]]
GT_GUI_EXPORT Icon info16();

[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT Icon infoBlue();
[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT Icon infoBlue16();

GT_GUI_EXPORT Icon input();
GT_GUI_EXPORT Icon inputForm();
[[deprecated("Use input instead")]]
GT_GUI_EXPORT Icon input16();
[[deprecated("Use inputSettings instead")]]
GT_GUI_EXPORT Icon input2();
GT_GUI_EXPORT Icon inputSettings();

GT_GUI_EXPORT Icon jumpTo();

GT_GUI_EXPORT Icon json();

GT_GUI_EXPORT Icon label();

GT_GUI_EXPORT Icon language();

GT_GUI_EXPORT Icon layers();

GT_GUI_EXPORT Icon list();
[[deprecated("Use list instead")]]
GT_GUI_EXPORT Icon list16();
GT_GUI_EXPORT Icon listFormatted();

GT_GUI_EXPORT Icon log();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT Icon log16();

GT_GUI_EXPORT Icon login();
[[deprecated("Use login instead")]]
GT_GUI_EXPORT Icon login16();

GT_GUI_EXPORT Icon loop();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT Icon loop16();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT Icon loop2_16();

GT_GUI_EXPORT Icon magnet();

GT_GUI_EXPORT Icon mass();

GT_GUI_EXPORT Icon mathDivision();
GT_GUI_EXPORT Icon mathExponent();
GT_GUI_EXPORT Icon mathMinus();
GT_GUI_EXPORT Icon mathMultiplication();
GT_GUI_EXPORT Icon mathPlus();
GT_GUI_EXPORT Icon mathRoot();

GT_GUI_EXPORT Icon minimize();

GT_GUI_EXPORT Icon network();
[[deprecated("Use network instead")]]
GT_GUI_EXPORT Icon network16();

GT_GUI_EXPORT Icon noteEdit();

GT_GUI_EXPORT Icon notificationUnread();

GT_GUI_EXPORT Icon objectCombustor();
GT_GUI_EXPORT Icon objectEmpty();
GT_GUI_EXPORT Icon objectEngine();
GT_GUI_EXPORT Icon objectFreestyleComponent();
GT_GUI_EXPORT Icon objectInvalid();
GT_GUI_EXPORT Icon objectUnknown();
GT_GUI_EXPORT Icon objectTurbine();
GT_GUI_EXPORT Icon objectCompressor();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT Icon objectUnkown();

GT_GUI_EXPORT Icon open();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT Icon open16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT Icon open2();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT Icon open2_16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT Icon open2_24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT Icon openProject16();

GT_GUI_EXPORT Icon palette();

GT_GUI_EXPORT Icon paramStudy();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT Icon paramStudy16();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT Icon paramStudy24();

GT_GUI_EXPORT Icon paste();
[[deprecated("Use paste instead")]]
GT_GUI_EXPORT Icon paste16();

GT_GUI_EXPORT Icon pause();

GT_GUI_EXPORT Icon pdf();

GT_GUI_EXPORT Icon perfDesignPoint();
GT_GUI_EXPORT Icon perfMap();
GT_GUI_EXPORT Icon perfMapExport();
GT_GUI_EXPORT Icon perfNoMap();
GT_GUI_EXPORT Icon perfModel();
GT_GUI_EXPORT Icon perfOperatingPoint();
GT_GUI_EXPORT Icon perfStageMap();
GT_GUI_EXPORT Icon perfSpeedLine();
GT_GUI_EXPORT Icon perfTsDiagram();
GT_GUI_EXPORT Icon perfWorkingLine();

GT_GUI_EXPORT Icon perspectives();
[[deprecated("Use perspectives instead")]]
GT_GUI_EXPORT Icon perspectives16();
GT_GUI_EXPORT Icon perspectivesEdit();

GT_GUI_EXPORT Icon play();

GT_GUI_EXPORT Icon plugin();
[[deprecated("Use plugin instead")]]
GT_GUI_EXPORT Icon plugin16();
GT_GUI_EXPORT Icon pluginSettings();

GT_GUI_EXPORT Icon print();
[[deprecated("Use print instead")]]
GT_GUI_EXPORT Icon printer();

GT_GUI_EXPORT Icon process();
[[deprecated("Use process instead")]]
GT_GUI_EXPORT Icon process16();
GT_GUI_EXPORT Icon processAdd();
GT_GUI_EXPORT Icon processRun();

[[deprecated("Use warning instead")]]
GT_GUI_EXPORT Icon processFailed16();

GT_GUI_EXPORT Icon projectAdd();
GT_GUI_EXPORT Icon projectClose();
GT_GUI_EXPORT Icon projectClosed();
GT_GUI_EXPORT Icon projectDelete();
GT_GUI_EXPORT Icon projectImport();
GT_GUI_EXPORT Icon projectOpen();
GT_GUI_EXPORT Icon projectRemove();

GT_GUI_EXPORT Icon property();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT Icon property16();

GT_GUI_EXPORT Icon prp();
[[deprecated("Use prp instead")]]
GT_GUI_EXPORT Icon prp16();

GT_GUI_EXPORT Icon puzzle();

GT_GUI_EXPORT Icon python();
[[deprecated("Use python instead")]]
GT_GUI_EXPORT Icon python16();

GT_GUI_EXPORT Icon questionmark();
[[deprecated("Use questionmark instead")]]
GT_GUI_EXPORT Icon question16();

GT_GUI_EXPORT Icon queue();
[[deprecated("Use queue instead")]]
GT_GUI_EXPORT Icon queue16();
GT_GUI_EXPORT Icon queueAdd();

GT_GUI_EXPORT Icon redo();
[[deprecated("Use redo instead")]]
GT_GUI_EXPORT Icon redo24();

GT_GUI_EXPORT Icon reload();

GT_GUI_EXPORT Icon remove();

GT_GUI_EXPORT Icon rename();

GT_GUI_EXPORT Icon repeat();

GT_GUI_EXPORT Icon resize();

GT_GUI_EXPORT Icon results();
[[deprecated("Use results instead")]]
GT_GUI_EXPORT Icon results16();

GT_GUI_EXPORT Icon revert();
[[deprecated("Use revert instead")]]
GT_GUI_EXPORT Icon restore16();

GT_GUI_EXPORT Icon ruler();

[[deprecated("Use processRun instead")]]
GT_GUI_EXPORT Icon runProcess16();

GT_GUI_EXPORT Icon save();
[[deprecated("Use save instead")]]
GT_GUI_EXPORT Icon saveProject16();

GT_GUI_EXPORT Icon schedules();
GT_GUI_EXPORT Icon schedules2();

GT_GUI_EXPORT Icon search();
[[deprecated("Use search instead")]]
GT_GUI_EXPORT Icon search16();

GT_GUI_EXPORT Icon select();

GT_GUI_EXPORT Icon server();

GT_GUI_EXPORT Icon session();
[[deprecated("Use session instead")]]
GT_GUI_EXPORT Icon session16();

GT_GUI_EXPORT Icon showText();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT Icon showText16();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT Icon showText24();

GT_GUI_EXPORT Icon skip();
[[deprecated("Use skip instead")]]
GT_GUI_EXPORT Icon skip16();
GT_GUI_EXPORT Icon unskip();

GT_GUI_EXPORT Icon sleep();
[[deprecated("Use sleep instead")]]
GT_GUI_EXPORT Icon sleep16();
GT_GUI_EXPORT Icon sleepOff();

GT_GUI_EXPORT Icon square();

GT_GUI_EXPORT Icon sort();
GT_GUI_EXPORT Icon sortAsc();
GT_GUI_EXPORT Icon sortDesc();

GT_GUI_EXPORT Icon stop();
[[deprecated("Use stop instead")]]
GT_GUI_EXPORT Icon stop16();

GT_GUI_EXPORT Icon stepFile();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT Icon stepFile16();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT Icon stepFile24();

GT_GUI_EXPORT Icon stretch();

GT_GUI_EXPORT Icon swap();

GT_GUI_EXPORT Icon sync();

GT_GUI_EXPORT Icon table();
[[deprecated("Use table instead")]]
GT_GUI_EXPORT Icon table16();

GT_GUI_EXPORT Icon text();

GT_GUI_EXPORT Icon timer();
GT_GUI_EXPORT Icon timerSand();

GT_GUI_EXPORT Icon trace();
GT_GUI_EXPORT Icon traceColorized();
[[deprecated("Use trace instead")]]
GT_GUI_EXPORT Icon trace16();

GT_GUI_EXPORT Icon tree();

GT_GUI_EXPORT Icon triangleDown();
GT_GUI_EXPORT Icon triangleLeft();
GT_GUI_EXPORT Icon triangleRight();
GT_GUI_EXPORT Icon triangleRightGroup();
GT_GUI_EXPORT Icon triangleUp();
GT_GUI_EXPORT Icon triangleSmallDown();
GT_GUI_EXPORT Icon triangleSmallLeft();
GT_GUI_EXPORT Icon triangleSmallRight();
GT_GUI_EXPORT Icon triangleSmallUp();

[[deprecated("Use perfTsDiagram instead")]]
GT_GUI_EXPORT Icon tsDiagram16();

[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT Icon turbine();
[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT Icon turbine16();

GT_GUI_EXPORT Icon undo();
[[deprecated("Use undo instead")]]
GT_GUI_EXPORT Icon undo24();

GT_GUI_EXPORT Icon update();
[[deprecated("Use update instead")]]
GT_GUI_EXPORT Icon update16();

GT_GUI_EXPORT Icon upload();

GT_GUI_EXPORT Icon variable();

GT_GUI_EXPORT Icon vectorBezier();
GT_GUI_EXPORT Icon vectorBezier2();
GT_GUI_EXPORT Icon vectorCurve();
GT_GUI_EXPORT Icon vectorLine();
GT_GUI_EXPORT Icon vectorPoint();
GT_GUI_EXPORT Icon vectorPointEdit();
GT_GUI_EXPORT Icon vectorPoly();
GT_GUI_EXPORT Icon vectorSimple();
GT_GUI_EXPORT Icon vectorSquare();
GT_GUI_EXPORT Icon vectorSquareEdit();
GT_GUI_EXPORT Icon vectorTriangle();

GT_GUI_EXPORT Icon warning();
GT_GUI_EXPORT Icon warningColorized();
[[deprecated("Use warning instead")]]
GT_GUI_EXPORT Icon warning16();

GT_GUI_EXPORT Icon web();

GT_GUI_EXPORT Icon xml();

GT_GUI_EXPORT Icon zoomMinus();
GT_GUI_EXPORT Icon zoomPlus();
[[deprecated("Use zoomPlus/zoomMinus instead")]]
GT_GUI_EXPORT Icon zoom16();



/** DEPRECATED **/
[[deprecated("Use objectEmpty instead")]]
GT_GUI_EXPORT Icon emptyElement();

[[deprecated("Use timerSand instead")]]
GT_GUI_EXPORT Icon inProgress16();

[[deprecated("Use timer instead")]]
GT_GUI_EXPORT Icon stopRequest16();

[[deprecated("Use applicationVar instead")]]
GT_GUI_EXPORT Icon pathSettings();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT Icon frame();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT Icon unknown();

[[deprecated("Use close instead")]]
GT_GUI_EXPORT Icon close24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT Icon components16();

[[deprecated("Use chartLine instead")]]
GT_GUI_EXPORT Icon post();

[[deprecated("Use label instead")]]
GT_GUI_EXPORT Icon labels();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT Icon stack();

[[deprecated("Use pdf instead")]]
GT_GUI_EXPORT Icon printPDF();

[[deprecated("Use update instead")]]
GT_GUI_EXPORT Icon upgradeProjectData();

[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT Icon move();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT Icon move16();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT Icon move24();

[[deprecated("Use mathPlus instead")]]
GT_GUI_EXPORT Icon plus();
[[deprecated("Use mathMinus instead")]]
GT_GUI_EXPORT Icon minus();
[[deprecated("Use mathMultiplication instead")]]
GT_GUI_EXPORT Icon multiply();
[[deprecated("Use mathDivision instead")]]
GT_GUI_EXPORT Icon divide();
[[deprecated("Use mathRoot instead")]]
GT_GUI_EXPORT Icon squareRoot();

[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT Icon fileIn();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT Icon fileIn16();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT Icon fileIn24();

[[deprecated("Use text instead")]]
GT_GUI_EXPORT Icon textNote24();

[[deprecated("Use vectorTriangle instead")]]
GT_GUI_EXPORT Icon polyCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT Icon bezierCurve();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT Icon simpleCurve();

[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT Icon dataBaseArrowUp();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT Icon dataBaseArrowUp16();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT Icon dataBaseArrowUp24();

[[deprecated("Use noteEdit instead")]]
GT_GUI_EXPORT Icon editPen();

[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT Icon addElement16();
[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT Icon addProcess16();

[[deprecated("use download instead")]]
GT_GUI_EXPORT Icon dropdown();

[[deprecated("Use dataSingle instead")]]
GT_GUI_EXPORT Icon globals16();

[[deprecated("Use chartXY instead")]]
GT_GUI_EXPORT Icon xyPlot16();

[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT Icon engine();
[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT Icon engine16();

[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT Icon combustor();
[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT Icon combustor16();

[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT Icon compressor();
[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT Icon compressor16();

[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT Icon speedLine();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT Icon speedLine16();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT Icon speedLine24();

[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT Icon performanceOffDesign();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT Icon performanceOffDesign16();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT Icon performanceOffDesign24();

[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT Icon map();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT Icon map16();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT Icon map24();

[[deprecated("Use perfNoMap instead")]]
GT_GUI_EXPORT Icon noMap();

[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT Icon exportMap();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT Icon exportMap16();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT Icon exportMap24();

[[deprecated("Use letter::d instead")]]
GT_GUI_EXPORT Icon double16();
[[deprecated("Use letter::i instead")]]
GT_GUI_EXPORT Icon int16();
[[deprecated("Use letter::o instead")]]
GT_GUI_EXPORT Icon o16();
[[deprecated("Use letter::s instead")]]
GT_GUI_EXPORT Icon string16();

/** OTHER **/
[[deprecated("Use perfModel instead")]]
GT_GUI_EXPORT Icon performanceModel();

[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT Icon performanceDesign();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT Icon performanceDesign16();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT Icon performanceDesign24();

GT_GUI_EXPORT Icon engineInstallation();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT Icon engineInstallation16();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT Icon engineInstallation24();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT Icon engineInstallation32();

GT_GUI_EXPORT Icon carpetPlot();

GT_GUI_EXPORT Icon bleedInPort();
GT_GUI_EXPORT Icon bleedInPort16();
GT_GUI_EXPORT Icon bleedInPort24();

GT_GUI_EXPORT Icon bleedOutPort();
GT_GUI_EXPORT Icon bleedOutPort16();
GT_GUI_EXPORT Icon bleedOutPort24();

GT_GUI_EXPORT Icon bleedPortGroup();
GT_GUI_EXPORT Icon bleedPortGroup16();
GT_GUI_EXPORT Icon bleedPortGroup24();

GT_GUI_EXPORT Icon fluidPort();
GT_GUI_EXPORT Icon fluidPort16();
GT_GUI_EXPORT Icon fluidPort24();

GT_GUI_EXPORT Icon fluidPortGroup();
GT_GUI_EXPORT Icon fluidPortGroup16();
GT_GUI_EXPORT Icon fluidPortGroup24();

GT_GUI_EXPORT Icon shaftPort();
GT_GUI_EXPORT Icon shaftPort16();
GT_GUI_EXPORT Icon shaftPort24();

GT_GUI_EXPORT QIcon processRunningIcon(int progress);

/// namespace for letter icons (standard is blue)
namespace letter {

GT_GUI_EXPORT Icon a();
GT_GUI_EXPORT Icon aSmall();

GT_GUI_EXPORT Icon b();
GT_GUI_EXPORT Icon bSmall();

GT_GUI_EXPORT Icon c();
GT_GUI_EXPORT Icon cSmall();

GT_GUI_EXPORT Icon d();
GT_GUI_EXPORT Icon dSmall();

GT_GUI_EXPORT Icon e();
GT_GUI_EXPORT Icon eSmall();
GT_GUI_EXPORT Icon eColorized();
GT_GUI_EXPORT Icon eSmallColorized();
[[deprecated("Use eColorized instead")]]
GT_GUI_EXPORT Icon eBrown();
[[deprecated("Use eSmallColorized instead")]]
GT_GUI_EXPORT Icon eBrownSmall();

GT_GUI_EXPORT Icon f();
GT_GUI_EXPORT Icon fSmall();

GT_GUI_EXPORT Icon g();
GT_GUI_EXPORT Icon gSmall();

GT_GUI_EXPORT Icon h();
GT_GUI_EXPORT Icon hSmall();

GT_GUI_EXPORT Icon i();
GT_GUI_EXPORT Icon iSmall();

GT_GUI_EXPORT Icon j();
GT_GUI_EXPORT Icon jSmall();

GT_GUI_EXPORT Icon k();
GT_GUI_EXPORT Icon kSmall();

GT_GUI_EXPORT Icon l();
GT_GUI_EXPORT Icon lSmall();

GT_GUI_EXPORT Icon m();
GT_GUI_EXPORT Icon mSmall();

GT_GUI_EXPORT Icon n();
GT_GUI_EXPORT Icon nSmall();

GT_GUI_EXPORT Icon o();
GT_GUI_EXPORT Icon oSmall();

GT_GUI_EXPORT Icon p();
GT_GUI_EXPORT Icon pSmall();

GT_GUI_EXPORT Icon q();
GT_GUI_EXPORT Icon qSmall();

GT_GUI_EXPORT Icon r();
GT_GUI_EXPORT Icon rSmall();

GT_GUI_EXPORT Icon s();
GT_GUI_EXPORT Icon sSmall();

GT_GUI_EXPORT Icon t();
GT_GUI_EXPORT Icon tSmall();

GT_GUI_EXPORT Icon u();
GT_GUI_EXPORT Icon uSmall();

GT_GUI_EXPORT Icon v();
GT_GUI_EXPORT Icon vSmall();

GT_GUI_EXPORT Icon w();
GT_GUI_EXPORT Icon wSmall();

GT_GUI_EXPORT Icon x();
GT_GUI_EXPORT Icon xSmall();

GT_GUI_EXPORT Icon y();
GT_GUI_EXPORT Icon ySmall();

GT_GUI_EXPORT Icon z();
GT_GUI_EXPORT Icon zSmall();

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

Q_DECLARE_METATYPE(gt::gui::Icon)

#endif // GTICONS_H
