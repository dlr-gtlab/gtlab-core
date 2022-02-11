/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionloadingwidget.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 19.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONLOADINGWIDGET_H
#define GT_COLLECTIONLOADINGWIDGET_H

#include <QWidget>

class QMovie;

/**
 * @brief The GtCollectionLoadingWidget class
 */
class GtCollectionLoadingWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    explicit GtCollectionLoadingWidget(QWidget* parent = nullptr);

    /**
     * @brief Runs loading animation.
     */
    void runAnimation();

    /**
     * @brief Stops loading animation.
     */
    void stopAnimation();

private:
    /// Loading animation.
    QMovie* m_anim;

};

#endif // GT_COLLECTIONLOADINGWIDGET_H
