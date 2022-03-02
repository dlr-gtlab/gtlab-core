/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectioneditor.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 02.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMovie>

#include "gt_application.h"
#include "gt_abstractcollectionsettings.h"
#include "gt_collectioninterface.h"
#include "gt_collectionloader.h"
#include "gt_collectionitem.h"
#include "gt_collectionreply.h"
#include "gt_accessmanager.h"
#include "gt_accessgroup.h"
#include "gt_accessdata.h"
#include "gt_abstractbrowserwidget.h"
#include "gt_collectionloadingwidget.h"
#include "gt_logging.h"

#include "gt_collectioneditor.h"

GtCollectionEditor::GtCollectionEditor() :
    m_specificCollectionWidget(nullptr),
    m_specificBrowserWidget(nullptr),
    m_collection(nullptr),
    m_collectionSettings(nullptr),
    m_srcCombo(nullptr),
    m_downloadButton(nullptr),
    m_updateAllButton(nullptr),
    m_selectAllCheckBox(nullptr),
    m_loader(nullptr),
    m_fetchMovie(nullptr)
{
    setObjectName(tr("Collection"));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    QFrame* frame = new QFrame(widget());
    frame->setObjectName(QStringLiteral("frame"));
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);

    if (gtApp->inDarkMode())
    {
        frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background_dark.png)}");
    }
    else
    {
        frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background.png)}");

    }

    QVBoxLayout* frameLay = new QVBoxLayout;
    frameLay->setContentsMargins(10, 10, 10, 10);

    m_tabWidget = new QTabWidget;

    m_collectionWidget = new QWidget;

    m_tabWidget->addTab(m_collectionWidget,
                      gtApp->icon(QStringLiteral("dropDownIcon.png")),
                      tr("Collection"));

    m_browserWidget = new QWidget;

    m_tabWidget->addTab(m_browserWidget,
                      gtApp->icon(QStringLiteral("zoomIcon_16.png")),
                      tr("Browse"));

    frameLay->addWidget(m_tabWidget);

    frame->setLayout(frameLay);

    lay->addWidget(frame);

    widget()->setLayout(lay);

    connect(m_tabWidget, SIGNAL(currentChanged(int)), SLOT(onTabChange(int)));
}

void
GtCollectionEditor::generateCollectionSpecificWidgets()
{
    if (!m_collectionSettings)
    {
        return;
    }

    QVBoxLayout* colLay = new QVBoxLayout;

    m_specificCollectionWidget = m_collectionSettings->localCollectionWidget(
                                     m_collectionWidget);

    colLay->addWidget(m_specificCollectionWidget);

    m_collectionWidget->setLayout(colLay);

    QVBoxLayout* browseLay = new QVBoxLayout;

    QFormLayout* formLay = new QFormLayout;

    m_srcCombo = new QComboBox;
    m_srcCombo->addItem(tr("All"));

    // fill source combo box
    fillSourceList();

    formLay->addRow(tr("Source:"), m_srcCombo);

//    QLineEdit* filterLine = new QLineEdit;

//    formLay->addRow(tr("Filter:"), filterLine);

    m_specificBrowserWidget = m_collectionSettings->browserWidget(
                                  m_browserWidget);

    browseLay->addLayout(formLay);

    m_fetchMovie = new GtCollectionLoadingWidget;
    browseLay->addWidget(m_fetchMovie);

    browseLay->addWidget(m_specificBrowserWidget);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setContentsMargins(0, 0, 0, 0);

    m_selectAllCheckBox = new QCheckBox(tr("Select All"));
    m_selectAllCheckBox->setEnabled(false);

    hLay->addWidget(m_selectAllCheckBox);

    QSpacerItem* spacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum);

    hLay->addSpacerItem(spacer);

    m_updateAllButton =
            new QPushButton(gtApp->icon(QStringLiteral("updateIcon_16.png")),
                            tr("Update All"));
    m_updateAllButton->setVisible(false);
    hLay->addWidget(m_updateAllButton);

    m_downloadButton =
            new QPushButton(gtApp->icon(QStringLiteral("dropDownIcon.png")),
                            tr("Download Selected"));

    m_downloadButton->setEnabled(false);

    hLay->addWidget(m_downloadButton);

    browseLay->addLayout(hLay);

    m_browserWidget->setLayout(browseLay);

    onBrowserSelectionChange();

    connect(m_srcCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(reloadNetworkCollection()));
    connect(m_specificBrowserWidget, SIGNAL(selectionChanged()),
            SLOT(onBrowserSelectionChange()));
    connect(m_downloadButton, SIGNAL(clicked(bool)),
            SLOT(downloadSelectedItems()));
    connect(m_selectAllCheckBox, SIGNAL(toggled(bool)),
            SLOT(onSelectAllCheckToggled(bool)));
    connect(m_updateAllButton, SIGNAL(clicked(bool)), SLOT(updateAllItems()));
}

