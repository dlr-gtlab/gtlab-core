/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskoverviewpage.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QDebug>

#include "gt_processwizard.h"
#include "gt_searchwidget.h"
#include "gt_processoverviewtree.h"
#include "gt_treefiltermodel.h"
#include "gt_taskoverviewmodel.h"
#include "gt_processtaskitem.h"
#include "gt_taskfactory.h"
#include "gt_calculatorsettingspage.h"
#include "gt_abstractprocessprovider.h"
#include "gt_extendedtaskdata.h"
#include "gt_customprocesswizard.h"
#include "gt_processwizardpage.h"
#include "gt_logging.h"

#include "gt_taskoverviewpage.h"

GtTaskOverviewPage::GtTaskOverviewPage(GtProcessWizard* parent) :
    QWizardPage(parent),
    m_wizard(parent)
{
    setTitle(tr("Select Task"));

    Qt::WindowFlags flags = m_wizard->windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    m_wizard->setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    GtTaskOverviewModel* model = new GtTaskOverviewModel;

    m_view = new GtProcessOverviewTree(model, this);
    m_view->setFrameStyle(QTreeView::NoFrame);
    m_view->setHeaderHidden(true);
    m_view->expandAll();

    layout->addWidget(m_view);

    //    layout->addStretch(1);

    GtSearchWidget* searchWidget = new GtSearchWidget(this);
    layout->addWidget(searchWidget);

    setLayout(layout);

    m_view->sortAlphabetically();

    connect(m_view, SIGNAL(searchRequest()), searchWidget,
            SLOT(enableSearch()));
    connect(searchWidget, SIGNAL(textEdited(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SIGNAL(completeChanged()));
    connect(m_view, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));
}

bool
GtTaskOverviewPage::validatePage()
{
    if (!m_wizard)
    {
        gtError() << "m_wizard == nullptr";
        return false;
    }

    if (!m_wizard->provider())
    {
        gtError() << "m_wizard->provider() == nullptr";
        return false;
    }

    QModelIndex index = m_view->currentIndex();

    if (!index.isValid())
    {
        gtError() << "!index.isValid()";
        return false;
    }

    index = m_view->filterModel()->mapToSource(index);

    GtAbstractProcessItem* aItem = m_view->sourceModel()->itemFromIndex(index);

    if (!aItem)
    {
        gtError() << "aItem == nullptr";
        return false;
    }

    GtProcessTaskItem* pItem = qobject_cast<GtProcessTaskItem*>(aItem);

    if (!pItem)
    {
        gtError() << "pItem == nullptr";
        return false;
    }

    if (!m_wizard->provider()->initComponent(pItem->taskData()->metaData()))
    {
        gtError() << "!m_wizard->provider()->initComponent(pItem->taskData()"
                  "->metaData())";
        return false;
    }

    // cleanup
    QList<int> pageIds = m_wizard->pageIds();

    foreach (int id, pageIds)
    {
        if (id != 1)
        {
            wizard()->removePage(id);
        }
    }

    GtExtendedTaskDataImpl* eData =
        dynamic_cast<GtExtendedTaskDataImpl*>(
            pItem->taskData().get());

    if (!eData || !eData->wizard)
    {
        m_wizard->setPage(GtProcessWizard::calculatorSettingsPage,
                          new GtCalculatorSettingsPage(m_wizard->scope(),
                                  m_wizard));
    }
    else
    {
        // get custom process wizard pages
        QList<GtProcessWizardPage*> cPages;

        foreach (const QMetaObject& metaObj, eData->wizard->pages())
        {
            QObject* obj = metaObj.newInstance();

            if (!obj)
            {
                qDebug() << "page not invokable!";
                cPages.clear();
                break;
            }

            GtProcessWizardPage* pwp = qobject_cast<GtProcessWizardPage*>(obj);

            if (!pwp)
            {
                qDebug() << "page corrupted!";
                delete obj;
                cPages.clear();
                break;
            }

            pwp->setProvider(m_wizard->provider());
            pwp->setScope(m_wizard->scope());

            cPages << pwp;
        }

        if (cPages.isEmpty())
        {
            m_wizard->setPage(GtProcessWizard::calculatorSettingsPage,
                              new GtCalculatorSettingsPage(m_wizard->scope(),
                                      m_wizard));
        }
        else
        {
            foreach (GtProcessWizardPage* pwp, cPages)
            {
                m_wizard->addPage(pwp);
            }
        }
    }

    return true;
}

bool
GtTaskOverviewPage::isComplete() const
{
    if (!m_view)
    {
        return false;
    }

    QModelIndex index = m_view->currentIndex();

    if (!index.isValid())
    {
        return false;
    }

    return !index.data(GtTaskOverviewModel::CategoryRole).toBool();
}

void
GtTaskOverviewPage::onDoubleClicked(const QModelIndex& /*index*/)
{
    if (!isComplete())
    {
        return;
    }

    wizard()->next();
}
