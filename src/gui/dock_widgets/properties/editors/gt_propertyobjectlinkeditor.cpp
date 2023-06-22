/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyobjectlinkeditor.cpp
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTreeView>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

#include "gt_objectlinkproperty.h"
#include "gt_object.h"
#include "gt_icons.h"
#include "gt_objectselectiondialog.h"
#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_objectfactory.h"
#include "gt_command.h"

#include "gt_propertyobjectlinkeditor.h"

GtPropertyObjectLinkEditor::GtPropertyObjectLinkEditor(QWidget* parent) :
    QWidget(parent),
    m_scope(nullptr)
{
    auto lay = new QHBoxLayout;
    m_objectId = new QLineEdit;

    setAcceptDrops(true);

    QPushButton* restoreButton = new QPushButton();
    restoreButton->setIcon(gt::gui::icon::clear());
    restoreButton->setFlat(true);
    restoreButton->setMaximumWidth(16);
    restoreButton->setAutoDefault(false);
    restoreButton->setToolTip(tr("Clear Object Link"));

    auto selectObjectButton = new QPushButton();
    selectObjectButton->setIcon(gt::gui::icon::dots());
    selectObjectButton->setFlat(true);
    selectObjectButton->setMaximumWidth(16);
    selectObjectButton->setAutoDefault(false);
    selectObjectButton->setToolTip(tr("Choose Object"));

    auto findObjectButton = new QPushButton(QLatin1String{});
    findObjectButton->setFlat(true);
    findObjectButton->setIcon(gt::gui::icon::jumpTo());
    findObjectButton->setMaximumWidth(16);
    findObjectButton->setAutoDefault(false);
    findObjectButton->setToolTip(tr("Jump to Object"));

    setLayout(lay);
    lay->addWidget(m_objectId);
    lay->addWidget(restoreButton);
    lay->addSpacing(1);
    lay->addWidget(findObjectButton);
    lay->addSpacing(1);
    lay->addWidget(selectObjectButton);


    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_objectId->setFrame(false);
    m_objectId->setReadOnly(true);

    connect(selectObjectButton, SIGNAL(clicked(bool)),
            SLOT(selectObjectLink()));
    connect(restoreButton, SIGNAL(clicked(bool)), SLOT(deleteObjectLink()));
    connect(findObjectButton, SIGNAL(clicked(bool)), SLOT(findObject()));
}

void
GtPropertyObjectLinkEditor::setObjectLinkProperty(GtObjectLinkProperty* prop)
{
    m_prop = prop;

    if (m_prop)
    {
        connect(m_prop.data(), SIGNAL(changed()), SLOT(propertyValueChanged()));
    }

    updateText();
}

void
GtPropertyObjectLinkEditor::setScope(GtObject* scope)
{
    m_scope = scope;
}

bool
GtPropertyObjectLinkEditor::validObjectSelected()
{
    if (!m_prop)
    {
        return false;
    }

    if (!m_scope)
    {
        return false;
    }

    if (!m_scope->getObjectByUuid(m_prop->linkedObjectUUID()))
    {
        return false;
    }

    return true;
}

GtObject*
GtPropertyObjectLinkEditor::selectedObject()
{
    if (!m_prop)
    {
        return nullptr;
    }

    if (!m_scope)
    {
        return nullptr;
    }

    return m_scope->getObjectByUuid(m_prop->linkedObjectUUID());
}

void
GtPropertyObjectLinkEditor::updateText()
{
    QString txt = QStringLiteral("-");

    if (m_prop && m_scope)
    {
        const QString uuid = m_prop->linkedObjectUUID();

        GtObject* linkedObject = m_scope->getObjectByUuid(uuid);

        if (linkedObject)
        {

            txt = linkedObject->objectName();
        }
    }

    m_objectId->setText(txt);
}

void
GtPropertyObjectLinkEditor::dropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mime, false,
                                                    gtObjectFactory);

    if (obj && m_prop->isAllowed(*obj))
    {
        setPropertyValue(obj->uuid());
        emit objectSelected();
    }
}

void
GtPropertyObjectLinkEditor::dragEnterEvent(QDragEnterEvent* e)
{
    const QMimeData* mime = e->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mime, false,
                                                    gtObjectFactory);

    if (obj && m_prop->isAllowed(*obj))
    {
        e->accept();
        return;
    }
    e->ignore();
}

/// helper method for accumulating all allowed objects
void
accumulateAllowedObjects(GtObjectLinkProperty const& prop,
                         QList<GtObject*>& objs,
                         GtObject& obj)
{
    if (prop.isAllowed(obj))
    {
        objs << &obj;
    }

    foreach (GtObject* child, obj.findDirectChildren<GtObject*>())
    {
        assert(child);
        accumulateAllowedObjects(prop, objs, *child);
    }
}

QList<GtObject*>
GtPropertyObjectLinkEditor::allowedObjects(GtObject* obj) const
{
    QList<GtObject*> objs;

    if (!obj) return objs;

    accumulateAllowedObjects(*m_prop, objs, *obj);

    return objs;
}

void
GtPropertyObjectLinkEditor::setPropertyValue(const QString& val)
{
    if (!m_scope || !m_prop) return;

    const QString commandMsg = m_scope->objectName() + QStringLiteral(" - ") +
            m_prop->objectName() + QStringLiteral(" ") + QObject::tr("changed");

    GtCommand cmd = gtApp->startCommand(m_scope, commandMsg);

    m_prop->setVal(val);

    gtApp->endCommand(cmd);
}

void
GtPropertyObjectLinkEditor::findObject()
{
    QString uuid = m_prop->getVal();

    if (uuid.isEmpty())
    {
        return;
    }

    emit gtApp->selectionByUuidRequested(uuid);
}

void
GtPropertyObjectLinkEditor::propertyValueChanged()
{
    updateText();
}

void
GtPropertyObjectLinkEditor::selectObjectLink()
{
    QList<GtObject*> allowedObjs = allowedObjects(m_scope);

    //gtDebug() << "####  allowedObjs size = " << allowedObjs.size();

    if (allowedObjs.size() == 1 && m_prop->get().isEmpty())
    {
        setPropertyValue(allowedObjs.first()->uuid());
    }
    else
    {
        QStringList list;
        foreach(GtObject* o, allowedObjs)
        {
            list.append(o->metaObject()->className());
        }
        list.removeDuplicates();


        GtObjectSelectionDialog dialog(m_scope);
        dialog.setFilterData(list);

        if (dialog.exec())
        {
            GtObject* obj = dialog.currentObject();

            if (obj)
            {
                setPropertyValue(obj->uuid());
            }
        }
    }

    updateText();

    emit objectSelected();
}

void
GtPropertyObjectLinkEditor::deleteObjectLink()
{
    setPropertyValue(QString());

    emit objectSelected();
}
