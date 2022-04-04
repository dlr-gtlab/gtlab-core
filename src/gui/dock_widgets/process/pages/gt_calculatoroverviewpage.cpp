/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatoroverviewpage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QDebug>

#include "gt_processwizard.h"
#include "gt_searchwidget.h"
#include "gt_processoverviewtree.h"
#include "gt_treefiltermodel.h"
#include "gt_calculatoroverviewmodel.h"
#include "gt_abstractprocessprovider.h"
#include "gt_processcalculatoritem.h"
#include "gt_extendedcalculatordata.h"
#include "gt_calculatorsettingspage.h"
#include "gt_customprocesswizard.h"
#include "gt_processwizardpage.h"
#include "gt_palette.h"
#include "gt_application.h"


#include "gt_calculatoroverviewpage.h"

GtCalculatorOverviewPage::GtCalculatorOverviewPage(GtProcessWizard* parent) :
    QWizardPage(parent),
    m_wizard(parent),
    m_model{std::make_unique<GtCalculatorOverviewModel>()}
{
    setTitle(tr("Select Calculator"));

    QVBoxLayout* layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_view = new GtProcessOverviewTree(m_model.get(), this);
    m_view->setFrameStyle(QTreeView::NoFrame);
    m_view->setHeaderHidden(true);
    m_view->expandAll();

    layout->addWidget(m_view);

//    layout->addStretch(1);

    GtSearchWidget* searchWidget = new GtSearchWidget(this);
    layout->addWidget(searchWidget);

    setLayout(layout);

    m_view->sortAlphabetically();

    connect(m_view, SIGNAL(searchRequest()), searchWidget, SLOT(enableSearch()));
    connect(searchWidget, SIGNAL(textEdited(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SIGNAL(completeChanged()));
    connect(m_view, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));

    GtPalette::applyThemeToWidget(this);
}

GtCalculatorOverviewPage::~GtCalculatorOverviewPage() = default;

bool
GtCalculatorOverviewPage::validatePage()
{
    if (!m_wizard)
    {
        return false;
    }

    if (!m_wizard->provider())
    {
        return false;
    }

    QModelIndex index = m_view->currentIndex();

    if (!index.isValid())
    {
        return false;
    }

    index = m_view->filterModel()->mapToSource(index);

    GtAbstractProcessItem* aItem = m_view->sourceModel()->itemFromIndex(index);

    if (!aItem)
    {
        return false;
    }

    GtProcessCalculatorItem* pItem =
            qobject_cast<GtProcessCalculatorItem*>(aItem);

    if (!pItem)
    {
        return false;
    }

    if (!m_wizard->provider()->initComponent(
                pItem->calculatorData()->metaData()))
    {
        return false;
    }

    // cleanup
    QList<int> pageIds = m_wizard->pageIds();

//    qDebug() << "page ids = " << m_wizard->pageIds();

    foreach (int id, pageIds)
    {
        if (id != 0)
        {
            m_wizard->removePage(id);
        }
    }

    GtExtendedCalculatorDataImpl* eData =
            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                pItem->calculatorData().get());

    if (!eData || !eData->wizard)
    {
        m_wizard->setPage(GtProcessWizard::calculatorSettingsPage,
                          new GtCalculatorSettingsPage(m_wizard->scope(),
                                                       m_wizard));
    }
    else
    {
        qDebug() << "#### found custom wizard!";
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

        qDebug() << "found " << cPages.size() << " pages!";

        if (cPages.isEmpty())
        {
            m_wizard->setPage(GtProcessWizard::calculatorSettingsPage,
                              new GtCalculatorSettingsPage(m_wizard->scope(),
                                                           m_wizard));
        }
        else
        {
            m_wizard->setEnableExecutionSettings(true);

            foreach (GtProcessWizardPage* pwp, cPages)
            {
                m_wizard->addPage(pwp);
            }
        }
    }

    return true;
}

bool
GtCalculatorOverviewPage::isComplete() const
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

    return !index.data(GtCalculatorOverviewModel::CategoryRole).toBool();
}

void
GtCalculatorOverviewPage::onDoubleClicked(const QModelIndex& /*index*/)
{
    if (!isComplete())
    {
        return;
    }

    wizard()->next();
}
