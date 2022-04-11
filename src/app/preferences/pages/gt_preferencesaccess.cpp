/* GTlab - Gas Turbine laboratory
 * Source File: gt_preferencesaccess.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLayout>
#include <QTabWidget>
#include <QTableView>
#include <QPushButton>
#include <QHeaderView>

#include "gt_icons.h"
#include "gt_accessgroupmodel.h"
#include "gt_accessgroup.h"
#include "gt_accessdatadialog.h"
#include "gt_abstractaccessdataconnection.h"
#include "gt_accessmanager.h"
#include "gt_deleteitemmessagebox.h"

#include "gt_preferencesaccess.h"

GtPreferencesAccess::GtPreferencesAccess()
{
    setTitle(tr("Access"));

//    QVBoxLayout* lay = new QVBoxLayout;

    m_tabWidget = new QTabWidget;

    layout()->addWidget(m_tabWidget);

    // initialize pages
    foreach (GtAccessGroup* group, gtAccessManager->accessGroups())
    {
        QWidget* tab = new QWidget;

        QTableView* view = new QTableView;
        QHeaderView* verticalHeader = view->verticalHeader();
        verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
        verticalHeader->setDefaultSectionSize(16);

        GtAccessGroupModel* model = new GtAccessGroupModel(group, this);

        m_models << model;

        view->setModel(model);

        view->setColumnWidth(0, 195);
        view->setColumnWidth(3, 20);
        view->setColumnWidth(4, 20);
        view->verticalHeader()->setVisible(false);
        view->setFrameStyle(QTableView::NoFrame);
        view->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        view->setIconSize(QSize(14, 14));

        QVBoxLayout* lay = new QVBoxLayout;

        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(0);

        lay->addWidget(view);

        QPushButton* btn = new QPushButton(GtGUI::Icon::add16(), tr("Add"));

        connect(btn, SIGNAL(clicked(bool)), SLOT(addAccessData()));
        connect(view, SIGNAL(doubleClicked(QModelIndex)),
                SLOT(onDoubleClicked(QModelIndex)));
        connect(view, SIGNAL(clicked(QModelIndex)),
                SLOT(onClicked(QModelIndex)));

        lay->addWidget(btn);

        tab->setLayout(lay);

        m_tabWidget->addTab(tab, group->objectName());
    }
}

void
GtPreferencesAccess::saveSettings()
{

}

void
GtPreferencesAccess::loadSettings()
{

}

void
GtPreferencesAccess::addAccessData()
{
    const int tab = m_tabWidget->currentIndex();

    if (tab < 0 || tab >= m_models.size())
    {
        return;
    }

    QList<GtAccessGroup*> groups = gtAccessManager->accessGroups();

    GtAccessGroup* group = groups[tab];

    if (!group)
    {
        return;
    }

    QObject* obj = group->connectionMetaData().newInstance();

    if (!obj)
    {
        return;
    }

    GtAbstractAccessDataConnection* con =
            qobject_cast<GtAbstractAccessDataConnection*>(obj);

    if (!con)
    {
        delete obj;
        return;
    }

    GtAccessData accessData(con->defaultHost(), con->defaultPort());

    delete con;

    GtAccessDataDialog dialog(accessData, group->connectionMetaData());

    if (dialog.exec())
    {
        m_models[tab]->addAccessData(accessData);

        gtAccessManager->saveAccessData();
    }
}

void
GtPreferencesAccess::onDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    const int tab = m_tabWidget->currentIndex();

    if (tab < 0 || tab >= m_models.size())
    {
        return;
    }

    GtAccessGroupModel* model = m_models[tab];

    if (index.model() != model)
    {
        return;
    }

    if (index.column() < 0 || index.column() > 2)
    {
        return;
    }

    QList<GtAccessGroup*> groups = gtAccessManager->accessGroups();

    GtAccessGroup* group = groups[tab];

    if (!group)
    {
        return;
    }

    QList<GtAccessData> accessDataList = group->accessData();

    GtAccessData accessData = accessDataList[index.row()];

    GtAccessDataDialog dialog(accessData, group->connectionMetaData());

    if (dialog.exec())
    {
        model->setAccessData(accessData, index);
        gtAccessManager->saveAccessData();
    }
}

void
GtPreferencesAccess::onClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    const int tab = m_tabWidget->currentIndex();

    if (tab < 0 || tab >= m_models.size())
    {
        return;
    }

    GtAccessGroupModel* model = m_models[tab];

    if (index.model() != model)
    {
        return;
    }

    if (index.column() == 3)
    {
        QList<GtAccessGroup*> groups = gtAccessManager->accessGroups();

        GtAccessGroup* group = groups[tab];

        if (!group)
        {
            return;
        }

        QList<GtAccessData> accessDataList = group->accessData();

        GtAccessData accessData = accessDataList[index.row()];

        GtAccessDataDialog dialog(accessData, group->connectionMetaData());

        if (dialog.exec())
        {
            model->setAccessData(accessData, index);
            gtAccessManager->saveAccessData();
        }
    }
    else if (index.column() == 4)
    {
        QList<GtAccessGroup*> groups = gtAccessManager->accessGroups();

        GtAccessGroup* group = groups[tab];

        if (!group)
        {
            return;
        }

        QList<GtAccessData> accessDataList = group->accessData();

        GtAccessData accessData = accessDataList[index.row()];

        GtDeleteItemMessageBox mb(tr("Delete Access Data"), accessData.host());

        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                model->removeAccessData(index);
                gtAccessManager->saveAccessData();
                break;
            }

            case QMessageBox::Cancel:
            {
                break;
            }

            default:
                break;
        }
    }
}
