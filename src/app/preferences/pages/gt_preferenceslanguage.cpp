/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLayout>
#include <QListWidget>
#include <QApplication>
#include <QDir>

#include "gt_preferenceslanguage.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_icons.h"

GtPreferencesLanguage::GtPreferencesLanguage():
    GtPreferencesPage(tr("Language"))
{
    setIcon(gt::gui::icon::language());
    QListWidget* list = new QListWidget;

    QString language = gtApp->settings()->language();

    // default setup

    QListWidgetItem* syslang = new QListWidgetItem(tr("System language"));
    QListWidgetItem* enlang = new QListWidgetItem("English");
    list->addItem(syslang);
    list->addItem(enlang);

    // dynamic language packages
    QDir dir(QApplication::applicationDirPath());

    if (language.isEmpty())
    {
        QFont font = syslang->font();
        font.setBold(true);
        syslang->setFont(font);
        list->setCurrentItem(syslang);
    }
    else if (language == "en")
    {
        QFont font = enlang->font();
        font.setBold(true);
        enlang->setFont(font);
        list->setCurrentItem(enlang);
    }

    if (dir.exists())
    {
        dir.setNameFilters(QStringList() << "*.qm");

        foreach (QString fileName, dir.entryList(QDir::Files))
        {
            fileName.truncate(fileName.lastIndexOf("."));
            fileName.remove("gtlab_");

            QLocale locale(QLocale(fileName).language());

            QString id = locale.nativeLanguageName() + " ("
                         + locale.name() + ")";


            if (fileName == language)
            {
                QListWidgetItem* item = new QListWidgetItem(id);
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                list->setCurrentItem(item);
            }

            list->addItem(id);
        }
    }

    auto layout = new QVBoxLayout;
    layout->addWidget(list);
    setLayout(layout);
}

void
GtPreferencesLanguage::saveSettings(GtSettings&) const
{

}

void
GtPreferencesLanguage::loadSettings(const GtSettings&)
{

}

