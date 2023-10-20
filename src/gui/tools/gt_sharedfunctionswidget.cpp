/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_sharedfunctionswidget.cpp
 *
 *  Created on: 25.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_sharedfunctionswidget.h"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>

#include "gt_searchwidget.h"
#include "gt_icons.h"
#include "gt_logging.h"

#include "internal/gt_sharedfunctionhandler.h"

using namespace gt;



GtSharedFunctionsWidget::GtSharedFunctionsWidget(QWidget* parent) :
    QWidget(parent)
{
    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    /// Main widget
    // plugins
    m_content = new QTreeWidget;
    m_content->setHeaderLabels(QStringList{tr("Module"), tr("Name"),
                                           tr("Description")});

    m_content->header()->setStretchLastSection(true);
    m_content->setColumnWidth(0, 150);


    QStringList functionIDs =
            detail::SharedFunctionHandler::instance().getRegisteredFunctionIDs();

    for (const QString& id : qAsConst(functionIDs))
    {
        QString moduleId;
        QString functionId;
        std::tie(moduleId, functionId) = divideKey(id);

        if (moduleId.isEmpty() || functionId.isEmpty())
        {
            continue;
        }

        QList<QTreeWidgetItem *> cats =
                m_content->findItems(moduleId, Qt::MatchFixedString);

        if (cats.isEmpty())
        {
            auto catItem = new QTreeWidgetItem(QStringList {moduleId});

            catItem->setIcon(0, gui::icon::variable());
            m_content->addTopLevelItem(catItem);

            cats.append(catItem);
        }

        auto func = interface::getSharedFunction(moduleId, functionId);

        if (!func)
        {
            continue;
        }

        auto item = new QTreeWidgetItem(QStringList {QString(),
                                                     functionId,
                                                     func.help()});
        for (int col = 0; col < item->columnCount(); ++col)
        {
            item->setTextAlignment(col, Qt::AlignTop | Qt::AlignLeft);
        }

        item->setToolTip(0, func.help());
        cats.first()->addChild(item);
        m_content->expandItem(cats.first());
    }

    m_content->resizeColumnToContents(0);
    m_content->resizeColumnToContents(1);
    m_content->resizeColumnToContents(2);

    layout->addWidget(m_content);

    m_searchWidget = new GtSearchWidget;
    layout->addWidget(m_searchWidget);

    connect(m_searchWidget, SIGNAL(textEdited(QString)), this,
            SLOT(filterData(QString)));
    connect(m_searchWidget, SIGNAL(textChanged(QString)), this,
            SLOT(filterData(QString)));

    setLayout(layout);
}

/// see explorerdock
void
GtSharedFunctionsWidget::filterData(const QString& val)
{
    if (!m_content)
    {
        return;
    }

    int numberOfCategories = m_content->topLevelItemCount();

    QList<QTreeWidgetItem *> cats;
    QList<QTreeWidgetItem *> allChildren;

    for (int i = 0; i < numberOfCategories; ++i)
    {
        cats.append(m_content->topLevelItem(i));
    }

    for (auto top : cats)
    {
        if (!top)
        {
            continue;
        }

        int numberOfChildren = top->childCount();

        for (int i = 0; i < numberOfChildren; ++i)
        {
            top->child(i)->setHidden(false);
            allChildren.append(top->child(i));
        }
    }

    for (auto c : allChildren)
    {
        if (c)
        {
            QString text = c->data(1, Qt::DisplayRole).toString();
            c->setHidden(!text.contains(val));
        }
    }
}

std::pair<QString, QString>
GtSharedFunctionsWidget::divideKey(QString const& functionId)
{
    QStringList list = functionId.split("::");

    if (list.size() != 2)
    {
        gtWarning() << "Cannot split shared function ID" << functionId;
        return {};
    }

    return {list.first(), list.at(1)};
}
