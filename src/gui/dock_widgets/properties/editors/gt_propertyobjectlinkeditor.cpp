/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyobjectlinkeditor.cpp
 * copyright 2009-2016 by DLR
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

#include "gt_propertyobjectlinkeditor.h"

GtPropertyObjectLinkEditor::GtPropertyObjectLinkEditor(QWidget* parent) :
    QWidget(parent),
    m_scope(nullptr)
{
    auto lay = new QHBoxLayout;
    m_objectId = new QLineEdit;

    setAcceptDrops(true);

    QPushButton* restoreButton = new QPushButton(QLatin1String{});
    restoreButton->setIcon(GtGUI::Icon::restore16());
    restoreButton->setFlat(true);
    restoreButton->setMaximumWidth(15);
    restoreButton->setAutoDefault(false);
    restoreButton->setToolTip(tr("Delete Object Link"));

    auto selectObjectButton = new QPushButton(QStringLiteral("..."));
    selectObjectButton->setMaximumWidth(30);
    selectObjectButton->setAutoDefault(false);
    selectObjectButton->setToolTip(tr("Choose Object"));

    auto findObjectButton = new QPushButton(QLatin1String{});
    findObjectButton->setIcon(GtGUI::Icon::jumpTo());
    findObjectButton->setMaximumWidth(15);
    findObjectButton->setAutoDefault(false);
    findObjectButton->setToolTip(tr("Find Object"));

    setLayout(lay);
    lay->addWidget(m_objectId);
    lay->addWidget(findObjectButton);
    lay->addSpacing(1);
    lay->addWidget(restoreButton);
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

    if (!m_prop)
    {
        qDebug() << "m_prop == nullptr";
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

    if (obj && m_prop->allowedClasses().contains(obj->metaObject()->className()))
    {
        m_prop->setVal(obj->uuid());
        updateText();
        emit objectSelected();
    }

}

void
GtPropertyObjectLinkEditor::dragEnterEvent(QDragEnterEvent* e)
{
    const QMimeData* mime = e->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mime, false,
                                                    gtObjectFactory);
    if (obj && m_prop->allowedClasses().contains(obj->metaObject()->className()))
    {
        e->accept();
        return;
    }
    e->ignore();
}

QList<GtObject*>
GtPropertyObjectLinkEditor::allowedObjects(GtObject* obj)
{
    if (!obj)
    {
        return QList<GtObject*>();
    }

    QList<GtObject*> retval;

    QStringList allowedClasses = m_prop->allowedClasses();

    bool useSuperClasses = m_prop->linkFromSuperClass();

    if (allowedClasses.contains(obj->metaObject()->className()))
    {
        retval << obj;
    }
    /// if the class is not directly allowed it might inherit from
    /// one of the allowed classes
    else if (useSuperClasses)
    {
        if (allowedSuperClassObjects(obj))
        {
            retval.append(obj);
        }
    }

    foreach (GtObject* child, obj->findDirectChildren<GtObject*>())
    {
        retval.append(allowedObjects(child));
    }

    return retval;
}

bool
GtPropertyObjectLinkEditor::allowedSuperClassObjects(GtObject* obj)
{
    QStringList allowedClasses = m_prop->allowedClasses();

    return std::any_of(std::begin(allowedClasses),
                       std::end(allowedClasses), [obj](const QString& s) {
        return (isDerivedFromClass(obj, s));
    });
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
GtPropertyObjectLinkEditor::selectObjectLink()
{
    QList<GtObject*> allowedObjs = allowedObjects(m_scope);

    //gtDebug() << "####  allowedObjs size = " << allowedObjs.size();

    if (allowedObjs.size() == 1 && m_prop->get().isEmpty())
    {
        m_prop->setVal(allowedObjs.first()->uuid());
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
                m_prop->setVal(obj->uuid());
            }
        }
    }

    updateText();

    emit objectSelected();
}

void
GtPropertyObjectLinkEditor::deleteObjectLink()
{
    m_prop->setVal(QString());

    updateText();

    emit objectSelected();
}
