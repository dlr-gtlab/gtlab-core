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
GT_GUI_EXPORT QIcon colorize(const Icon& icon, SvgColorData colorData);

/**
 *  @brief Overload. Accepts two function pointers for the active and
 *  disabled color.
 *  NOTE: Prefere using SvgColorData
 *  @param icon Icon to colorize.
 *  @param getActiveColor Color function pointer for an active/normal icon.
 *  @param getDisabledColor Color function pointer for a disabled icon.
 * @return Icon
 */
GT_GUI_EXPORT QIcon colorize(const Icon& icon,
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
    return "qrc" + getIcon().path();
}

/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

GT_GUI_EXPORT Icon add();

GT_GUI_EXPORT Icon application();
GT_GUI_EXPORT Icon applicationSettings();
GT_GUI_EXPORT Icon applicationVar();

GT_GUI_EXPORT Icon arrowDown();
GT_GUI_EXPORT Icon arrowUp();

GT_GUI_EXPORT Icon arrowRight();
GT_GUI_EXPORT Icon arrowLeft();

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

GT_GUI_EXPORT Icon cancel();

GT_GUI_EXPORT Icon chain();
GT_GUI_EXPORT Icon chainOff();

GT_GUI_EXPORT Icon chartBar();
GT_GUI_EXPORT Icon chartBell();
GT_GUI_EXPORT Icon chartCurve();
GT_GUI_EXPORT Icon chartLine();
GT_GUI_EXPORT Icon chartScatter();
GT_GUI_EXPORT Icon chartXY();

GT_GUI_EXPORT Icon check();

GT_GUI_EXPORT Icon clear();

GT_GUI_EXPORT Icon clone();

GT_GUI_EXPORT Icon close();

GT_GUI_EXPORT Icon cog();
GT_GUI_EXPORT Icon cogPlus();

GT_GUI_EXPORT Icon collapsed();
GT_GUI_EXPORT Icon collapsedColorized();
GT_GUI_EXPORT Icon uncollapsed();
GT_GUI_EXPORT Icon uncollapsedColorized();

GT_GUI_EXPORT Icon collection();

GT_GUI_EXPORT Icon comment();

GT_GUI_EXPORT Icon config();

GT_GUI_EXPORT Icon connection();
GT_GUI_EXPORT Icon connection16();

GT_GUI_EXPORT Icon convergence();

GT_GUI_EXPORT Icon copy();

GT_GUI_EXPORT Icon cross();

GT_GUI_EXPORT Icon cursor();
GT_GUI_EXPORT Icon cursorMove();

GT_GUI_EXPORT Icon cut();

GT_GUI_EXPORT Icon data();
GT_GUI_EXPORT Icon dataArrowUp();
GT_GUI_EXPORT Icon dataEye();
GT_GUI_EXPORT Icon dataLock();
GT_GUI_EXPORT Icon dataSingle();

GT_GUI_EXPORT Icon delete_();

GT_GUI_EXPORT Icon disk();
GT_GUI_EXPORT Icon diskGradient();

GT_GUI_EXPORT Icon devices();

GT_GUI_EXPORT Icon dock();

GT_GUI_EXPORT Icon dots();

GT_GUI_EXPORT Icon download();

GT_GUI_EXPORT Icon duplicate();

GT_GUI_EXPORT Icon error();
GT_GUI_EXPORT Icon errorColorized();

GT_GUI_EXPORT Icon examples();

GT_GUI_EXPORT Icon exclamationmark();

GT_GUI_EXPORT Icon export_();

GT_GUI_EXPORT Icon eye();
GT_GUI_EXPORT Icon eyeOff();

GT_GUI_EXPORT Icon fatal();
GT_GUI_EXPORT Icon fatalColorized();

GT_GUI_EXPORT Icon file();
GT_GUI_EXPORT Icon fileAdd();
GT_GUI_EXPORT Icon fileClock();
GT_GUI_EXPORT Icon fileCode();
GT_GUI_EXPORT Icon fileDelete();
GT_GUI_EXPORT Icon fileDoc();
GT_GUI_EXPORT Icon fileEdit();
GT_GUI_EXPORT Icon fileEye();
GT_GUI_EXPORT Icon fileImport();

GT_GUI_EXPORT Icon folder();
GT_GUI_EXPORT Icon folderAdd();
GT_GUI_EXPORT Icon folderEdit();
GT_GUI_EXPORT Icon folderEye();
GT_GUI_EXPORT Icon folderOpen();
GT_GUI_EXPORT Icon folderSearch();

GT_GUI_EXPORT Icon font();

GT_GUI_EXPORT Icon gas();

GT_GUI_EXPORT Icon global();

GT_GUI_EXPORT Icon grid();
GT_GUI_EXPORT Icon gridSnap();

GT_GUI_EXPORT Icon help();

