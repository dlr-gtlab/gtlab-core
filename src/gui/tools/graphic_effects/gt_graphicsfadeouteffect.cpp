/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 10.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPropertyAnimation>

#include "gt_graphicsfadeouteffect.h"

GtGraphicsFadeOutEffect::GtGraphicsFadeOutEffect(QObject* parent) :
    QGraphicsOpacityEffect(parent)
{
    setOpacity(1.0);

    m_anim = new QPropertyAnimation(this, "opacity");
    m_anim->setDuration(300);
    m_anim->setKeyValueAt(0, 1.0);
    m_anim->setKeyValueAt(1, 0.0);

    connect(m_anim, SIGNAL(finished()), SIGNAL(finished()));

    m_anim->start(QAbstractAnimation::DeleteWhenStopped);
}

