/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectselectiondialog.cpp
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QPushButton>

#include "gt_icons.h"
#include "gt_treeview.h"
#include "gt_searchwidget.h"
#include "gt_object.h"
#include "gt_objectmodel.h"
#include "gt_styledmodel.h"
#include "gt_objectfiltermodel.h"

#include "gt_objectselectiondialog.h"

GtObjectSelectionDialog::GtObjectSelectionDialog(GtObject* root,
                                                 QWidget* parent) :
    GtDialog(parent),
    m_root(root)
{
    setWindowTitle(tr("Choose Object"));
    setWindowIcon(gt::gui::icon::property());

    auto lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_treeView = new GtTreeView(this);
    m_treeView->setFrameStyle(QTreeView::NoFrame);
    lay->addWidget(m_treeView);

    auto* btnLay = new QHBoxLayout;
    btnLay->setContentsMargins(0, 0, 0, 0);
    btnLay->setSpacing(0);
    lay->addLayout(btnLay);

    auto searchWidget = new GtSearchWidget(this);
    btnLay->addWidget(searchWidget);
    btnLay->addStretch();

    auto* expandBtn = new QPushButton;
    expandBtn->setFlat(true);

    btnLay->addWidget(expandBtn);

    m_srcModel = new GtObjectModel(this);
    m_srcModel->setRootObject(m_root);

    m_styledModel = new GtStyledModel(this);
    m_styledModel->setSourceModel(m_srcModel);

    m_filterModel = new GtObjectFilterModel(this);
    m_filterModel->setSourceModel(m_styledModel);

    QModelIndex srcIndex = m_srcModel->indexFromObject(m_root);
    QModelIndex styIndex = m_styledModel->mapFromSource(srcIndex);

    m_treeView->setModel(m_filterModel);
    m_treeView->setRootIndex(m_filterModel->mapFromSource(styIndex));

    m_treeView->setColumnHidden(1, true);
    m_treeView->setHeaderHidden(true);

    setLayout(lay);

    connect(m_treeView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));
    connect(searchWidget, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_treeView, SIGNAL(searchRequest()), searchWidget,
            SLOT(enableSearch()));

    /// returns whether the view is expanded by checking the children of root
    auto const isViewExpanded = [=](){
        int rowCount = m_filterModel->rowCount();

        QVector<QModelIndex> idxs;
        idxs.reserve(rowCount);

        auto root = m_treeView->rootIndex();
        for (int i = 0; i < rowCount; ++i)
        {
            auto idx = m_filterModel->index(i, 0, root);
            if (idx.isValid()) idxs.append(std::move(idx));
        }

        return std::all_of(std::cbegin(idxs), std::cend(idxs),
                           [=](auto const& idx){
            return m_treeView->isExpanded(idx);
        });
    };

    /// updates the visuals of the expand btn
    auto const updateExpandBtn = [=](bool isExpanded){
        using namespace gt::gui;
        expandBtn->setIcon(isExpanded ? icon::collapsed():icon::uncollapsed());
        expandBtn->setToolTip(isExpanded ? tr("Collapse All"):tr("Expand All"));
    };

    /// updates the visuals of the expand btn if an item was expanded
    auto const onExpandStateChanged = [=](QModelIndex const& idx){
        if (idx.parent() == m_treeView->rootIndex())
        {
            updateExpandBtn(isViewExpanded());
        }
    };

    connect(m_treeView, &QTreeView::expanded, this, onExpandStateChanged);
    connect(m_treeView, &QTreeView::collapsed, this, onExpandStateChanged);

    connect(expandBtn, &QPushButton::clicked, this, [=](){
        bool expand = !isViewExpanded();
        expand ? m_treeView->expandAll() : m_treeView->collapseAll();
        updateExpandBtn(expand);
    });

    setFilterData(QStringList());

    resize(300, 330);
}

void
GtObjectSelectionDialog::setFilterData(const QStringList& filter)
{
    m_filterModel->setFilterData(filter);
    m_treeView->expandAll();
}

GtObject*
GtObjectSelectionDialog::currentObject()
{
    QModelIndex index = m_treeView->currentIndex();
    QModelIndex styIndex = m_filterModel->mapToSource(index);
    QModelIndex srcIndex = m_styledModel->mapToSource(styIndex);

    return m_srcModel->objectFromIndex(srcIndex);
}

void
GtObjectSelectionDialog::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid() && (index.flags() & Qt::ItemIsEnabled))
    {
        accept();
    }
}

void
GtObjectSelectionDialog::filterData(const QString& val)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_filterModel->setFilterRegExp(val);
#else
    m_filterModel->setFilterRegularExpression(val);
#endif
    m_treeView->expandAll();
}
