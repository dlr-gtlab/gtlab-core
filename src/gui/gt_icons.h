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
    return QString("qrc") + getIcon().name();
}


/**
 * @brief standardSizeSmall
 * @return the standardsize of a small icon
 */
GT_GUI_EXPORT QSize standardSizeSmall();

GT_GUI_EXPORT QIcon add();
GT_GUI_EXPORT QUrl add_Url();
[[deprecated("Use add instead")]]
GT_GUI_EXPORT QIcon add16();
GT_GUI_EXPORT QUrl add16_Url();
[[deprecated("Use projectAdd instead")]]
GT_GUI_EXPORT QIcon addProject();
GT_GUI_EXPORT QUrl addProject_Url();

GT_GUI_EXPORT QIcon application();
GT_GUI_EXPORT QUrl application_Url();
GT_GUI_EXPORT QIcon applicationSettings();
GT_GUI_EXPORT QUrl applicationSettings_Url();
GT_GUI_EXPORT QIcon applicationVar();
GT_GUI_EXPORT QUrl applicationVar_Url();

GT_GUI_EXPORT QIcon arrowDown();
GT_GUI_EXPORT QUrl arrowDown_Url();
GT_GUI_EXPORT QIcon arrowUp();
GT_GUI_EXPORT QUrl arrowUp_Url();

GT_GUI_EXPORT QIcon arrowRight();
GT_GUI_EXPORT QUrl arrowRight_Url();
[[deprecated("Use arrowRight instead")]]
GT_GUI_EXPORT QIcon arrowRightBlue();
GT_GUI_EXPORT QUrl arrowRightBlue_Url();
GT_GUI_EXPORT QIcon arrowLeft();
GT_GUI_EXPORT QUrl arrowLeft_Url();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT QIcon arrowDownBlue();
GT_GUI_EXPORT QUrl arrowDownBlue_Url();
[[deprecated("Use arrowDown instead")]]
GT_GUI_EXPORT QIcon arrowDownBlueAll();
GT_GUI_EXPORT QUrl arrowDownBlueAll_Url();
[[deprecated("Use arrowUp instead")]]
GT_GUI_EXPORT QIcon arrowUpBlueAll();
GT_GUI_EXPORT QUrl arrowUpBlueAll_Url();

GT_GUI_EXPORT QIcon backspace();
GT_GUI_EXPORT QUrl backspace_Url();
GT_GUI_EXPORT QIcon backspaceFlipped();
GT_GUI_EXPORT QUrl backspaceFlipped_Url();

GT_GUI_EXPORT QIcon binaryTree();
GT_GUI_EXPORT QUrl binaryTree_Url();

GT_GUI_EXPORT QIcon blades();
GT_GUI_EXPORT QUrl blades_Url();

GT_GUI_EXPORT QIcon brush();
GT_GUI_EXPORT QUrl brush_Url();

GT_GUI_EXPORT QIcon bug();
GT_GUI_EXPORT QUrl bug_Url();
GT_GUI_EXPORT QIcon bugColorized();
GT_GUI_EXPORT QUrl bugColorized_Url();
GT_GUI_EXPORT QIcon bugPlay();
GT_GUI_EXPORT QUrl bugPlay_Url();

GT_GUI_EXPORT QIcon bulb();
GT_GUI_EXPORT QUrl bulb_Url();

GT_GUI_EXPORT QIcon calculator();
GT_GUI_EXPORT QUrl calculator_Url();
[[deprecated("Use calculator instead")]]
GT_GUI_EXPORT QIcon calculator16();
GT_GUI_EXPORT QUrl calculator16_Url();

GT_GUI_EXPORT QIcon cancel();
GT_GUI_EXPORT QUrl cancel_Url();

GT_GUI_EXPORT QIcon chain();
GT_GUI_EXPORT QUrl chain_Url();
[[deprecated("Use chain instead")]]
GT_GUI_EXPORT QIcon chain16();
GT_GUI_EXPORT QUrl chain16_Url();
GT_GUI_EXPORT QIcon chainOff();
GT_GUI_EXPORT QUrl chainOff_Url();

GT_GUI_EXPORT QIcon chartBar();
GT_GUI_EXPORT QUrl chartBar_Url();
GT_GUI_EXPORT QIcon chartBell();
GT_GUI_EXPORT QUrl chartBell_Url();
GT_GUI_EXPORT QIcon chartCurve();
GT_GUI_EXPORT QUrl chartCurve_Url();
GT_GUI_EXPORT QIcon chartLine();
GT_GUI_EXPORT QUrl chartLine_Url();
GT_GUI_EXPORT QIcon chartScatter();
GT_GUI_EXPORT QUrl chartScatter_Url();
GT_GUI_EXPORT QIcon chartXY();
GT_GUI_EXPORT QUrl chartXY_Url();

GT_GUI_EXPORT QIcon check();
GT_GUI_EXPORT QUrl check_Url();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon check16();
GT_GUI_EXPORT QUrl check16_Url();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon check24();
GT_GUI_EXPORT QUrl check24_Url();
[[deprecated("Use check instead")]]
GT_GUI_EXPORT QIcon checkSmall16();
GT_GUI_EXPORT QUrl checkSmall16_Url();

GT_GUI_EXPORT QIcon clear();
GT_GUI_EXPORT QUrl clear_Url();
[[deprecated("Use clear instead")]]
GT_GUI_EXPORT QIcon clear16();
GT_GUI_EXPORT QUrl clear16_Url();
[[deprecated("Use backspaceFlipped instead")]]
GT_GUI_EXPORT QIcon clear2_16();

GT_GUI_EXPORT QIcon clone();
GT_GUI_EXPORT QUrl clone_Url();
[[deprecated("Use clone instead")]]
GT_GUI_EXPORT QIcon clone16();
GT_GUI_EXPORT QUrl clone16_Url();

