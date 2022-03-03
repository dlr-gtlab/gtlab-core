/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectselectiondialog.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QDebug>

#include "gt_application.h"
#include "gt_treeview.h"
#include "gt_searchwidget.h"
#include "gt_object.h"
#include "gt_objectmodel.h"
#include "gt_styledmodel.h"
#include "gt_objectfiltermodel.h"

#include "gt_objectselectiondialog.h"

GtObjectSelectionDialog::GtObjectSelectionDialog(GtObject* root,
                                                 QWidget* parent) :
    QDialog(parent),
    m_root(root)
{
    setWindowTitle(tr("Choose Object"));
    setWindowIcon(gtApp->icon(QStringLiteral("propertyIcon_16.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);


    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_treeView = new GtTreeView(this);
    m_treeView->setFrameStyle(QTreeView::NoFrame);
    lay->addWidget(m_treeView);

    GtSearchWidget* searchWidget = new GtSearchWidget(this);
    lay->addWidget(searchWidget);

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

    setFilterData(QStringList());

    if (root)
    {
        qDebug() << "#### " << "root object" << root->objectName();
    }
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
    m_filterModel->setFilterRegExp(val);
}
