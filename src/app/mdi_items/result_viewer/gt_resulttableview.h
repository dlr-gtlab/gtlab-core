/* GTlab - Gas Turbine laboratory
 * Source File: gt_resulttableview.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTRESULTTABLEVIEW_H
#define GTRESULTTABLEVIEW_H

#include <QTableView>

/**
 * @brief The GtResultTableView class
 */
class GtResultTableView : public QTableView
{
    Q_OBJECT

public:
    /**
     * @brief GtResultTableView
     * @param parent
     */
    explicit GtResultTableView(QWidget* parent = nullptr);

protected:
    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);

private slots:
    /**
     * @brief customMenuRequested
     * @param point
     */
    void customMenuRequested(QPoint point);

    /**
     * @brief copyToClipboard
     */
    void copyToClipboard();

};

#endif // GTRESULTTABLEVIEW_H