[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject();
GT_GUI_EXPORT QUrl closeProject_Url();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject16();
GT_GUI_EXPORT QUrl closeProject16_Url();
[[deprecated("Use projectClose instead")]]
GT_GUI_EXPORT QIcon closeProject24();
GT_GUI_EXPORT QUrl closeProject24_Url();

[[deprecated("Use projectClosed instead")]]
GT_GUI_EXPORT QIcon closedProject16();
GT_GUI_EXPORT QUrl closedProject16_Url();

GT_GUI_EXPORT QIcon close();
GT_GUI_EXPORT QUrl close_Url();

GT_GUI_EXPORT QIcon cog();
GT_GUI_EXPORT QUrl cog_Url();
GT_GUI_EXPORT QIcon cogPlus();
GT_GUI_EXPORT QUrl cogPlus_Url();

GT_GUI_EXPORT QIcon collapsed();
GT_GUI_EXPORT QUrl collapsed_Url();
GT_GUI_EXPORT QIcon collapsedColorized();
GT_GUI_EXPORT QUrl collapsedColorized_Url();
GT_GUI_EXPORT QIcon uncollapsed();
GT_GUI_EXPORT QUrl uncollapsed_Url();
GT_GUI_EXPORT QIcon uncollapsedColorized();
GT_GUI_EXPORT QUrl uncollapsedColorized_Url();

GT_GUI_EXPORT QIcon collection();
GT_GUI_EXPORT QUrl collection_Url();
[[deprecated("Use collection instead")]]
GT_GUI_EXPORT QIcon collection16();
GT_GUI_EXPORT QUrl collection16_Url();

GT_GUI_EXPORT QIcon comment();
GT_GUI_EXPORT QUrl comment_Url();

GT_GUI_EXPORT QIcon config();
GT_GUI_EXPORT QUrl config_Url();
[[deprecated("Use config instead")]]
GT_GUI_EXPORT QIcon config16();
GT_GUI_EXPORT QUrl config16_Url();

GT_GUI_EXPORT QIcon connection();
GT_GUI_EXPORT QUrl connection_Url();
GT_GUI_EXPORT QIcon connection16();
GT_GUI_EXPORT QUrl connection16_Url();

GT_GUI_EXPORT QIcon convergence();
GT_GUI_EXPORT QUrl convergence_Url();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT QIcon convergence16();
GT_GUI_EXPORT QUrl convergence16_Url();
[[deprecated("Use convergence instead")]]
GT_GUI_EXPORT QIcon convergence24();
GT_GUI_EXPORT QUrl convergence24_Url();

GT_GUI_EXPORT QIcon copy();
GT_GUI_EXPORT QUrl copy_Url();
[[deprecated("Use copy instead")]]
GT_GUI_EXPORT QIcon copy16();
GT_GUI_EXPORT QUrl copy16_Url();

GT_GUI_EXPORT QIcon cross();
GT_GUI_EXPORT QUrl cross_Url();
[[deprecated("Use cross instead")]]
GT_GUI_EXPORT QIcon cross16();
GT_GUI_EXPORT QUrl cross16_Url();

GT_GUI_EXPORT QIcon cursor();
GT_GUI_EXPORT QUrl cursor_Url();
GT_GUI_EXPORT QIcon cursorMove();
GT_GUI_EXPORT QUrl cursorMove_Url();

GT_GUI_EXPORT QIcon cut();
GT_GUI_EXPORT QUrl cut_Url();
[[deprecated("Use cut instead")]]
GT_GUI_EXPORT QIcon cut16();
GT_GUI_EXPORT QUrl cut16_Url();

GT_GUI_EXPORT QIcon data();
GT_GUI_EXPORT QUrl data_Url();
[[deprecated("Use data instead")]]
GT_GUI_EXPORT QIcon data16();
GT_GUI_EXPORT QUrl data16_Url();
GT_GUI_EXPORT QIcon dataArrowUp();
GT_GUI_EXPORT QUrl dataArrowUp_Url();
GT_GUI_EXPORT QIcon dataEye();
GT_GUI_EXPORT QUrl dataEye_Url();
GT_GUI_EXPORT QIcon dataLock();
GT_GUI_EXPORT QUrl dataLock_Url();
GT_GUI_EXPORT QIcon dataSingle();
GT_GUI_EXPORT QUrl dataSingle_Url();

GT_GUI_EXPORT QIcon delete_();
GT_GUI_EXPORT QUrl delete__Url();
[[deprecated("Use delete_ instead")]]
GT_GUI_EXPORT QIcon delete16();
GT_GUI_EXPORT QUrl delete16_Url();

GT_GUI_EXPORT QIcon disk();
GT_GUI_EXPORT QUrl disk_Url();
GT_GUI_EXPORT QIcon diskGradient();
GT_GUI_EXPORT QUrl diskGradient_Url();

[[deprecated("Use projectDelete instead")]]
GT_GUI_EXPORT QIcon deleteProject16();
GT_GUI_EXPORT QUrl deleteProject16_Url();

GT_GUI_EXPORT QIcon devices();
GT_GUI_EXPORT QUrl devices_Url();

GT_GUI_EXPORT QIcon dock();
GT_GUI_EXPORT QUrl dock_Url();

GT_GUI_EXPORT QIcon dots();
GT_GUI_EXPORT QUrl dots_Url();

GT_GUI_EXPORT QIcon download();
GT_GUI_EXPORT QUrl download_Url();

GT_GUI_EXPORT QIcon duplicate();
GT_GUI_EXPORT QUrl duplicate_Url();

[[deprecated("Use object instead")]]
GT_GUI_EXPORT QIcon empty16();
GT_GUI_EXPORT QUrl empty16_Url();

GT_GUI_EXPORT QIcon error();
GT_GUI_EXPORT QUrl error_Url();
GT_GUI_EXPORT QIcon errorColorized();
GT_GUI_EXPORT QUrl errorColorized_Url();
[[deprecated("Use error instead")]]
GT_GUI_EXPORT QIcon error16();
GT_GUI_EXPORT QUrl error16_Url();

GT_GUI_EXPORT QIcon examples();
GT_GUI_EXPORT QUrl examples_Url();
[[deprecated("Use examples instead")]]
GT_GUI_EXPORT QIcon examples16();
GT_GUI_EXPORT QUrl examples16_Url();

GT_GUI_EXPORT QIcon exclamationmark();
GT_GUI_EXPORT QUrl exclamationmark_Url();
[[deprecated("Use exclamationmark instead")]]
GT_GUI_EXPORT QIcon exclamation16();
GT_GUI_EXPORT QUrl exclamation16_Url();

GT_GUI_EXPORT QIcon export_();
GT_GUI_EXPORT QUrl export__Url();
[[deprecated("Use export_ instead")]]
GT_GUI_EXPORT QIcon export16();
GT_GUI_EXPORT QUrl export16_Url();

GT_GUI_EXPORT QIcon eye();
GT_GUI_EXPORT QUrl eye_Url();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT QIcon eye16();
GT_GUI_EXPORT QUrl eye16_Url();
[[deprecated("Use eye instead")]]
GT_GUI_EXPORT QIcon eye24();
GT_GUI_EXPORT QUrl eye24_Url();
GT_GUI_EXPORT QIcon eyeOff();
GT_GUI_EXPORT QUrl eyeOff_Url();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT QIcon eyeOff16();
GT_GUI_EXPORT QUrl eyeOff16_Url();
[[deprecated("Use eyeOff instead")]]
GT_GUI_EXPORT QIcon eyeOff24();
GT_GUI_EXPORT QUrl eyeOff24_Url();

GT_GUI_EXPORT QIcon fatal();
GT_GUI_EXPORT QUrl fatal_Url();
GT_GUI_EXPORT QIcon fatalColorized();
GT_GUI_EXPORT QUrl fatalColorized_Url();
[[deprecated("Use fatal instead")]]
GT_GUI_EXPORT QIcon fatal16();
GT_GUI_EXPORT QUrl fatal16_Url();

GT_GUI_EXPORT QIcon file();
GT_GUI_EXPORT QUrl file_Url();
[[deprecated("Use file instead")]]
GT_GUI_EXPORT QIcon file16();
GT_GUI_EXPORT QUrl file16_Url();
GT_GUI_EXPORT QIcon fileAdd();
GT_GUI_EXPORT QUrl fileAdd_Url();
GT_GUI_EXPORT QIcon fileClock();
GT_GUI_EXPORT QUrl fileClock_Url();
GT_GUI_EXPORT QIcon fileCode();
GT_GUI_EXPORT QUrl fileCode_Url();
GT_GUI_EXPORT QIcon fileDelete();
GT_GUI_EXPORT QUrl fileDelete_Url();
GT_GUI_EXPORT QIcon fileDoc();
GT_GUI_EXPORT QUrl fileDoc_Url();
GT_GUI_EXPORT QIcon fileEdit();
GT_GUI_EXPORT QUrl fileEdit_Url();
GT_GUI_EXPORT QIcon fileEye();
GT_GUI_EXPORT QUrl fileEye_Url();
GT_GUI_EXPORT QIcon fileImport();
GT_GUI_EXPORT QUrl fileImport_Url();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT QIcon fileStep();
GT_GUI_EXPORT QUrl fileStep_Url();

GT_GUI_EXPORT QIcon folder();
GT_GUI_EXPORT QUrl folder_Url();
[[deprecated("Use folder instead")]]
GT_GUI_EXPORT QIcon folder16();
GT_GUI_EXPORT QUrl folder16_Url();
GT_GUI_EXPORT QIcon folderAdd();
GT_GUI_EXPORT QUrl folderAdd_Url();
GT_GUI_EXPORT QIcon folderEdit();
GT_GUI_EXPORT QUrl folderEdit_Url();
GT_GUI_EXPORT QIcon folderEye();
GT_GUI_EXPORT QUrl folderEye_Url();
GT_GUI_EXPORT QIcon folderOpen();
GT_GUI_EXPORT QUrl folderOpen_Url();
GT_GUI_EXPORT QIcon folderSearch();
GT_GUI_EXPORT QUrl folderSearch_Url();

GT_GUI_EXPORT QIcon font();
GT_GUI_EXPORT QUrl font_Url();

GT_GUI_EXPORT QIcon gas();
GT_GUI_EXPORT QUrl gas_Url();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gas16();
GT_GUI_EXPORT QUrl gas16_Url();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gas24();
GT_GUI_EXPORT QUrl gas24_Url();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes();
GT_GUI_EXPORT QUrl gasTypes_Url();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes16();
GT_GUI_EXPORT QUrl gasTypes16_Url();
[[deprecated("Use gas instead")]]
GT_GUI_EXPORT QIcon gasTypes24();
GT_GUI_EXPORT QUrl gasTypes24_Url();

GT_GUI_EXPORT QIcon global();
GT_GUI_EXPORT QUrl global_Url();

GT_GUI_EXPORT QIcon grid();
GT_GUI_EXPORT QUrl grid_Url();
GT_GUI_EXPORT QIcon gridSnap();
GT_GUI_EXPORT QUrl gridSnap_Url();

GT_GUI_EXPORT QIcon help();
GT_GUI_EXPORT QUrl help_Url();

GT_GUI_EXPORT QIcon hdf5();
GT_GUI_EXPORT QUrl hdf5_Url();

GT_GUI_EXPORT QIcon histogram();
GT_GUI_EXPORT QUrl histogram_Url();
[[deprecated("Use histogram() instead")]]
GT_GUI_EXPORT QIcon histogram16();
GT_GUI_EXPORT QUrl histogram16_Url();

GT_GUI_EXPORT QIcon history();
GT_GUI_EXPORT QUrl history_Url();
[[deprecated("Use history() instead")]]
GT_GUI_EXPORT QIcon history16();
GT_GUI_EXPORT QUrl history16_Url();

GT_GUI_EXPORT QIcon home();
GT_GUI_EXPORT QUrl home_Url();
GT_GUI_EXPORT QIcon homeAccount();
GT_GUI_EXPORT QUrl homeAccount_Url();

GT_GUI_EXPORT QIcon import();
GT_GUI_EXPORT QUrl import_Url();
[[deprecated("Use import() instead")]]
GT_GUI_EXPORT QIcon import16();
GT_GUI_EXPORT QUrl import16_Url();

[[deprecated("Use objectInvalid instead")]]
GT_GUI_EXPORT QIcon inconsistentProject16();
GT_GUI_EXPORT QUrl inconsistentProject16_Url();

GT_GUI_EXPORT QIcon info();
GT_GUI_EXPORT QUrl info_Url();
GT_GUI_EXPORT QIcon infoColorized();
GT_GUI_EXPORT QUrl infoColorized_Url();
GT_GUI_EXPORT QIcon info2();
GT_GUI_EXPORT QUrl info2_Url();
[[deprecated("Use info instead")]]
GT_GUI_EXPORT QIcon info16();
GT_GUI_EXPORT QUrl info16_Url();

[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT QIcon infoBlue();
GT_GUI_EXPORT QUrl infoBlue_Url();
[[deprecated("Use info2() instead")]]
GT_GUI_EXPORT QIcon infoBlue16();
GT_GUI_EXPORT QUrl infoBlue16_Url();

GT_GUI_EXPORT QIcon input();
GT_GUI_EXPORT QUrl input_Url();
GT_GUI_EXPORT QIcon inputForm();
GT_GUI_EXPORT QUrl inputForm_Url();
[[deprecated("Use input instead")]]
GT_GUI_EXPORT QIcon input16();
GT_GUI_EXPORT QUrl input16_Url();
[[deprecated("Use inputSettings instead")]]
GT_GUI_EXPORT QIcon input2();
GT_GUI_EXPORT QUrl input2_Url();
GT_GUI_EXPORT QIcon inputSettings();
GT_GUI_EXPORT QUrl inputSettings_Url();

GT_GUI_EXPORT QIcon jumpTo();
GT_GUI_EXPORT QUrl jumpTo_Url();

GT_GUI_EXPORT QIcon json();
GT_GUI_EXPORT QUrl json_Url();

GT_GUI_EXPORT QIcon label();
GT_GUI_EXPORT QUrl label_Url();

GT_GUI_EXPORT QIcon language();
GT_GUI_EXPORT QUrl language_Url();

GT_GUI_EXPORT QIcon layers();
GT_GUI_EXPORT QUrl layers_Url();

GT_GUI_EXPORT QIcon list();
GT_GUI_EXPORT QUrl list_Url();
[[deprecated("Use list instead")]]
GT_GUI_EXPORT QIcon list16();
GT_GUI_EXPORT QUrl list16_Url();
GT_GUI_EXPORT QIcon listFormatted();
GT_GUI_EXPORT QUrl listFormatted_Url();

GT_GUI_EXPORT QIcon log();
GT_GUI_EXPORT QUrl log_Url();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT QIcon log16();
GT_GUI_EXPORT QUrl log16_Url();

GT_GUI_EXPORT QIcon login();
GT_GUI_EXPORT QUrl login_Url();
[[deprecated("Use login instead")]]
GT_GUI_EXPORT QIcon login16();
GT_GUI_EXPORT QUrl login16_Url();

GT_GUI_EXPORT QIcon loop();
GT_GUI_EXPORT QUrl loop_Url();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT QIcon loop16();
GT_GUI_EXPORT QUrl loop16_Url();
[[deprecated("Use loop instead")]]
GT_GUI_EXPORT QIcon loop2_16();

GT_GUI_EXPORT QIcon magnet();
GT_GUI_EXPORT QUrl magnet_Url();

GT_GUI_EXPORT QIcon mass();
GT_GUI_EXPORT QUrl mass_Url();

GT_GUI_EXPORT QIcon mathDivision();
GT_GUI_EXPORT QUrl mathDivision_Url();
GT_GUI_EXPORT QIcon mathExponent();
GT_GUI_EXPORT QUrl mathExponent_Url();
GT_GUI_EXPORT QIcon mathMinus();
GT_GUI_EXPORT QUrl mathMinus_Url();
GT_GUI_EXPORT QIcon mathMultiplication();
GT_GUI_EXPORT QUrl mathMultiplication_Url();
GT_GUI_EXPORT QIcon mathPlus();
GT_GUI_EXPORT QUrl mathPlus_Url();
GT_GUI_EXPORT QIcon mathRoot();
GT_GUI_EXPORT QUrl mathRoot_Url();

GT_GUI_EXPORT QIcon minimize();
GT_GUI_EXPORT QUrl minimize_Url();

GT_GUI_EXPORT QIcon network();
GT_GUI_EXPORT QUrl network_Url();
[[deprecated("Use network instead")]]
GT_GUI_EXPORT QIcon network16();
GT_GUI_EXPORT QUrl network16_Url();

GT_GUI_EXPORT QIcon noteEdit();
GT_GUI_EXPORT QUrl noteEdit_Url();

GT_GUI_EXPORT QIcon notificationUnread();
GT_GUI_EXPORT QUrl notificationUnread_Url();

GT_GUI_EXPORT QIcon objectCombustor();
GT_GUI_EXPORT QUrl objectCombustor_Url();
GT_GUI_EXPORT QIcon objectEmpty();
GT_GUI_EXPORT QUrl objectEmpty_Url();
GT_GUI_EXPORT QIcon objectEngine();
GT_GUI_EXPORT QUrl objectEngine_Url();
GT_GUI_EXPORT QIcon objectFreestyleComponent();
GT_GUI_EXPORT QUrl objectFreestyleComponent_Url();
GT_GUI_EXPORT QIcon objectInvalid();
GT_GUI_EXPORT QUrl objectInvalid_Url();
GT_GUI_EXPORT QIcon objectUnknown();
GT_GUI_EXPORT QUrl objectUnknown_Url();
GT_GUI_EXPORT QIcon objectTurbine();
GT_GUI_EXPORT QUrl objectTurbine_Url();
GT_GUI_EXPORT QIcon objectCompressor();
GT_GUI_EXPORT QUrl objectCompressor_Url();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT QIcon objectUnkown();
GT_GUI_EXPORT QUrl objectUnkown_Url();

GT_GUI_EXPORT QIcon open();
GT_GUI_EXPORT QUrl open_Url();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open16();
GT_GUI_EXPORT QUrl open16_Url();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2();
GT_GUI_EXPORT QUrl open2_Url();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2_16();
[[deprecated("Use open instead")]]
GT_GUI_EXPORT QIcon open2_24();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT QIcon openProject16();
GT_GUI_EXPORT QUrl openProject16_Url();

GT_GUI_EXPORT QIcon palette();
GT_GUI_EXPORT QUrl palette_Url();

GT_GUI_EXPORT QIcon paramStudy();
GT_GUI_EXPORT QUrl paramStudy_Url();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT QIcon paramStudy16();
GT_GUI_EXPORT QUrl paramStudy16_Url();
[[deprecated("Use paramStudy instead")]]
GT_GUI_EXPORT QIcon paramStudy24();
GT_GUI_EXPORT QUrl paramStudy24_Url();

GT_GUI_EXPORT QIcon paste();
GT_GUI_EXPORT QUrl paste_Url();
[[deprecated("Use paste instead")]]
GT_GUI_EXPORT QIcon paste16();
GT_GUI_EXPORT QUrl paste16_Url();

GT_GUI_EXPORT QIcon pause();
GT_GUI_EXPORT QUrl pause_Url();

GT_GUI_EXPORT QIcon pdf();
GT_GUI_EXPORT QUrl pdf_Url();

GT_GUI_EXPORT QIcon perfDesignPoint();
GT_GUI_EXPORT QUrl perfDesignPoint_Url();
GT_GUI_EXPORT QIcon perfMap();
GT_GUI_EXPORT QUrl perfMap_Url();
GT_GUI_EXPORT QIcon perfMapExport();
GT_GUI_EXPORT QUrl perfMapExport_Url();
GT_GUI_EXPORT QIcon perfNoMap();
GT_GUI_EXPORT QUrl perfNoMap_Url();
GT_GUI_EXPORT QIcon perfModel();
GT_GUI_EXPORT QUrl perfModel_Url();
GT_GUI_EXPORT QIcon perfOperatingPoint();
GT_GUI_EXPORT QUrl perfOperatingPoint_Url();
GT_GUI_EXPORT QIcon perfStageMap();
GT_GUI_EXPORT QUrl perfStageMap_Url();
GT_GUI_EXPORT QIcon perfSpeedLine();
GT_GUI_EXPORT QUrl perfSpeedLine_Url();
GT_GUI_EXPORT QIcon perfTsDiagram();
GT_GUI_EXPORT QUrl perfTsDiagram_Url();
GT_GUI_EXPORT QIcon perfWorkingLine();
GT_GUI_EXPORT QUrl perfWorkingLine_Url();

GT_GUI_EXPORT QIcon perspectives();
GT_GUI_EXPORT QUrl perspectives_Url();
[[deprecated("Use perspectives instead")]]
GT_GUI_EXPORT QIcon perspectives16();
GT_GUI_EXPORT QUrl perspectives16_Url();
GT_GUI_EXPORT QIcon perspectivesEdit();
GT_GUI_EXPORT QUrl perspectivesEdit_Url();

GT_GUI_EXPORT QIcon play();
GT_GUI_EXPORT QUrl play_Url();

GT_GUI_EXPORT QIcon plugin();
GT_GUI_EXPORT QUrl plugin_Url();
[[deprecated("Use plugin instead")]]
GT_GUI_EXPORT QIcon plugin16();
GT_GUI_EXPORT QUrl plugin16_Url();
GT_GUI_EXPORT QIcon pluginSettings();
GT_GUI_EXPORT QUrl pluginSettings_Url();

GT_GUI_EXPORT QIcon print();
GT_GUI_EXPORT QUrl print_Url();
[[deprecated("Use print instead")]]
GT_GUI_EXPORT QIcon printer();
GT_GUI_EXPORT QUrl printer_Url();

GT_GUI_EXPORT QIcon process();
GT_GUI_EXPORT QUrl process_Url();
[[deprecated("Use process instead")]]
GT_GUI_EXPORT QIcon process16();
GT_GUI_EXPORT QUrl process16_Url();
GT_GUI_EXPORT QIcon processAdd();
GT_GUI_EXPORT QUrl processAdd_Url();
GT_GUI_EXPORT QIcon processRun();
GT_GUI_EXPORT QUrl processRun_Url();

[[deprecated("Use warning instead")]]
GT_GUI_EXPORT QIcon processFailed16();
GT_GUI_EXPORT QUrl processFailed16_Url();

GT_GUI_EXPORT QIcon projectAdd();
GT_GUI_EXPORT QUrl projectAdd_Url();
GT_GUI_EXPORT QIcon projectClose();
GT_GUI_EXPORT QUrl projectClose_Url();
GT_GUI_EXPORT QIcon projectClosed();
GT_GUI_EXPORT QUrl projectClosed_Url();
GT_GUI_EXPORT QIcon projectDelete();
GT_GUI_EXPORT QUrl projectDelete_Url();
GT_GUI_EXPORT QIcon projectImport();
GT_GUI_EXPORT QUrl projectImport_Url();
GT_GUI_EXPORT QIcon projectOpen();
GT_GUI_EXPORT QUrl projectOpen_Url();
GT_GUI_EXPORT QIcon projectRemove();
GT_GUI_EXPORT QUrl projectRemove_Url();

GT_GUI_EXPORT QIcon property();
GT_GUI_EXPORT QUrl property_Url();
[[deprecated("Use property instead")]]
GT_GUI_EXPORT QIcon property16();
GT_GUI_EXPORT QUrl property16_Url();

GT_GUI_EXPORT QIcon prp();
GT_GUI_EXPORT QUrl prp_Url();
[[deprecated("Use prp instead")]]
GT_GUI_EXPORT QIcon prp16();
GT_GUI_EXPORT QUrl prp16_Url();

GT_GUI_EXPORT QIcon puzzle();
GT_GUI_EXPORT QUrl puzzle_Url();

GT_GUI_EXPORT QIcon python();
GT_GUI_EXPORT QUrl python_Url();
[[deprecated("Use python instead")]]
GT_GUI_EXPORT QIcon python16();
GT_GUI_EXPORT QUrl python16_Url();

GT_GUI_EXPORT QIcon questionmark();
GT_GUI_EXPORT QUrl questionmark_Url();
[[deprecated("Use questionmark instead")]]
GT_GUI_EXPORT QIcon question16();
GT_GUI_EXPORT QUrl question16_Url();

GT_GUI_EXPORT QIcon queue();
GT_GUI_EXPORT QUrl queue_Url();
[[deprecated("Use queue instead")]]
GT_GUI_EXPORT QIcon queue16();
GT_GUI_EXPORT QUrl queue16_Url();
GT_GUI_EXPORT QIcon queueAdd();
GT_GUI_EXPORT QUrl queueAdd_Url();

GT_GUI_EXPORT QIcon redo();
GT_GUI_EXPORT QUrl redo_Url();
[[deprecated("Use redo instead")]]
GT_GUI_EXPORT QIcon redo24();
GT_GUI_EXPORT QUrl redo24_Url();

GT_GUI_EXPORT QIcon reload();
GT_GUI_EXPORT QUrl reload_Url();

GT_GUI_EXPORT QIcon remove();
GT_GUI_EXPORT QUrl remove_Url();

GT_GUI_EXPORT QIcon rename();
GT_GUI_EXPORT QUrl rename_Url();

GT_GUI_EXPORT QIcon repeat();
GT_GUI_EXPORT QUrl repeat_Url();

GT_GUI_EXPORT QIcon resize();
GT_GUI_EXPORT QUrl resize_Url();

GT_GUI_EXPORT QIcon results();
GT_GUI_EXPORT QUrl results_Url();
[[deprecated("Use results instead")]]
GT_GUI_EXPORT QIcon results16();
GT_GUI_EXPORT QUrl results16_Url();

GT_GUI_EXPORT QIcon revert();
GT_GUI_EXPORT QUrl revert_Url();
[[deprecated("Use revert instead")]]
GT_GUI_EXPORT QIcon restore16();
GT_GUI_EXPORT QUrl restore16_Url();

GT_GUI_EXPORT QIcon ruler();
GT_GUI_EXPORT QUrl ruler_Url();

[[deprecated("Use processRun instead")]]
GT_GUI_EXPORT QIcon runProcess16();
GT_GUI_EXPORT QUrl runProcess16_Url();

GT_GUI_EXPORT QIcon save();
GT_GUI_EXPORT QUrl save_Url();
[[deprecated("Use save instead")]]
GT_GUI_EXPORT QIcon saveProject16();
GT_GUI_EXPORT QUrl saveProject16_Url();

GT_GUI_EXPORT QIcon schedules();
GT_GUI_EXPORT QUrl schedules_Url();
GT_GUI_EXPORT QIcon schedules2();
GT_GUI_EXPORT QUrl schedules2_Url();

GT_GUI_EXPORT QIcon search();
GT_GUI_EXPORT QUrl search_Url();
[[deprecated("Use search instead")]]
GT_GUI_EXPORT QIcon search16();
GT_GUI_EXPORT QUrl search16_Url();

GT_GUI_EXPORT QIcon select();
GT_GUI_EXPORT QUrl select_Url();

GT_GUI_EXPORT QIcon server();
GT_GUI_EXPORT QUrl server_Url();

GT_GUI_EXPORT QIcon session();
GT_GUI_EXPORT QUrl session_Url();
[[deprecated("Use session instead")]]
GT_GUI_EXPORT QIcon session16();
GT_GUI_EXPORT QUrl session16_Url();

GT_GUI_EXPORT QIcon showText();
GT_GUI_EXPORT QUrl showText_Url();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT QIcon showText16();
GT_GUI_EXPORT QUrl showText16_Url();
[[deprecated("Use showText instead")]]
GT_GUI_EXPORT QIcon showText24();
GT_GUI_EXPORT QUrl showText24_Url();

GT_GUI_EXPORT QIcon skip();
GT_GUI_EXPORT QUrl skip_Url();
[[deprecated("Use skip instead")]]
GT_GUI_EXPORT QIcon skip16();
GT_GUI_EXPORT QUrl skip16_Url();
GT_GUI_EXPORT QIcon unskip();
GT_GUI_EXPORT QUrl unskip_Url();

GT_GUI_EXPORT QIcon sleep();
GT_GUI_EXPORT QUrl sleep_Url();
[[deprecated("Use sleep instead")]]
GT_GUI_EXPORT QIcon sleep16();
GT_GUI_EXPORT QUrl sleep16_Url();
GT_GUI_EXPORT QIcon sleepOff();
GT_GUI_EXPORT QUrl sleepOff_Url();

GT_GUI_EXPORT QIcon square();
GT_GUI_EXPORT QUrl square_Url();

GT_GUI_EXPORT QIcon sort();
GT_GUI_EXPORT QUrl sort_Url();
GT_GUI_EXPORT QIcon sortAsc();
GT_GUI_EXPORT QUrl sortAsc_Url();
GT_GUI_EXPORT QIcon sortDesc();
GT_GUI_EXPORT QUrl sortDesc_Url();

GT_GUI_EXPORT QIcon stop();
GT_GUI_EXPORT QUrl stop_Url();
[[deprecated("Use stop instead")]]
GT_GUI_EXPORT QIcon stop16();
GT_GUI_EXPORT QUrl stop16_Url();

GT_GUI_EXPORT QIcon stepFile();
GT_GUI_EXPORT QUrl stepFile_Url();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT QIcon stepFile16();
GT_GUI_EXPORT QUrl stepFile16_Url();
[[deprecated("Use stepFile instead")]]
GT_GUI_EXPORT QIcon stepFile24();
GT_GUI_EXPORT QUrl stepFile24_Url();

GT_GUI_EXPORT QIcon stretch();
GT_GUI_EXPORT QUrl stretch_Url();

GT_GUI_EXPORT QIcon swap();
GT_GUI_EXPORT QUrl swap_Url();

GT_GUI_EXPORT QIcon sync();
GT_GUI_EXPORT QUrl sync_Url();

GT_GUI_EXPORT QIcon table();
GT_GUI_EXPORT QUrl table_Url();
[[deprecated("Use table instead")]]
GT_GUI_EXPORT QIcon table16();
GT_GUI_EXPORT QUrl table16_Url();

GT_GUI_EXPORT QIcon text();
GT_GUI_EXPORT QUrl text_Url();

GT_GUI_EXPORT QIcon timer();
GT_GUI_EXPORT QUrl timer_Url();
GT_GUI_EXPORT QIcon timerSand();
GT_GUI_EXPORT QUrl timerSand_Url();

GT_GUI_EXPORT QIcon trace();
GT_GUI_EXPORT QUrl trace_Url();
GT_GUI_EXPORT QIcon traceColorized();
GT_GUI_EXPORT QUrl traceColorized_Url();
[[deprecated("Use trace instead")]]
GT_GUI_EXPORT QIcon trace16();
GT_GUI_EXPORT QUrl trace16_Url();

GT_GUI_EXPORT QIcon tree();
GT_GUI_EXPORT QUrl tree_Url();

GT_GUI_EXPORT QIcon triangleDown();
GT_GUI_EXPORT QUrl triangleDown_Url();
GT_GUI_EXPORT QIcon triangleLeft();
GT_GUI_EXPORT QUrl triangleLeft_Url();
GT_GUI_EXPORT QIcon triangleRight();
GT_GUI_EXPORT QUrl triangleRight_Url();
GT_GUI_EXPORT QIcon triangleRightGroup();
GT_GUI_EXPORT QUrl triangleRightGroup_Url();
GT_GUI_EXPORT QIcon triangleUp();
GT_GUI_EXPORT QUrl triangleUp_Url();
GT_GUI_EXPORT QIcon triangleSmallDown();
GT_GUI_EXPORT QUrl triangleSmallDown_Url();
GT_GUI_EXPORT QIcon triangleSmallLeft();
GT_GUI_EXPORT QUrl triangleSmallLeft_Url();
GT_GUI_EXPORT QIcon triangleSmallRight();
GT_GUI_EXPORT QUrl triangleSmallRight_Url();
GT_GUI_EXPORT QIcon triangleSmallUp();
GT_GUI_EXPORT QUrl triangleSmallUp_Url();

[[deprecated("Use perfTsDiagram instead")]]
GT_GUI_EXPORT QIcon tsDiagram16();
GT_GUI_EXPORT QUrl tsDiagram16_Url();

[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT QIcon turbine();
GT_GUI_EXPORT QUrl turbine_Url();
[[deprecated("Use objectTurbine instead")]]
GT_GUI_EXPORT QIcon turbine16();
GT_GUI_EXPORT QUrl turbine16_Url();

GT_GUI_EXPORT QIcon undo();
GT_GUI_EXPORT QUrl undo_Url();
[[deprecated("Use undo instead")]]
GT_GUI_EXPORT QIcon undo24();
GT_GUI_EXPORT QUrl undo24_Url();

GT_GUI_EXPORT QIcon update();
GT_GUI_EXPORT QUrl update_Url();
[[deprecated("Use update instead")]]
GT_GUI_EXPORT QIcon update16();
GT_GUI_EXPORT QUrl update16_Url();

GT_GUI_EXPORT QIcon upload();
GT_GUI_EXPORT QUrl upload_Url();

GT_GUI_EXPORT QIcon variable();
GT_GUI_EXPORT QUrl variable_Url();

GT_GUI_EXPORT QIcon vectorBezier();
GT_GUI_EXPORT QUrl vectorBezier_Url();
GT_GUI_EXPORT QIcon vectorBezier2();
GT_GUI_EXPORT QUrl vectorBezier2_Url();
GT_GUI_EXPORT QIcon vectorCurve();
GT_GUI_EXPORT QUrl vectorCurve_Url();
GT_GUI_EXPORT QIcon vectorLine();
GT_GUI_EXPORT QUrl vectorLine_Url();
GT_GUI_EXPORT QIcon vectorPoint();
GT_GUI_EXPORT QUrl vectorPoint_Url();
GT_GUI_EXPORT QIcon vectorPointEdit();
GT_GUI_EXPORT QUrl vectorPointEdit_Url();
GT_GUI_EXPORT QIcon vectorPoly();
GT_GUI_EXPORT QUrl vectorPoly_Url();
GT_GUI_EXPORT QIcon vectorSimple();
GT_GUI_EXPORT QUrl vectorSimple_Url();
GT_GUI_EXPORT QIcon vectorSquare();
GT_GUI_EXPORT QUrl vectorSquare_Url();
GT_GUI_EXPORT QIcon vectorSquareEdit();
GT_GUI_EXPORT QUrl vectorSquareEdit_Url();
GT_GUI_EXPORT QIcon vectorTriangle();
GT_GUI_EXPORT QUrl vectorTriangle_Url();

GT_GUI_EXPORT QIcon warning();
GT_GUI_EXPORT QUrl warning_Url();
GT_GUI_EXPORT QIcon warningColorized();
GT_GUI_EXPORT QUrl warningColorized_Url();
[[deprecated("Use warning instead")]]
GT_GUI_EXPORT QIcon warning16();
GT_GUI_EXPORT QUrl warning16_Url();

GT_GUI_EXPORT QIcon web();
GT_GUI_EXPORT QUrl web_Url();

GT_GUI_EXPORT QIcon xml();
GT_GUI_EXPORT QUrl xml_Url();

GT_GUI_EXPORT QIcon zoomMinus();
GT_GUI_EXPORT QUrl zoomMinus_Url();
GT_GUI_EXPORT QIcon zoomPlus();
GT_GUI_EXPORT QUrl zoomPlus_Url();
[[deprecated("Use zoomPlus/zoomMinus instead")]]
GT_GUI_EXPORT QIcon zoom16();
GT_GUI_EXPORT QUrl zoom16_Url();



/** DEPRECATED **/
[[deprecated("Use objectEmpty instead")]]
GT_GUI_EXPORT QIcon emptyElement();
GT_GUI_EXPORT QUrl emptyElement_Url();

[[deprecated("Use timerSand instead")]]
GT_GUI_EXPORT QIcon inProgress16();
GT_GUI_EXPORT QUrl inProgress16_Url();

[[deprecated("Use timer instead")]]
GT_GUI_EXPORT QIcon stopRequest16();
GT_GUI_EXPORT QUrl stopRequest16_Url();

[[deprecated("Use applicationVar instead")]]
GT_GUI_EXPORT QIcon pathSettings();
GT_GUI_EXPORT QUrl pathSettings_Url();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT QIcon frame();
GT_GUI_EXPORT QUrl frame_Url();

[[deprecated("Use objectUnknown instead")]]
GT_GUI_EXPORT QIcon unknown();
GT_GUI_EXPORT QUrl unknown_Url();

[[deprecated("Use close instead")]]
GT_GUI_EXPORT QIcon close24();
GT_GUI_EXPORT QUrl close24_Url();

[[deprecated("Use projectOpen instead")]]
GT_GUI_EXPORT QIcon components16();
GT_GUI_EXPORT QUrl components16_Url();

[[deprecated("Use chartLine instead")]]
GT_GUI_EXPORT QIcon post();
GT_GUI_EXPORT QUrl post_Url();

[[deprecated("Use label instead")]]
GT_GUI_EXPORT QIcon labels();
GT_GUI_EXPORT QUrl labels_Url();

[[deprecated("Use layers instead")]]
GT_GUI_EXPORT QIcon stack();
GT_GUI_EXPORT QUrl stack_Url();

[[deprecated("Use pdf instead")]]
GT_GUI_EXPORT QIcon printPDF();
GT_GUI_EXPORT QUrl printPDF_Url();

[[deprecated("Use update instead")]]
GT_GUI_EXPORT QIcon upgradeProjectData();
GT_GUI_EXPORT QUrl upgradeProjectData_Url();

[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move();
GT_GUI_EXPORT QUrl move_Url();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move16();
GT_GUI_EXPORT QUrl move16_Url();
[[deprecated("Use cursorMove instead")]]
GT_GUI_EXPORT QIcon move24();
GT_GUI_EXPORT QUrl move24_Url();

[[deprecated("Use mathPlus instead")]]
GT_GUI_EXPORT QIcon plus();
GT_GUI_EXPORT QUrl plus_Url();
[[deprecated("Use mathMinus instead")]]
GT_GUI_EXPORT QIcon minus();
GT_GUI_EXPORT QUrl minus_Url();
[[deprecated("Use mathMultiplication instead")]]
GT_GUI_EXPORT QIcon multiply();
GT_GUI_EXPORT QUrl multiply_Url();
[[deprecated("Use mathDivision instead")]]
GT_GUI_EXPORT QIcon divide();
GT_GUI_EXPORT QUrl divide_Url();
[[deprecated("Use mathRoot instead")]]
GT_GUI_EXPORT QIcon squareRoot();
GT_GUI_EXPORT QUrl squareRoot_Url();

[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn();
GT_GUI_EXPORT QUrl fileIn_Url();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn16();
GT_GUI_EXPORT QUrl fileIn16_Url();
[[deprecated("Use fileImport instead")]]
GT_GUI_EXPORT QIcon fileIn24();
GT_GUI_EXPORT QUrl fileIn24_Url();

[[deprecated("Use text instead")]]
GT_GUI_EXPORT QIcon textNote24();
GT_GUI_EXPORT QUrl textNote24_Url();

[[deprecated("Use vectorTriangle instead")]]
GT_GUI_EXPORT QIcon polyCurve();
GT_GUI_EXPORT QUrl polyCurve_Url();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT QIcon bezierCurve();
GT_GUI_EXPORT QUrl bezierCurve_Url();
[[deprecated("Use vectorBezier instead")]]
GT_GUI_EXPORT QIcon simpleCurve();
GT_GUI_EXPORT QUrl simpleCurve_Url();

[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp();
GT_GUI_EXPORT QUrl dataBaseArrowUp_Url();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp16();
GT_GUI_EXPORT QUrl dataBaseArrowUp16_Url();
[[deprecated("Use dataArrowUp instead")]]
GT_GUI_EXPORT QIcon dataBaseArrowUp24();
GT_GUI_EXPORT QUrl dataBaseArrowUp24_Url();

[[deprecated("Use noteEdit instead")]]
GT_GUI_EXPORT QIcon editPen();
GT_GUI_EXPORT QUrl editPen_Url();

[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT QIcon addElement16();
GT_GUI_EXPORT QUrl addElement16_Url();
[[deprecated("Use processAdd instead")]]
GT_GUI_EXPORT QIcon addProcess16();
GT_GUI_EXPORT QUrl addProcess16_Url();

[[deprecated("use download instead")]]
GT_GUI_EXPORT QIcon dropdown();
GT_GUI_EXPORT QUrl dropdown_Url();

[[deprecated("Use dataSingle instead")]]
GT_GUI_EXPORT QIcon globals16();
GT_GUI_EXPORT QUrl globals16_Url();

[[deprecated("Use chartXY instead")]]
GT_GUI_EXPORT QIcon xyPlot16();
GT_GUI_EXPORT QUrl xyPlot16_Url();

[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT QIcon engine();
GT_GUI_EXPORT QUrl engine_Url();
[[deprecated("Use objectEngine instead")]]
GT_GUI_EXPORT QIcon engine16();
GT_GUI_EXPORT QUrl engine16_Url();

[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT QIcon combustor();
GT_GUI_EXPORT QUrl combustor_Url();
[[deprecated("Use objectCombustor instead")]]
GT_GUI_EXPORT QIcon combustor16();
GT_GUI_EXPORT QUrl combustor16_Url();

[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT QIcon compressor();
GT_GUI_EXPORT QUrl compressor_Url();
[[deprecated("Use objectCompressor instead")]]
GT_GUI_EXPORT QIcon compressor16();
GT_GUI_EXPORT QUrl compressor16_Url();

[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT QIcon speedLine();
GT_GUI_EXPORT QUrl speedLine_Url();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT QIcon speedLine16();
GT_GUI_EXPORT QUrl speedLine16_Url();
[[deprecated("Use perfSpeedLine instead")]]
GT_GUI_EXPORT QIcon speedLine24();
GT_GUI_EXPORT QUrl speedLine24_Url();

[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT QIcon performanceOffDesign();
GT_GUI_EXPORT QUrl performanceOffDesign_Url();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT QIcon performanceOffDesign16();
GT_GUI_EXPORT QUrl performanceOffDesign16_Url();
[[deprecated("Use perfOperatingPoint instead")]]
GT_GUI_EXPORT QIcon performanceOffDesign24();
GT_GUI_EXPORT QUrl performanceOffDesign24_Url();

[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT QIcon map();
GT_GUI_EXPORT QUrl map_Url();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT QIcon map16();
GT_GUI_EXPORT QUrl map16_Url();
[[deprecated("Use perfMap instead")]]
GT_GUI_EXPORT QIcon map24();
GT_GUI_EXPORT QUrl map24_Url();

[[deprecated("Use perfNoMap instead")]]
GT_GUI_EXPORT QIcon noMap();
GT_GUI_EXPORT QUrl noMap_Url();

[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT QIcon exportMap();
GT_GUI_EXPORT QUrl exportMap_Url();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT QIcon exportMap16();
GT_GUI_EXPORT QUrl exportMap16_Url();
[[deprecated("Use perfMapExport instead")]]
GT_GUI_EXPORT QIcon exportMap24();
GT_GUI_EXPORT QUrl exportMap24_Url();

[[deprecated("Use letter::d instead")]]
GT_GUI_EXPORT QIcon double16();
GT_GUI_EXPORT QUrl double16_Url();
[[deprecated("Use letter::i instead")]]
GT_GUI_EXPORT QIcon int16();
GT_GUI_EXPORT QUrl int16_Url();
[[deprecated("Use letter::o instead")]]
GT_GUI_EXPORT QIcon o16();
GT_GUI_EXPORT QUrl o16_Url();
[[deprecated("Use letter::s instead")]]
GT_GUI_EXPORT QIcon string16();
GT_GUI_EXPORT QUrl string16_Url();

/** OTHER **/
[[deprecated("Use perfModel instead")]]
GT_GUI_EXPORT QIcon performanceModel();
GT_GUI_EXPORT QUrl performanceModel_Url();

[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT QIcon performanceDesign();
GT_GUI_EXPORT QUrl performanceDesign_Url();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT QIcon performanceDesign16();
GT_GUI_EXPORT QUrl performanceDesign16_Url();
[[deprecated("Use perfDesignPoint instead")]]
GT_GUI_EXPORT QIcon performanceDesign24();
GT_GUI_EXPORT QUrl performanceDesign24_Url();

GT_GUI_EXPORT QIcon engineInstallation();
GT_GUI_EXPORT QUrl engineInstallation_Url();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT QIcon engineInstallation16();
GT_GUI_EXPORT QUrl engineInstallation16_Url();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT QIcon engineInstallation24();
GT_GUI_EXPORT QUrl engineInstallation24_Url();
[[deprecated("Use engineInstallation instead")]]
GT_GUI_EXPORT QIcon engineInstallation32();
GT_GUI_EXPORT QUrl engineInstallation32_Url();

GT_GUI_EXPORT QIcon carpetPlot();
GT_GUI_EXPORT QUrl carpetPlot_Url();

GT_GUI_EXPORT QIcon bleedInPort();
GT_GUI_EXPORT QUrl bleedInPort_Url();
GT_GUI_EXPORT QIcon bleedInPort16();
GT_GUI_EXPORT QUrl bleedInPort16_Url();
GT_GUI_EXPORT QIcon bleedInPort24();
GT_GUI_EXPORT QUrl bleedInPort24_Url();

GT_GUI_EXPORT QIcon bleedOutPort();
GT_GUI_EXPORT QUrl bleedOutPort_Url();
GT_GUI_EXPORT QIcon bleedOutPort16();
GT_GUI_EXPORT QUrl bleedOutPort16_Url();
GT_GUI_EXPORT QIcon bleedOutPort24();
GT_GUI_EXPORT QUrl bleedOutPort24_Url();

GT_GUI_EXPORT QIcon bleedPortGroup();
GT_GUI_EXPORT QUrl bleedPortGroup_Url();
GT_GUI_EXPORT QIcon bleedPortGroup16();
GT_GUI_EXPORT QUrl bleedPortGroup16_Url();
GT_GUI_EXPORT QIcon bleedPortGroup24();
GT_GUI_EXPORT QUrl bleedPortGroup24_Url();

GT_GUI_EXPORT QIcon fluidPort();
GT_GUI_EXPORT QUrl fluidPort_Url();
GT_GUI_EXPORT QIcon fluidPort16();
GT_GUI_EXPORT QUrl fluidPort16_Url();
GT_GUI_EXPORT QIcon fluidPort24();
GT_GUI_EXPORT QUrl fluidPort24_Url();

GT_GUI_EXPORT QIcon fluidPortGroup();
GT_GUI_EXPORT QUrl fluidPortGroup_Url();
GT_GUI_EXPORT QIcon fluidPortGroup16();
GT_GUI_EXPORT QUrl fluidPortGroup16_Url();
GT_GUI_EXPORT QIcon fluidPortGroup24();
GT_GUI_EXPORT QUrl fluidPortGroup24_Url();

GT_GUI_EXPORT QIcon shaftPort();
GT_GUI_EXPORT QUrl shaftPort_Url();
GT_GUI_EXPORT QIcon shaftPort16();
GT_GUI_EXPORT QUrl shaftPort16_Url();
GT_GUI_EXPORT QIcon shaftPort24();
GT_GUI_EXPORT QUrl shaftPort24_Url();

GT_GUI_EXPORT QIcon processRunningIcon(int progress);

/// namespace for letter icons (standard is blue)
namespace letter {

GT_GUI_EXPORT QIcon a();
GT_GUI_EXPORT QUrl a_Url();
GT_GUI_EXPORT QIcon aSmall();
GT_GUI_EXPORT QUrl aSmall_Url();

GT_GUI_EXPORT QIcon b();
GT_GUI_EXPORT QUrl b_Url();
GT_GUI_EXPORT QIcon bSmall();
GT_GUI_EXPORT QUrl bSmall_Url();

GT_GUI_EXPORT QIcon c();
GT_GUI_EXPORT QUrl c_Url();
GT_GUI_EXPORT QIcon cSmall();
GT_GUI_EXPORT QUrl cSmall_Url();

GT_GUI_EXPORT QIcon d();
GT_GUI_EXPORT QUrl d_Url();
GT_GUI_EXPORT QIcon dSmall();
GT_GUI_EXPORT QUrl dSmall_Url();

GT_GUI_EXPORT QIcon e();
GT_GUI_EXPORT QUrl e_Url();
GT_GUI_EXPORT QIcon eSmall();
GT_GUI_EXPORT QUrl eSmall_Url();
GT_GUI_EXPORT QIcon eColorized();
GT_GUI_EXPORT QUrl eColorized_Url();
GT_GUI_EXPORT QIcon eSmallColorized();
GT_GUI_EXPORT QUrl eSmallColorized_Url();
[[deprecated("Use eColorized instead")]]
GT_GUI_EXPORT QIcon eBrown();
GT_GUI_EXPORT QUrl eBrown_Url();
[[deprecated("Use eSmallColorized instead")]]
GT_GUI_EXPORT QIcon eBrownSmall();
GT_GUI_EXPORT QUrl eBrownSmall_Url();

GT_GUI_EXPORT QIcon f();
GT_GUI_EXPORT QUrl f_Url();
GT_GUI_EXPORT QIcon fSmall();
GT_GUI_EXPORT QUrl fSmall_Url();

GT_GUI_EXPORT QIcon g();
GT_GUI_EXPORT QUrl g_Url();
GT_GUI_EXPORT QIcon gSmall();
GT_GUI_EXPORT QUrl gSmall_Url();

GT_GUI_EXPORT QIcon h();
GT_GUI_EXPORT QUrl h_Url();
GT_GUI_EXPORT QIcon hSmall();
GT_GUI_EXPORT QUrl hSmall_Url();

GT_GUI_EXPORT QIcon i();
GT_GUI_EXPORT QUrl i_Url();
GT_GUI_EXPORT QIcon iSmall();
GT_GUI_EXPORT QUrl iSmall_Url();

GT_GUI_EXPORT QIcon j();
GT_GUI_EXPORT QUrl j_Url();
GT_GUI_EXPORT QIcon jSmall();
GT_GUI_EXPORT QUrl jSmall_Url();

GT_GUI_EXPORT QIcon k();
GT_GUI_EXPORT QUrl k_Url();
GT_GUI_EXPORT QIcon kSmall();
GT_GUI_EXPORT QUrl kSmall_Url();

GT_GUI_EXPORT QIcon l();
GT_GUI_EXPORT QUrl l_Url();
GT_GUI_EXPORT QIcon lSmall();
GT_GUI_EXPORT QUrl lSmall_Url();

GT_GUI_EXPORT QIcon m();
GT_GUI_EXPORT QUrl m_Url();
GT_GUI_EXPORT QIcon mSmall();
GT_GUI_EXPORT QUrl mSmall_Url();

GT_GUI_EXPORT QIcon n();
GT_GUI_EXPORT QUrl n_Url();
GT_GUI_EXPORT QIcon nSmall();
GT_GUI_EXPORT QUrl nSmall_Url();

GT_GUI_EXPORT QIcon o();
GT_GUI_EXPORT QUrl o_Url();
GT_GUI_EXPORT QIcon oSmall();
GT_GUI_EXPORT QUrl oSmall_Url();

GT_GUI_EXPORT QIcon p();
GT_GUI_EXPORT QUrl p_Url();
GT_GUI_EXPORT QIcon pSmall();
GT_GUI_EXPORT QUrl pSmall_Url();

GT_GUI_EXPORT QIcon q();
GT_GUI_EXPORT QUrl q_Url();
GT_GUI_EXPORT QIcon qSmall();
GT_GUI_EXPORT QUrl qSmall_Url();

GT_GUI_EXPORT QIcon r();
GT_GUI_EXPORT QUrl r_Url();
GT_GUI_EXPORT QIcon rSmall();
GT_GUI_EXPORT QUrl rSmall_Url();

GT_GUI_EXPORT QIcon s();
GT_GUI_EXPORT QUrl s_Url();
GT_GUI_EXPORT QIcon sSmall();
GT_GUI_EXPORT QUrl sSmall_Url();

GT_GUI_EXPORT QIcon t();
GT_GUI_EXPORT QUrl t_Url();
GT_GUI_EXPORT QIcon tSmall();
GT_GUI_EXPORT QUrl tSmall_Url();

GT_GUI_EXPORT QIcon u();
GT_GUI_EXPORT QUrl u_Url();
GT_GUI_EXPORT QIcon uSmall();
GT_GUI_EXPORT QUrl uSmall_Url();

GT_GUI_EXPORT QIcon v();
GT_GUI_EXPORT QUrl v_Url();
GT_GUI_EXPORT QIcon vSmall();
GT_GUI_EXPORT QUrl vSmall_Url();

GT_GUI_EXPORT QIcon w();
GT_GUI_EXPORT QUrl w_Url();
GT_GUI_EXPORT QIcon wSmall();
GT_GUI_EXPORT QUrl wSmall_Url();

GT_GUI_EXPORT QIcon x();
GT_GUI_EXPORT QUrl x_Url();
GT_GUI_EXPORT QIcon xSmall();
GT_GUI_EXPORT QUrl xSmall_Url();

GT_GUI_EXPORT QIcon y();
GT_GUI_EXPORT QUrl y_Url();
GT_GUI_EXPORT QIcon ySmall();
GT_GUI_EXPORT QUrl ySmall_Url();

GT_GUI_EXPORT QIcon z();
GT_GUI_EXPORT QUrl z_Url();
GT_GUI_EXPORT QIcon zSmall();
GT_GUI_EXPORT QUrl zSmall_Url();

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

#endif // GTICONS_H
