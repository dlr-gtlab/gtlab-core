/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>

#include "gt_object.h"
#include "gt_propertytreeview.h"
#include "gt_application.h"
#include "gt_session.h"
#include "gt_project.h"
#include "gt_propertystructcontainer.h"
#include "gt_icons.h"
#include "gt_stylesheets.h"
#include "gt_propertymodel.h"
#include "gt_command.h"

#include "gt_propertycontainerwidget.h"
#include "gt_structproperty.h"
#include "gt_qtutilities.h"

#include <QInputDialog>


GtPropertyContainerWidget::GtPropertyContainerWidget(
        GtObject* obj, GtPropertyStructContainer& container, QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout* lay = new QVBoxLayout;

    lay->setContentsMargins(0, 0, 0, 0);

    m_containerTree = new GtPropertyTreeView(gtApp->session());
    m_containerTree->setColumnWidth(0, 100);
    m_containerTree->setColumnWidth(1, 70);
    m_containerTree->setColumnWidth(2, 50);
    m_containerTree->setFrameStyle(QTreeView::NoFrame);

    m_containerTree->setObject(obj, container);
    m_containerTree->setScope(obj->findParent<GtProject*>());

    lay->addWidget(m_containerTree);

    QPushButton* addBtn = new QPushButton(tr("Add new ") +
                                          container.entryPrefix());
    addBtn->setIcon(gt::gui::icon::add());
    addBtn->setToolTip(tr("Add New Entry"));
    addBtn->setStyleSheet(gt::gui::stylesheet::button());

    if (container.getFlags() & GtPropertyStructContainer::ReadOnly)
    {
        addBtn->setVisible(false);
    }

    lay->addWidget(addBtn);

    setLayout(lay);

    m_allowedTypes = container.allowedTypes();
    m_obj = obj;
    m_containerId = container.ident();

    connect(addBtn, &QPushButton::clicked,
            this, qOverload<>(&GtPropertyContainerWidget::addNewEntry));
    connect(gtApp, &GtApplication::themeChanged, addBtn, [=](){
        addBtn->setStyleSheet(gt::gui::stylesheet::button());
    });

    GtPropertyModel* model = m_containerTree->propertyModel();

    if (model)
    {
        connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(onRowsInserted(QModelIndex,int,int)));
    }
}

void
GtPropertyContainerWidget::addNewEntry()
{
    if (m_allowedTypes.isEmpty())
    {
        gtWarning() << tr("no types registered!");
        return;
    }

    if (!m_obj)
    {
        gtWarning() << tr("invalid object!");
        return;
    }

    auto* container = m_obj->findPropertyContainer(m_containerId);

    if (!container)
    {
        gtWarning() << tr("container not found!");
        return;
    }

    // only single type allows. add default entry
    if (m_allowedTypes.size() == 1)
    {
        addNewEntry(*container, m_allowedTypes.first());
        return;
    }

    QMenu menu(this);

    QList<QAction*> actions;

    foreach (const QString& allowedType, m_allowedTypes)
    {
        actions << menu.addAction(allowedType);
    }

    QAction* a = menu.exec(QCursor::pos());

    const int a_idx = actions.indexOf(a);

    if (a_idx >= 0)
    {
        addNewEntry(*container, m_allowedTypes.at(a_idx));
    }
}

void
GtPropertyContainerWidget::onRowsInserted(const QModelIndex& parent, int first,
                                          int last)
{
    if (!parent.isValid() && first == last)
    {
        QModelIndex fidx = m_containerTree->mapFromSource(
                    m_containerTree->propertyModel()->index(first, 0));


        m_containerTree->expandRecursively(fidx);
    }
}

QString queryNewEntryId(QWidget* parent, GtPropertyStructContainer& container)
{
    // we need to query the id from the user

    bool ok = false;

    // proposal
    QString newKey = "my_key";

    QStringList keys;
    std::transform(container.begin(), container.end(),
                   std::back_inserter(keys), [](const auto& arg){
                       return arg.ident();
                   });

    while(!ok)
    {
        newKey = QInputDialog::getText(parent, QObject::tr("Enter the key of the entry"),
                                      QObject::tr("Entry key:"), QLineEdit::Normal, newKey, &ok);

        if (!ok) return ""; // aborted

        ok = !keys.contains(newKey);

        if (!ok) newKey = gt::makeUniqueName(newKey, keys);
    }

    return newKey;
}

void
GtPropertyContainerWidget::addNewEntry(GtPropertyStructContainer& container,
                                       const QString& entryType)
{


    QString newId;

    if (container.type() == GtPropertyStructContainer::Associative)
    {
        newId = queryNewEntryId(this, container);

        // the new id must not be empty for a map style container
        if (newId.isEmpty()) return;
    }

    const QString cmdStr = m_obj->objectName() +
            QStringLiteral(" - ") +
            container.name() +
            QStringLiteral(" ") +
            QObject::tr("Entry added");

    auto cmd = gtApp->makeCommand(m_obj, cmdStr);
    Q_UNUSED(cmd)

    container.newEntry(entryType, newId);
}
