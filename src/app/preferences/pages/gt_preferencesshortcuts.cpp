/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_preferencesshortcuts.cpp
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
#include "gt_icons.h"
#include "gt_logging.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>


GtPreferencesShortCuts::GtPreferencesShortCuts() :
    GtPreferencesPage(tr("Short Cuts"))
{
    setIcon(gt::gui::icon::inputSettings());

    m_tab = new QTableWidget(1, 1, this);
    m_tab->setFrameShape(QFrame::NoFrame);
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
    hlay->setContentsMargins(0, 0, 0, 0);
    hlay->addWidget(m_tab);
    hlay->addLayout(blay);
    setLayout(hlay);

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

        auto edit = new GtShortCutEdit(key, id);
        edit->setFrame(false);
        m_tab->setCellWidget(i, 1, edit);

        if (!editable)
        {
            edit->setEnabled(false);
            edit->GtLineEdit::setToolTip(tr("Not editable"));
        }

        m_tab->setItem(i, 0, idItem);
        m_tab->setItem(i, 1, edit);
        m_tab->setItem(i, 2, catItem);
        ++i;
    }
    m_tab->setSortingEnabled(true);
    m_tab->resizeColumnsToContents();
}

void
GtPreferencesShortCuts::saveSettings(GtSettings& settings) const
{
    GtShortCuts* shortcuts = gtApp->shortCuts();

    if (!shortcuts || !m_tab)
    {
        return;
    }

    QList<GtShortCutSettingsData> settingsList;

    bool anyChange = false;
    for (int i = 0; i < m_tab->rowCount(); ++i)
    {
        QString id = m_tab->item(i, 0)->text();
        QWidget* w = m_tab->cellWidget(i, 1);
        auto* e = qobject_cast<GtShortCutEdit*>(w);
        if (!e)
        {
            continue;
        }

        QKeySequence key = e->keySequence();
        QString cat = m_tab->item(i, 2)->text();

        GtShortCut* current = shortcuts->findShortCut(id, cat);
        assert(current);

        if (current->key() != key)
        {
            anyChange = true;
        }

        current->setKey(key);
        settingsList.append({id, cat, key.toString(), current->isReadOnly()});
    }

    if (anyChange)
    {
        emit shortcuts->changed();
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
    assert(m_tab);

    QList<GtShortCutSettingsData> initialShortcuts = gtApp->initCoreShortCuts();

    /// add the default vales from the modules
    initialShortcuts.append(gtApp->moduleShortCuts());

    if (initialShortcuts.isEmpty())
    {
        return;
    }

    // helper list for identifying duplicates
    QVector<QString> handledShortCuts;
    handledShortCuts.reserve(m_tab->rowCount());

    // iterate of all widgets
    for (int i = 0; i < m_tab->rowCount(); ++i)
    {
        // get short cut data
        QString id = m_tab->item(i, 0)->text();
        QString cat = m_tab->item(i, 2)->text();
        QWidget* w = m_tab->cellWidget(i, 1);
        auto* e = qobject_cast<GtShortCutEdit*>(w);
        if (!e)
        {
            continue;
        }

        // create a "unique" id for the shortcut
        QString uid = id + cat;

        // check for duplicate
        if (handledShortCuts.contains(uid))
        {
            gtWarning().medium() << tr("Removed duplicate shortcut") << id;
            m_tab->removeRow(i--);
            continue;
        }

        // pointer to keysequence to check if key was found
        QKeySequence key{};

        // find default shortcut value
        for (GtShortCutSettingsData const& entry : qAsConst(initialShortcuts))
        {
            if (entry.id == id && entry.category == cat)
            { // cppcheck-suppress useStlAlgorithm
                key = QKeySequence::fromString(entry.shortCut);
                break;
            }
        }

        // key not found -> shortcut is a remnant
        if (key.isEmpty())
        {
            gtWarning().medium() << tr("Removed unknown shortcut") << id;
            m_tab->removeRow(i--);
            continue;
        }

        // apply shortcut
        e->setKeySequence(key);
        handledShortCuts.append(uid);
    }

    if (GtShortCuts* shortcuts = gtApp->shortCuts())
    {
        emit shortcuts->changed();
    }
}
