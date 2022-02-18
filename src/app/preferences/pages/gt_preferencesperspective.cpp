/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QHBoxLayout>
#include <QPushButton>

#include "gt_preferencesperspective.h"

#include "gt_perspectiveslistwidget.h"
#include "gt_application.h"

GtPreferencesPerspective::GtPreferencesPerspective()
{
    setTitle(tr("Perspectives"));

    QVBoxLayout* lay = new QVBoxLayout;

    QHBoxLayout* hLayout = new QHBoxLayout;

    m_list = new GtPerspectivesListWidget(false);

    connect(m_list, SIGNAL(itemSelectionChanged()),
            SLOT(onItemSelectionChange()));

    hLayout->addWidget(m_list);

    QVBoxLayout* bLayout = new QVBoxLayout;

    m_btnNew = new QPushButton(tr("New"));

    m_btnRename = new QPushButton(tr("Rename"));
    m_btnDuplicate = new QPushButton(tr("Duplicate"));

    m_btnDelete = new QPushButton(tr("Delete"));

    m_btnSwitch = new QPushButton(tr("Switch to"));

    bLayout->addWidget(m_btnNew);
    bLayout->addWidget(m_btnRename);
    bLayout->addWidget(m_btnDuplicate);
    bLayout->addWidget(m_btnDelete);
    bLayout->addWidget(m_btnSwitch);

//    m_btnNew->setEnabled(false);
//    m_btnRename->setEnabled(false);
//    m_btnDuplicate->setEnabled(false);
//    m_btnDelete->setEnabled(false);
//    m_btnSwitch->setEnabled(false);

    bLayout->addStretch(1);

    hLayout->addLayout(bLayout);

    lay->addLayout(hLayout);

//    m_openLastCheck = new QCheckBox(tr("Open last session on startup"));

//    lay->addWidget(m_openLastCheck);

    connect(m_btnSwitch, SIGNAL(clicked(bool)), SLOT(switchPerspective()));
    connect(m_btnRename, SIGNAL(clicked(bool)), m_list,
            SLOT(renameItem()));
    connect(m_btnNew, SIGNAL(clicked(bool)), m_list, SLOT(newItem()));
    connect(m_btnDuplicate, SIGNAL(clicked(bool)), m_list,
            SLOT(duplicateItem()));
    connect(m_btnDelete, SIGNAL(clicked(bool)), m_list, SLOT(deleteItem()));

    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

    if (mainLayout)
    {
        mainLayout->addLayout(lay);
        m_list->init();
    }
    else
    {
        delete lay;
    }
}

void
GtPreferencesPerspective::saveSettings()
{

}

void
GtPreferencesPerspective::loadSettings()
{

}

void
GtPreferencesPerspective::switchPerspective()
{
    QListWidgetItem* item = m_list->currentItem();

    if (item == nullptr)
    {
        return;
    }

    gtApp->switchPerspective(item->text());
    m_list->init();
}

void
GtPreferencesPerspective::renamePerspective()
{
    QListWidgetItem* item = m_list->currentItem();

    if (item == nullptr)
    {
        return;
    }

    item->setFlags (item->flags () | Qt::ItemIsEditable);
    m_list->editItem(item);
}

void
GtPreferencesPerspective::onItemSelectionChange()
{
    QListWidgetItem* item = m_list->currentItem();

    if (item == nullptr)
    {
        return;
    }

    if (item->text() == gtApp->perspectiveId())
    {
        m_btnSwitch->setEnabled(false);
        m_btnRename->setEnabled(false);
        m_btnDelete->setEnabled(false);
    }
    else
    {
        m_btnSwitch->setEnabled(true);
        m_btnRename->setEnabled(true);
        m_btnDelete->setEnabled(true);
    }

    if (item->text() == QLatin1String("default"))
    {
        m_btnRename->setEnabled(false);
        m_btnDelete->setEnabled(false);
    }
}
