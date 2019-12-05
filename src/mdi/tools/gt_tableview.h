/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTABLEVIEW_H
#define GTTABLEVIEW_H

#include "gt_mdi_exports.h"

#include <QTableView>

/**
 * @brief The GtTableView class
 */
class GT_MDI_EXPORT GtTableView : public QTableView
{
    Q_OBJECT

public:
    GtTableView(QWidget* parent = Q_NULLPTR);

protected:
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

signals:
    void searchRequest();

    void copyRequest();

};

#endif // GTTABLEVIEW_H
