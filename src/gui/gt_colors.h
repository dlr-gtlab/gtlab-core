/* GTlab - Gas Turbine laboratory
 * Source File: gt_colors.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTCOLORS_H
#define GTCOLORS_H

#include "gt_gui_exports.h"
#include <QColor>

class QPainter;
/**
 * namespace for basic gui elements for GTlab Application
 */
namespace gt
{
namespace gui
{
namespace color
{

/**
 * @brief Palette main color
 * @return
 */
GT_GUI_EXPORT QColor main();

/**
 * @brief Palette base color
 * @return
 */
GT_GUI_EXPORT QColor base();

/**
 * @brief Palette text color
 * @return
 */
GT_GUI_EXPORT QColor text();

/**
 * @brief Palette disabled color
 * @return
 */
GT_GUI_EXPORT QColor disabled();

/**
 * @brief Palette highlight color
 * @return
 */
GT_GUI_EXPORT QColor highlight();

/**
 * @brief Palette highlighted text color
 * @return
 */
GT_GUI_EXPORT QColor textHighlight();

/**
 * @brief basicDarkColor
 * @return return basic dark color
 */
GT_GUI_EXPORT QColor basicDark();

/**
 * @brief dummyObjectBackground
 * @return color of objects in the datamodel view dock widget
 * which are dummy objects
 */
GT_GUI_EXPORT QColor dummyObjectBackground();

/**
 * @brief newObjectForground
 * @return foreground color of objects in the datamodel view widget
 * which are new
 */
GT_GUI_EXPORT QColor newObjectForground();

/**
 * @brief newObjectForground
 * @return foreground color of objects in the datamodel view widget
 * which are changed
 */
GT_GUI_EXPORT QColor changedObjectForground();

/**
 * @brief footprintWarning
 * @return color for warnings in the footprint dialog
 */
GT_GUI_EXPORT QColor footprintWarning();

/**
 * @brief footprintError
 * @return color for errors in the footprint dialog
 */
GT_GUI_EXPORT QColor footprintError();

/**
 * @brief debugText
 * @return color for debug texts
 */
GT_GUI_EXPORT QColor debugText();

/**
 * @brief infoText
 * @return color for simple info texts
 */
GT_GUI_EXPORT QColor infoText();

/**
 * @brief warningText
 * @return color for warning texts (as in the output dock widget)
 */
GT_GUI_EXPORT QColor warningText();

/**
 * @brief errorText
 * @return color for error texts (as in the output dock widget)
 */
GT_GUI_EXPORT QColor errorText();

/**
 * @brief fatalText
 * @return color for fatal texts (as in the output dock widget)
 */
GT_GUI_EXPORT QColor fatalText();

/**
 * @brief fatalTextBackground
 * @return color for the background of fatal texts
 * (as in the output dock widget)
 */
GT_GUI_EXPORT QColor fatalTextBackground();

/**
 * @brief collectionAvailableItemBackground
 * @return background color for the collection view for
 * available items
 */
GT_GUI_EXPORT QColor collectionAvailableItemBackground();

/**
 * @brief collectionInstalledItemBackground
 * @return background color for the collection view for
 * installed items
 */
GT_GUI_EXPORT QColor collectionInstalledItemBackground();

/**
 * @brief environmentModelBack
 * @return color for the background of the environmental model view
 */
GT_GUI_EXPORT QColor environmentModelBack();

GT_GUI_EXPORT void setPaintertoGray(QPainter* painter);

GT_GUI_EXPORT QColor randomColor();

GT_GUI_EXPORT QColor gridLineColor();

GT_GUI_EXPORT QColor gridPointColor();

} // namespace color

} // namespace gui

} // namespace gt

#endif // GTCOLORS_H
