/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stylesheets.cpp
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
* @brief Standard button stylesheet.
* @return the standard button style sheet (rounded edges,
* white background, min-heigth 22...)
*/
QString GT_GUI_EXPORT button();
/**
* @brief Standard button stylesheet. Accepts a QColor as an Input which will be
* used for the background and the different hover effects
* @return the standard button style sheet
*/
QString GT_GUI_EXPORT button(const QColor& baseColor);

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

/**
 * @brief Prefered stylesheet for comboboxes. Does not set min and max width
 * @return
 */
QString GT_GUI_EXPORT comboBox();

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
