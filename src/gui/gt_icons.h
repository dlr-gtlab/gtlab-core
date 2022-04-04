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

namespace GtGUI
{
    /** Creates a new icon from a resource file.
     *  @param iconPath Relative or custom resource path to a icon file.
     */
    GT_GUI_EXPORT QIcon icon(const QString &iconPath);

    namespace Icon {
        /**
         * @brief add16
         * @return add icon i the size of 16x16
         */
        GT_GUI_EXPORT QIcon add16();

        GT_GUI_EXPORT QIcon delete16();

        GT_GUI_EXPORT QIcon error16();

        GT_GUI_EXPORT QIcon export16();

        GT_GUI_EXPORT QIcon import16();

        GT_GUI_EXPORT QIcon saveProject16();
    }

}
#endif // GTICONS_H