void
GtCollectionEditor::loadLocalCollection()
{
    if (!m_collection)
    {
        return;
    }

    GtCollectionLoader loader(m_collection->collectionId());
    QList<GtCollectionItem> items = loader.loadLocalCollection();

    m_collectionSettings->setLocalCollectionData(m_specificCollectionWidget,
                                                 items);
}

void
GtCollectionEditor::fillSourceList()
{
    if (!m_srcCombo)
    {
        return;
    }

    GtAccessGroup* accessGroup =
            gtAccessManager->accessGroup(m_collection->collectionId());

    if (!accessGroup)
    {
        return;
    }

    foreach (GtAccessData accessData, accessGroup->accessData())
    {
        m_srcCombo->addItem(accessData.host());
    }
}

void
GtCollectionEditor::setBrowserCollection(
        const QList<GtCollectionNetworkItem>& installedItems,
        const QList<GtCollectionNetworkItem>& availableItems,
        const QList<GtCollectionNetworkItem>& updataAvailableItems)
{
    if (!m_collection)
    {
        return;
    }

    m_collectionSettings->setBrowserWidgetData(m_specificBrowserWidget,
                                               installedItems,
                                               availableItems,
                                               updataAvailableItems);
}

//void
//GtCollectionEditor::loadNextAccessData()
//{
//    if (m_accessQueue.isEmpty())
//    {
//        return;
//    }

//    GtAccessData accessData = m_accessQueue.first();

//    GtCollectionReply* reply = m_loader->loadNetworkCollection(accessData);

//    m_replies << reply;

//    connect(reply, SIGNAL(finished()), SLOT(onCollectionReply()));
//}

void
GtCollectionEditor::onTabChange(int tab)
{
    if (tab == 1)
    {
        reloadNetworkCollection();
    }
}

void
GtCollectionEditor::reloadNetworkCollection()
{
    if (!m_collection)
    {
        return;
    }

//    if (!m_replies.isEmpty())
//    {
//        return;
//    }

    if (!m_srcCombo)
    {
        return;
    }

    if (m_srcCombo->currentIndex() < 0)
    {
        return;
    }

    if (m_fetchMovie)
    {
        m_fetchMovie->setVisible(true);
        m_fetchMovie->runAnimation();
    }

    if (m_specificBrowserWidget)
    {
        m_specificBrowserWidget->setVisible(false);
    }

    if (m_selectAllCheckBox)
    {
        m_selectAllCheckBox->setEnabled(false);
    }

    gtDebug() << "reloading network collection...";

    if (!m_loader)
    {
        m_loader = new GtCollectionLoader(m_collection->collectionId(),
                                          this);
    }

//    m_accessQueue.clear();

    QList<GtAccessData> accessDataList;

    GtAccessGroup* accessGroup =
            gtAccessManager->accessGroup(m_collection->collectionId());

    if (accessGroup)
    {
        const int index = m_srcCombo->currentIndex();

        if (index == 0)
        {
            accessDataList = accessGroup->accessData();
        }
        else
        {

            if (index <= accessGroup->numberOfAccessData())
            {
                accessDataList << accessGroup->accessData()[index - 1];
            }
        }
    }

    GtCollectionReply* reply = m_loader->loadNetworkCollection(accessDataList);

    connect(reply, SIGNAL(finished()), SLOT(onCollectionReply()));
    connect(this, SIGNAL(destroyed(QObject*)), reply, SLOT(deleteLater()));
}

void
GtCollectionEditor::setCollection(GtCollectionInterface* collection)
{
    if (!collection)
    {
        return;
    }

    m_collection = collection;

    setObjectName(collection->collectionId());

    generateCollectionSettings();

    generateCollectionSpecificWidgets();

    loadLocalCollection();
}

