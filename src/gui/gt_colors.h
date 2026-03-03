/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_colors.cpp
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTCOLORS_H
#define GTCOLORS_H

#include "gt_gui_exports.h"

#include <gt_version.h>

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
 * @brief Makes the color lighter (or darker) depending on the current theme
 * mode. Amount denotes the value to lighten the color. The value will be
 * applied additively and not percentage wise, therefore the amount maybe in
 * range of 0 and 255. Negative values can be used to darken the color
 * @param color Color to lighten
 * @param amount Value to lighten the color by. May be negative to darken the
 * color
 * @return New color
 */
GT_GUI_EXPORT QColor lighten(QColor const& color, int amount);

/**
 * @brief Desaturates the given color by the specified multiplier.
 * @param color The color to be desaturated.
 * @param multiplier The amount of desaturation to be applied.
 * 0 = meaning no changes.
 * @return A new QColor object representing the desaturated color.
 */
GT_GUI_EXPORT QColor desaturate(QColor const& color, double multiplier);


/**
 * @brief Same as lighten, but makes the color darker depending on the current
 * theme. The value will be applied additively
 * @param color Color to darken
 * @param amount Value to darken the color by. May be negative to lighten the
 * color
 * @return New color
 */
inline QColor darken(QColor const& color, int amount)
{
    return lighten(color, amount * -1);
}

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
 * @brief Color for frames with a heavy constrast (e.g. lineedits or checkboxes)
 * @return
 */
GT_GUI_EXPORT QColor frame();

/**
 * @brief Color for frames with a light contrast (e.g. for tabwidgets or views)
 * @return
 */
GT_GUI_EXPORT QColor lightFrame();

/**
 * @brief Color for title labels in viewers etc.
 * @return
 */
GT_GUI_EXPORT QColor titleLabelBackground();

/**
 * @brief Color for standard info labels.
 * @return
 */
GT_GUI_EXPORT QColor infoLabelBackground();

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
 * @brief newObjectForeground
 * @return foreground color of objects in the datamodel view widget
 * which are new
 */
GT_GUI_EXPORT QColor newObjectForeground();

/**
 * @brief newObjectForeground
 * @return foreground color of objects in the datamodel view widget
 * which are changed
 */
GT_GUI_EXPORT QColor changedObjectForeground();

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

GT_GUI_EXPORT QColor gridLine();

GT_GUI_EXPORT QColor gridLineMinor();

GT_GUI_EXPORT QColor gridPoint();

GT_GUI_EXPORT QColor gridAxis();

namespace connection_editor
{
GT_GUI_EXPORT QColor connection();
GT_GUI_EXPORT QColor connectionDraft();
GT_GUI_EXPORT QColor connectionHighlight();
GT_GUI_EXPORT QColor portBackground();
GT_GUI_EXPORT QColor portHover();
} // connection_editor

namespace code_editor {
GT_GUI_EXPORT QColor highlightLine();
} // namespace code_editor

namespace xml_highlight {
GT_GUI_EXPORT QColor syntaxChar();
GT_GUI_EXPORT QColor elementName();
GT_GUI_EXPORT QColor comment();
GT_GUI_EXPORT QColor attributeName();
GT_GUI_EXPORT QColor attributeValue();
GT_GUI_EXPORT QColor error();
GT_GUI_EXPORT QColor other();
} // namespace xml_highlight

namespace js_highlight {
GT_GUI_EXPORT QColor normal();
GT_GUI_EXPORT QColor comment();
GT_GUI_EXPORT QColor number();
GT_GUI_EXPORT QColor string();
GT_GUI_EXPORT QColor operator_();
GT_GUI_EXPORT QColor identifier();
GT_GUI_EXPORT QColor keyword();
GT_GUI_EXPORT QColor builtIn();
GT_GUI_EXPORT QColor marker();
} // namespace js_highlight

/// Deprecated functions
GT_DEPRECATED_REMOVED_IN(2, 1, "Use gridLine instead")
GT_GUI_EXPORT QColor gridLineColor();

GT_DEPRECATED_REMOVED_IN(2, 1, "Use gridPoint instead")
GT_GUI_EXPORT QColor gridPointColor();

GT_DEPRECATED_REMOVED_IN(2, 1, "Use newObjectForeground instead")
GT_GUI_EXPORT QColor newObjectForground();

GT_DEPRECATED_REMOVED_IN(2, 1, "Use changedObjectForeground instead")
GT_GUI_EXPORT QColor changedObjectForground();

} // namespace color

} // namespace gui

} // namespace gt

#endif // GTCOLORS_H
