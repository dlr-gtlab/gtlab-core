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

    GT_GUI_EXPORT QColor dummyObjectBackground();

    GT_GUI_EXPORT QColor newObjectForground();

    GT_GUI_EXPORT QColor changedObjectForground();

    GT_GUI_EXPORT QColor footprintWarning();

    GT_GUI_EXPORT QColor footprintError();

    GT_GUI_EXPORT QColor infoText();

    GT_GUI_EXPORT QColor warningText();

    GT_GUI_EXPORT QColor errorText();

    GT_GUI_EXPORT QColor fatalText();

    GT_GUI_EXPORT QColor fatalTextBackground();

    GT_GUI_EXPORT QColor collectionAvailableItemBackground();

    GT_GUI_EXPORT QColor collectionInstalledItemBackground();

    GT_GUI_EXPORT QColor environmentModelBack();

    }

}

#endif // GTPALETTE_H
