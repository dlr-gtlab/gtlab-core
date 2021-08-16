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

#include <QTableWidget>
#include <QHeaderView>
#include <QLayout>
#include <QLineEdit>

GtPreferencesShortCuts::GtPreferencesShortCuts() :
    m_tab(nullptr)
{
    setTitle(tr("Short Cuts"));

    m_tab = new QTableWidget(1, 1, this);

    GtShortCuts* cuts = gtApp->shortCuts();

    QList<GtShortCut*> list = cuts->shortCuts();

    m_tab->setRowCount(list.count());
    m_tab->setColumnCount(3);

    m_tab->setHorizontalHeaderLabels(QStringList()
                                   << "Function" << "ShortCut" << "Category");
    m_tab->verticalHeader()->hide();
    m_tab->setShowGrid(false);
    m_tab->setColumnWidth(0, 150);
    m_tab->setColumnWidth(1, 95);
    m_tab->setColumnWidth(2, 80);

    m_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tab->verticalHeader()->setDefaultSectionSize(20);

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

        QLineEdit* edit = new QLineEdit(m_tab);
        edit->setFrame(false);
        edit->setValidator(new QRegExpValidator(GtRegExp::forShortCuts(),
                                                edit));
        edit->setText(key);

        m_tab->setItem(i, 0, idItem);
        m_tab->setCellWidget(i, 1, edit);
        m_tab->setItem(i, 2, catItem);
        ++i;
    }

    layout()->addWidget(m_tab);
}

void
GtPreferencesShortCuts::saveSettings()
{
    GtShortCuts* cuts = gtApp->shortCuts();

    QMap<QString, QStringList> settingsTable;

    if (cuts == Q_NULLPTR)
    {
        return;
    }
    if (m_tab != nullptr)
    {
        for (int i = 0; i < m_tab->rowCount(); ++i)
        {
            QString id = m_tab->item(i, 0)->text();
            QWidget* w = m_tab->cellWidget(i, 1);
            QLineEdit* e = qobject_cast<QLineEdit*>(w);

            if (e == nullptr)
            {
                return;
            }

            QString key = e->text();
            QString cat = m_tab->item(i, 2)->text();

            GtShortCut* current = cuts->findShortCut(id, cat);
            current->setKey(QKeySequence(key));

            settingsTable.insert(id, QStringList{key, cat});
        }
    }

    gtApp->settings()->setShortcutsTable(settingsTable);
}

void
GtPreferencesShortCuts::loadSettings()
{

}
