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

    m_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setDefaultSectionSize(20);
    m_tab->verticalHeader()->setSectionsClickable(false);

    auto restoreBtn = new QPushButton(tr("Restore"), this);
    restoreBtn->setToolTip(tr("Restores the default key combinations"));
    auto spacer = new QSpacerItem(1, 1, QSizePolicy::Fixed,
                                  QSizePolicy::Expanding);

    auto blay = new QVBoxLayout;
    blay->addWidget(restoreBtn);
    blay->addItem(spacer);
    blay->addStretch(1);

    auto hlay = new QHBoxLayout;
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
    for (GtShortCut *s : qAsConst(list)) {
        QString id = s->id();
        QString key = s->key().toString();
        QString cat = s->category();
        bool editable = !s->isReadOnly();

        auto idItem = new QTableWidgetItem(id);
        idItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        auto catItem = new QTableWidgetItem(cat);
        catItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        if (editable)
        {
            auto edit = new GtShortCutEdit(key, id);
            edit->setFrame(false);
            m_tab->setCellWidget(i, 1, edit);
        }
        else
        {
            auto shortCutItem = new QTableWidgetItem(key);
            shortCutItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            shortCutItem->setToolTip("Not editable");
            m_tab->setItem(i, 1, shortCutItem);
        }


        m_tab->setItem(i, 0, idItem);
        m_tab->setItem(i, 2, catItem);
        ++i;
    }
    m_tab->setSortingEnabled(true);
    m_tab->resizeColumnsToContents();
}

void
GtPreferencesShortCuts::saveSettings(GtSettings& settings) const
{
    GtShortCuts* cuts = gtApp->shortCuts();

    if (!cuts || !m_tab)
    {
        return;
    }

    QList<GtShortCutSettingsData> settingsList;

    bool anyChange = false;
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

        if (current->key() != key)
        {
            anyChange = true;
        }

        current->setKey(QKeySequence(key));
        settingsList.append({id, cat, key, current->isReadOnly()});
    }

    if (anyChange)
    {
        cuts->emitChange();
    }

    settings.setShortcutsTable(settingsList);
}

void
GtPreferencesShortCuts::loadSettings(const GtSettings&)
{

}

void
GtPreferencesShortCuts::restoreDefaults()
{
    auto settingsTable = gtApp->settings()->intialShortCutsList();

    /// add the default vales from the modules
    settingsTable.append(gtApp->moduleShortCuts());

    if (settingsTable.isEmpty() || !m_tab)
    {
        return;
    }

    for (int i = 0; i < m_tab->rowCount(); ++i)
    {
        QString id = m_tab->item(i, 0)->text();
        QString cat = m_tab->item(i, 2)->text();
        QWidget* w = m_tab->cellWidget(i, 1);
        GtShortCutEdit* e = qobject_cast<GtShortCutEdit*>(w);

        // list with two elements (keysequence and category)
        QKeySequence key;


        for (GtShortCutSettingsData const& entry : settingsTable)
        {
            if (entry.id == id && entry.category == cat)
            { // cppcheck-suppress useStlAlgorithm
                key = entry.shortCut;
                break;
            }
        }

        if (!e)
        {
            continue;
        }

        e->setKeySequence(key);
    }

    if (GtShortCuts* cuts = gtApp->shortCuts())
    {
        cuts->emitChange();
    }
}