void
GtCollectionEditor::generateCollectionSettings()
{
    if (!m_collection)
    {
        gtError() << tr("Could not generate settings!") <<
                     tr("Collection not found!");

        return;
    }

    if (!m_collection)
    {
        gtError() << tr("Could not generate settings!") <<
                     tr("Invalid collection!");

        return;
    }

    QObject* obj = m_collection->collectionSettings().newInstance();

    if (!obj)
    {
        gtError() << tr("Could not generate settings!") <<
                     tr("Invalid settings meta data!");

        return;
    }

    m_collectionSettings = qobject_cast<GtAbstractCollectionSettings*>(obj);

    if (!m_collectionSettings)
    {
        delete obj;

        gtError() << tr("Could not generate settings!") <<
                     tr("Invalid settings meta data!");

        return;
    }

    m_collectionSettings->setParent(this);

    connect(gtApp, SIGNAL(themeChanged(bool)),
            m_collectionSettings, SIGNAL(themeChanged()));
}

void
GtCollectionEditor::onCollectionReply()
{
    GtCollectionReply* reply = qobject_cast<GtCollectionReply*>(sender());

    if (!reply)
    {
        return;
    }

    QList<GtCollectionNetworkItem> installedItems = reply->installedItems();
    QList<GtCollectionNetworkItem> availableItems = reply->avilableItems();
    QList<GtCollectionNetworkItem> updateAvailableItems =
            reply->updateAvailableItems();

    gtDebug() << "reply successfull!";
    gtDebug() << "  |-> installed = " << installedItems.size();
    gtDebug() << "  |-> available = " << availableItems.size();
    gtDebug() << "  |-> updateAvailableItems = " << updateAvailableItems.size();

    setBrowserCollection(installedItems, availableItems,
                         updateAvailableItems);

    reply->deleteLater();

    if (m_fetchMovie)
    {
        m_fetchMovie->setVisible(false);
        m_fetchMovie->stopAnimation();
    }

    if (m_specificBrowserWidget)
    {
        m_specificBrowserWidget->setVisible(true);
    }

    if (m_selectAllCheckBox)
    {
        m_selectAllCheckBox->setEnabled(true);
    }

    if (updateAvailableItems.isEmpty())
    {
        m_updateAllButton->setVisible(false);
    }
    else
    {
        m_updateAllButton->setVisible(true);
    }
}

void
GtCollectionEditor::onBrowserSelectionChange()
{
    if (!m_specificBrowserWidget)
    {
        return;
    }

    if (m_specificBrowserWidget->itemsToDownload().isEmpty())
    {
        m_downloadButton->setEnabled(false);
    }
    else
    {
        m_downloadButton->setEnabled(true);
    }
}

void
GtCollectionEditor::onSelectAllCheckToggled(bool checked)
{
    if (!m_specificBrowserWidget)
    {
        return;
    }

    if (checked)
    {
        m_specificBrowserWidget->selectAllItems();
    }
    else
    {
        m_specificBrowserWidget->unselectAllItems();
    }

    onBrowserSelectionChange();
}

void
GtCollectionEditor::downloadSelectedItems()
{
    if (!m_specificBrowserWidget)
    {
        return;
    }

    if (!m_loader)
    {
        return;
    }

    QList<GtCollectionNetworkItem> items =
            m_specificBrowserWidget->itemsToDownload();

    if (items.isEmpty())
    {
        return;
    }

    gtDebug() << "downloading " << items.size() << " items...";

    m_loader->downloadCollectionItems(items);

//    reloadNetworkCollection();
    loadLocalCollection();

    m_tabWidget->setCurrentIndex(0);
}

void
GtCollectionEditor::updateAllItems()
{
    if (!m_specificBrowserWidget)
    {
        return;
    }

    if (!m_loader)
    {
        return;
    }

    QList<GtCollectionNetworkItem> items =
            m_specificBrowserWidget->itemsToUpdate();

    if (items.isEmpty())
    {
        return;
    }

    gtDebug() << "updating " << items.size() << " items...";

    m_loader->downloadCollectionItems(items);

    reloadNetworkCollection();
    loadLocalCollection();

    m_tabWidget->setCurrentIndex(0);
}
