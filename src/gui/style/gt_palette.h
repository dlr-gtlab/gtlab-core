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

namespace gt
{

// namespace for the basic paletts to use for GTlab Application
namespace gui
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
GT_GUI_EXPORT void applyThemeToApplication();

} // namespace gui

} // namespace gt

#endif // GTPALETTE_H
