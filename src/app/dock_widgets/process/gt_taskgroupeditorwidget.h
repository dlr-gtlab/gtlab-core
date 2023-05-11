/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 02.02.2023
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKGROUPEDITORWIDGET_H
#define GTTASKGROUPEDITORWIDGET_H

#include <QPointer>

#include "gt_dialog.h"
#include "qstandarditemmodel.h"

class GtProcessData;
class QListView;
class QStandardItemModel;
class QStandardItem;

class GtTaskGroupEditorWidget : public GtDialog
{
    Q_OBJECT

public:
    GtTaskGroupEditorWidget(GtProcessData& processData,
                            const QString& projectPath,
                            QWidget* parWid = nullptr);

private:
    QPointer<GtProcessData> m_processData;

    QString m_projectPath;

    QListView* m_view;

    QStandardItemModel* m_model;

    void fillList();

    QStandardItem* addNewItem(const QString& id);

private slots:
    void createNewTaskGroup();

    void onItemChanged(QStandardItem* item);

};

#endif // GTTASKGROUPEDITORWIDGET_H
