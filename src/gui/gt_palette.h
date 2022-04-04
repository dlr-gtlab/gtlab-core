/* GTlab - Gas Turbine laboratory
 * Source File: gt_palette.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTPALETTE_H
#define GTPALETTE_H

#include "gt_gui_exports.h"
#include <QPalette>

class QWidget;
/**
 * namespace for the basic paletts to use for GTlab Application
 */
namespace GtPalette
{
    /**
     * @brief currentTheme
     * @return the current theme depending on the application settings
     */
    GT_GUI_EXPORT QPalette currentTheme();

    /**
     * @brief darkTheme
     * @return a dark theme
     */
    GT_GUI_EXPORT QPalette darkTheme();

    /**
     * @brief standardTheme
     * @return a normal standard theme
     */
    GT_GUI_EXPORT QPalette standardTheme();

    /**
     * @brief applyThemeToWidget
     * @param w - widget to manipulate
     */
    GT_GUI_EXPORT void applyThemeToWidget(QWidget* w);

    namespace Color {
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

    }

}

#endif // GTPALETTE_H
