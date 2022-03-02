/* GTlab - Gas Turbine laboratory
 * Source File: gt_preferencesshortcuts.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_preferencesshortcuts.h"
#include "gt_application.h"
#include "gt_shortcut.h"
#include "gt_shortcuts.h"
#include "gt_regexp.h"
#include "gt_settings.h"
#include "gt_shortcutedit.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>

GtPreferencesShortCuts::GtPreferencesShortCuts() :
    m_tab(nullptr)
{
    setTitle(tr("Short Cuts"));

    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

    if (!mainLayout)
    {
        return;
    }

    m_tab = new QTableWidget(1, 1, this);

    m_tab->setColumnCount(3);
    m_tab->setHorizontalHeaderLabels(QStringList{tr("Function"),
                                                 tr("ShortCut"),
                                                 tr("Category")});
    m_tab->verticalHeader()->hide();
    m_tab->setShowGrid(false);
    m_tab->setColumnWidth(0, 150);
    m_tab->setColumnWidth(1, 100);
    m_tab->setColumnWidth(2, 100);

    m_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setDefaultSectionSize(20);
    m_tab->verticalHeader()->setSectionsClickable(false);

    QPushButton* restoreBtn = new QPushButton(tr("Restore"), this);
    restoreBtn->setToolTip(tr("Restores the default key combinations"));
    QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Fixed,
                                          QSizePolicy::Expanding);

    QVBoxLayout* blay = new QVBoxLayout;
    blay->addWidget(restoreBtn);
    blay->addItem(spacer);
    blay->addStretch(1);

    QHBoxLayout* hlay = new QHBoxLayout;
    hlay->addWidget(m_tab);
    hlay->addLayout(blay);
    mainLayout->addLayout(hlay);

    connect(restoreBtn, SIGNAL(clicked(bool)), this, SLOT(restoreDefaults()));

    // populate shortcut table
    GtShortCuts* cuts = gtApp->shortCuts();

    if (!cuts)
    {
        return;
    }

    QList<GtShortCut*> list = cuts->shortCuts();

    m_tab->setRowCount(list.count());

    int i = 0;
    for (GtShortCut* s : list)
    {
        QString id = s->id();
        QString key = s->key().toString();
        QString cat = s->category();

        QTableWidgetItem* idItem = new QTableWidgetItem(id);
        idItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        QTableWidgetItem* catItem = new QTableWidgetItem(cat);
        catItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        GtShortCutEdit* edit = new GtShortCutEdit(key, id);
        edit->setFrame(false);

        m_tab->setItem(i, 0, idItem);
        m_tab->setCellWidget(i, 1, edit);
        m_tab->setItem(i, 2, catItem);
        ++i;
    }
}

void
GtPreferencesShortCuts::saveSettings()
{
    GtShortCuts* cuts = gtApp->shortCuts();

    if (!cuts || !m_tab)
    {
        return;
    }

    QMap<QString, QStringList> settingsTable;

    for (int i = 0; i < m_tab->rowCount(); ++i)
    {
        QString id = m_tab->item(i, 0)->text();
        QWidget* w = m_tab->cellWidget(i, 1);
        GtShortCutEdit* e = qobject_cast<GtShortCutEdit*>(w);

        if (!e)
        {
            continue;
        }

        QKeySequence key = e->keySequence();
        QString cat = m_tab->item(i, 2)->text();

        GtShortCut* current = cuts->findShortCut(id, cat);
        current->setKey(QKeySequence(key));

        settingsTable.insert(id, QStringList{ key.toString(), cat });
    }

    gtApp->settings()->setShortcutsTable(settingsTable);
}

void
GtPreferencesShortCuts::loadSettings()
{

}

void
GtPreferencesShortCuts::restoreDefaults()
{
    auto settingsTable = gtApp->settings()->intialShortCutsMap();

    if (settingsTable.isEmpty() || !m_tab)
    {
        return;
    }

    for (int i = 0; i < m_tab->rowCount(); ++i)
    {
        QString id = m_tab->item(i, 0)->text();
        QWidget* w = m_tab->cellWidget(i, 1);
        GtShortCutEdit* e = qobject_cast<GtShortCutEdit*>(w);

        // list with two elements (keysequence and category)
        QStringList list = settingsTable.value(id);

        if (list.length() != 2 || !e)
        {
            continue;
        }

        e->setKeySequence(QKeySequence(list.at(0)));
    }
}
