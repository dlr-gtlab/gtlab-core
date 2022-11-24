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

GT_GUI_EXPORT QIcon map16();

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
