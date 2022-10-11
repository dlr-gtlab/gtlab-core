/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2012 by DLR
 *
 *  Created on: 16.09.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTOOLBARHANDLER_H
#define GTTOOLBARHANDLER_H

#include <QObject>
#include <QPointer>

#include "gt_object.h"

class GtToolbarHandler: public QObject
{
    Q_OBJECT

public:
    GtToolbarHandler();

public slots:
    void buttonClicked(const QString& btnId);

    void onObjectSelected(GtObject* obj);

private:
    QPointer<GtObject> m_selectedObj;

signals:
    void newProjectButtonClicked();

    void saveProjectButtonClicked();

    void openProjectButtonClicked();

    void undoButtonClicked();

    void redoButtonClicked();

};

#endif // GTTOOLBARHANDLER_H
