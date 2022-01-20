/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionloadingwidget.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 19.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QMovie>

#include "gt_collectionloadingwidget.h"

GtCollectionLoadingWidget::GtCollectionLoadingWidget(QWidget* parent) :
    QWidget(parent),
    m_anim(new QMovie(QStringLiteral(":/pixmaps/loading.gif")))
{
    m_anim->setParent(this);

    QGridLayout* lay = new QGridLayout;

    QVBoxLayout* vLay = new QVBoxLayout;
    vLay->setContentsMargins(0, 0, 0, 0);
    QLabel* lbl = new QLabel;

    vLay->addWidget(lbl);

    QLabel* loadingLabel = new QLabel;
    loadingLabel->setMovie(m_anim);

    vLay->addWidget(loadingLabel);


    lay->addLayout(vLay, 1, 1, 1, 1);

    QSpacerItem* verticalSpacer_2 =
            new QSpacerItem(1, 1, QSizePolicy::Minimum,
                            QSizePolicy::Expanding);

    lay->addItem(verticalSpacer_2, 0, 1, 1, 1);

    QSpacerItem* verticalSpacer_3 =
            new QSpacerItem(1, 1, QSizePolicy::Minimum,
                            QSizePolicy::Expanding);

    lay->addItem(verticalSpacer_3, 2, 1, 1, 1);

    QSpacerItem* horizontalSpacer_2 =
            new QSpacerItem(1, 1, QSizePolicy::Expanding,
                            QSizePolicy::Minimum);

    lay->addItem(horizontalSpacer_2, 1, 2, 1, 1);

    QSpacerItem* horizontalSpacer =
           new QSpacerItem(1, 1, QSizePolicy::Expanding,
                           QSizePolicy::Minimum);

    lay->addItem(horizontalSpacer, 1, 0, 1, 1);

    setLayout(lay);
}

void
GtCollectionLoadingWidget::runAnimation()
{
    m_anim->start();
}

void
GtCollectionLoadingWidget::stopAnimation()
{
    m_anim->stop();
}
