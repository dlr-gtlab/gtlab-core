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

        GT_GUI_EXPORT QIcon deleteProject16();

        GT_GUI_EXPORT QIcon info16();

        GT_GUI_EXPORT QIcon infoBlue16();

        GT_GUI_EXPORT QIcon runProcess16();

        GT_GUI_EXPORT QIcon stop16();

        GT_GUI_EXPORT QIcon addProcess16();

        GT_GUI_EXPORT QIcon check16();

        GT_GUI_EXPORT QIcon clear16();

        GT_GUI_EXPORT QIcon config16();

        GT_GUI_EXPORT QIcon input16();

        GT_GUI_EXPORT QIcon plugin16();

        GT_GUI_EXPORT QIcon session16();

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

        GT_GUI_EXPORT QIcon processFailed16();

        GT_GUI_EXPORT QIcon property16();

        GT_GUI_EXPORT QIcon log16();

        GT_GUI_EXPORT QIcon open16();

        GT_GUI_EXPORT QIcon skip16();
    }

}
#endif // GTICONS_H
