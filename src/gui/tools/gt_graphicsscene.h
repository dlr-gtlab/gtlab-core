/* GTlab - Gas Turbine laboratory
 * Source File: gtd_graphicsscene.h
 * copyright 2009-2013 by DLR
 *
 *  Created on: 19.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_GRAPHICSSCENE_H
#define GTD_GRAPHICSSCENE_H

#include <QGraphicsScene>
#include "gt_gui_exports.h"

class GtGraphicsView;

class GT_GUI_EXPORT GtGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GtGraphicsScene(QObject *parent = 0);

//    void setPointSelectionMode(bool val);

    template <class T>
    QList<T> findItems()
    {
        QList<T> retval;

        foreach (QGraphicsItem* item, items())
        {
            T obj = dynamic_cast<T>(item);

            if (obj)
            {
                retval.append(obj);
            }
        }

        return retval;
    }
    
};

#endif // GTD_GRAPHICSSCENE_H
