/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTGRAPHICSFADEOUTEFFECT_H
#define GTGRAPHICSFADEOUTEFFECT_H

#include "gt_mdi_exports.h"

#include <QGraphicsOpacityEffect>

class QPropertyAnimation;

class GT_MDI_EXPORT GtGraphicsFadeOutEffect : public QGraphicsOpacityEffect
{
    Q_OBJECT

public:
    GtGraphicsFadeOutEffect(QObject* parent = Q_NULLPTR);

private:
    QPropertyAnimation* m_anim;

signals:
    void finished();

};

#endif // GTGRAPHICSFADEOUTEFFECT_H
