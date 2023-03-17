/* GTlab - Gas Turbine laboratory
 * Source File: gt_stylesheets.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#ifndef GTSTYLESHEETS_H
#define GTSTYLESHEETS_H

#include "gt_gui_exports.h"

#include <QString>
#include <QColor>

namespace gt
{
namespace gui
{
// StyleSheets for typical use in GTlab to match the corporate design
namespace stylesheet
{

enum class RunButtonState
{
    NotSelected = 0,
    StopProcess,
    QueueProcess,
    RunProcess
};

/**
* @brief buttonStyleSheet
* @return the standard button style sheet (rounded edges,
* white background, min-heigth 22...)
*/
QString GT_GUI_EXPORT buttonStyleSheet();
QString GT_GUI_EXPORT buttonStyleSheet(const QColor& baseColor);

/**
* @brief buttonStyleSheet
* @return the standard button style sheet (rounded edges,
* white background, min-heigth 20...)
*/
[[deprecated("Use buttonStyleSheet instead with color::base")]]
QString GT_GUI_EXPORT buttonStyleSheet2();

/**
 * @brief performanceTaskElementDelBtn
 * @return
 */
[[deprecated("Use buttonStyleSheet instead and use 'setFlat(true)'")]]
QString GT_GUI_EXPORT performanceTaskElementDelBtn();

/**
 * @brief processRunButton -
 *  Return the Style-Sheet for the run-process-button with a specification
 * for the color
 * @param state - current state if the process is running
 * or not or another one is running
 * @return the stylesheet
 *
 */
QString GT_GUI_EXPORT processRunButton(RunButtonState const& state);

/**
 * @brief performanceTaskLineEdit0
 * @return
 */
[[deprecated("Use standardLineEdit instead and use 'setFrame(false)'")]]
QString GT_GUI_EXPORT performanceTaskLineEdit0();

/**
 * @brief standardLineEdit -
 * (rounded corners, white backgorund...)
 * @return
 */
QString GT_GUI_EXPORT standardLineEdit();

/**
 * @brief StyleSheet for warning labels
 * @return
 */
QString GT_GUI_EXPORT warningLabel();

/**
 * @brief StyleSheet for info labels
 * @return
 */
QString GT_GUI_EXPORT standardLabel();

/**
 * @brief Standard title line styleSheet for viewers etc.
 * @return
 */
QString GT_GUI_EXPORT titleLabel();
[[deprecated("Use titleLabel instead")]]
QString GT_GUI_EXPORT resultViewerTitleLabel();

/**
 * @brief Prefered stylesheet for comboboxes. Does not set min and max width
 * @return
 */
QString GT_GUI_EXPORT comboBox();

/**
 * @brief Prefered stylesheet for comboboxes
 * @param minWidth Min width of combobox
 * @param maxWidth Max width of combobox
 * @return
 */
[[deprecated("Use comboBox instead and set min-max width manually on widget")]]
QString GT_GUI_EXPORT selectionComboBox(const QString& minWidth,
                                        const QString& maxWidth);

/**
 * @brief Prefered stylesheet for spinboxes
 * @return
 */
QString GT_GUI_EXPORT spinbox();

/**
 * @brief Stylesheet for tooltips
 * @return
 */
QString GT_GUI_EXPORT toolTip();

/**
 * @brief coloredCarpetPlotBtn - gives carpet plot button a color
 * @return
 */
QString GT_GUI_EXPORT coloredCarpetPlotBtn(const QColor& col);

/**
 * @brief backgroundFrame - Applies the background image to a QFrame
 * @return Stylesheet for QFrame
 */
QString GT_GUI_EXPORT backgroundFrame();

} // namespace stylesheet

} // namespace gui

} // namespace gt

#endif // GTSTYLESHEETS_H
