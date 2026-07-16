/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_graphicsscene.h
 *
 *  Created on: 19.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#ifndef GTD_GRAPHICSSCENE_H
#define GTD_GRAPHICSSCENE_H

#include <QGraphicsScene>

#include <gt_gui_exports.h>
#include <gt_version.h>
#include <gt_guiutilities.h>

class GtGraphicsView;

class GT_GUI_EXPORT GtGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `QGraphicsScene` instead.")
    explicit GtGraphicsScene(QObject *parent = 0);

    template <class T>
    GT_DEPRECATED_ATTR(2, 2, "Function will be removed, "
                             "use `gt::gui::findGraphicItems<T>` "
                             "(gt_guiutilities.h) instead.")
    QList<T> findItems()
    {
        return gt::gui::findGraphicItems<T>(*this);
    }
    
};

#endif // GTD_GRAPHICSSCENE_H