GT_GUI_EXPORT Icon hdf5();

GT_GUI_EXPORT Icon histogram();

GT_GUI_EXPORT Icon history();

GT_GUI_EXPORT Icon home();
GT_GUI_EXPORT Icon homeAccount();

GT_GUI_EXPORT Icon import();

GT_GUI_EXPORT Icon info();
GT_GUI_EXPORT Icon infoColorized();
GT_GUI_EXPORT Icon info2();

GT_GUI_EXPORT Icon input();
GT_GUI_EXPORT Icon inputForm();
GT_GUI_EXPORT Icon inputSettings();

GT_GUI_EXPORT Icon jumpTo();

GT_GUI_EXPORT Icon json();

GT_GUI_EXPORT Icon label();

GT_GUI_EXPORT Icon language();

GT_GUI_EXPORT Icon layers();

GT_GUI_EXPORT Icon list();
GT_GUI_EXPORT Icon listFormatted();

GT_GUI_EXPORT Icon log();

GT_GUI_EXPORT Icon login();

GT_GUI_EXPORT Icon loop();

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

GT_GUI_EXPORT Icon open();

GT_GUI_EXPORT Icon palette();

GT_GUI_EXPORT Icon paramStudy();

GT_GUI_EXPORT Icon paste();

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
GT_GUI_EXPORT Icon perspectivesEdit();

GT_GUI_EXPORT Icon play();

GT_GUI_EXPORT Icon plugin();
GT_GUI_EXPORT Icon pluginSettings();

GT_GUI_EXPORT Icon print();

GT_GUI_EXPORT Icon process();
GT_GUI_EXPORT Icon processAdd();
GT_GUI_EXPORT Icon processRun();

GT_GUI_EXPORT Icon projectAdd();
GT_GUI_EXPORT Icon projectClose();
GT_GUI_EXPORT Icon projectClosed();
GT_GUI_EXPORT Icon projectDelete();
GT_GUI_EXPORT Icon projectImport();
GT_GUI_EXPORT Icon projectOpen();
GT_GUI_EXPORT Icon projectRemove();

GT_GUI_EXPORT Icon property();

GT_GUI_EXPORT Icon prp();

GT_GUI_EXPORT Icon puzzle();

GT_GUI_EXPORT Icon python();

GT_GUI_EXPORT Icon questionmark();

GT_GUI_EXPORT Icon queue();
GT_GUI_EXPORT Icon queueAdd();

GT_GUI_EXPORT Icon redo();

GT_GUI_EXPORT Icon reload();

GT_GUI_EXPORT Icon remove();

GT_GUI_EXPORT Icon rename();

GT_GUI_EXPORT Icon repeat();

GT_GUI_EXPORT Icon resize();

GT_GUI_EXPORT Icon results();

GT_GUI_EXPORT Icon revert();

GT_GUI_EXPORT Icon ruler();

GT_GUI_EXPORT Icon save();

GT_GUI_EXPORT Icon schedules();
GT_GUI_EXPORT Icon schedules2();

GT_GUI_EXPORT Icon search();

GT_GUI_EXPORT Icon select();

GT_GUI_EXPORT Icon server();

GT_GUI_EXPORT Icon session();

GT_GUI_EXPORT Icon showText();

GT_GUI_EXPORT Icon skip();
GT_GUI_EXPORT Icon unskip();

GT_GUI_EXPORT Icon sleep();
GT_GUI_EXPORT Icon sleepOff();

GT_GUI_EXPORT Icon square();

GT_GUI_EXPORT Icon sort();
GT_GUI_EXPORT Icon sortAsc();
GT_GUI_EXPORT Icon sortDesc();

GT_GUI_EXPORT Icon stop();

GT_GUI_EXPORT Icon stepFile();

GT_GUI_EXPORT Icon stretch();

GT_GUI_EXPORT Icon swap();

GT_GUI_EXPORT Icon sync();

GT_GUI_EXPORT Icon table();

GT_GUI_EXPORT Icon text();

GT_GUI_EXPORT Icon timer();
GT_GUI_EXPORT Icon timerSand();

GT_GUI_EXPORT Icon trace();
GT_GUI_EXPORT Icon traceColorized();

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

GT_GUI_EXPORT Icon undo();

GT_GUI_EXPORT Icon update();

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

GT_GUI_EXPORT Icon web();

GT_GUI_EXPORT Icon xml();

GT_GUI_EXPORT Icon zoomMinus();
GT_GUI_EXPORT Icon zoomPlus();



/** OTHER **/

GT_GUI_EXPORT Icon engineInstallation();

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

GT_GUI_EXPORT QPixmap downloaderLogo();

GT_GUI_EXPORT QPixmap printHeadline();

} // namespace pixmap

} // namespace gui

} // namespace gt

Q_DECLARE_METATYPE(gt::gui::Icon)

#endif // GTICONS_H
