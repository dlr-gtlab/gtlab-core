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
#include <QDialog>
#include <QTreeView>

#include "gt_objectlinkproperty.h"
#include "gt_object.h"
#include "gt_application.h"
#include "gt_project.h"
#include "gt_objectselectiondialog.h"
#include "gt_objectfactory.h"

#include "gt_propertyobjectlinkeditor.h"

GtPropertyObjectLinkEditor::GtPropertyObjectLinkEditor(QWidget* parent) :
    QWidget(parent),
    m_scope(Q_NULLPTR)
{
    QHBoxLayout* lay = new QHBoxLayout;
    m_objectId = new QLineEdit;

    QPushButton* restoreButton = new QPushButton(QStringLiteral(""));
    restoreButton->setIcon(gtApp->icon(QStringLiteral("restoreIcon_16.png")));
    restoreButton->setFlat(true);
    restoreButton->setMaximumWidth(15);
    restoreButton->setAutoDefault(false);
    restoreButton->setToolTip(tr("Delete Object Link"));

    QPushButton* selectObjectButton = new QPushButton(QStringLiteral("..."));
    selectObjectButton->setMaximumWidth(30);
    selectObjectButton->setAutoDefault(false);
    selectObjectButton->setToolTip(tr("Choose Object"));

    setLayout(lay);
    lay->addWidget(m_objectId);
    lay->addWidget(restoreButton);
    lay->addWidget(selectObjectButton);

    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_objectId->setFrame(false);
    m_objectId->setReadOnly(true);

    connect(selectObjectButton, SIGNAL(clicked(bool)),
            SLOT(selectObjectLink()));
    connect(restoreButton, SIGNAL(clicked(bool)), SLOT(deleteObjectLink()));
}

void
GtPropertyObjectLinkEditor::setObjectLinkProperty(GtObjectLinkProperty* prop)
{
    m_prop = prop;

    if (m_prop == Q_NULLPTR)
    {
        qDebug() << "m_prop == Q_NULLPTR";
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
    if (m_prop == Q_NULLPTR)
    {
        return false;
    }

    if (m_scope == Q_NULLPTR)
    {
        return false;
    }

    if (m_scope->getObjectByUuid(m_prop->linkedObjectUUID()) == Q_NULLPTR)
    {
        return false;
    }

    return true;
}

GtObject*
GtPropertyObjectLinkEditor::selectedObject()
{
    if (m_prop == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (m_scope == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return m_scope->getObjectByUuid(m_prop->linkedObjectUUID());
}

void
GtPropertyObjectLinkEditor::updateText()
{
    QString txt = QStringLiteral("-");

    if (m_prop != Q_NULLPTR && m_scope != Q_NULLPTR)
    {
        const QString uuid = m_prop->linkedObjectUUID();

        GtObject* linkedObject = m_scope->getObjectByUuid(uuid);

        if (linkedObject != Q_NULLPTR)
        {
            txt = linkedObject->objectName();
        }
    }

    m_objectId->setText(txt);
}

QList<GtObject*>
GtPropertyObjectLinkEditor::allowedObjects(GtObject* obj)
{
    if (obj == Q_NULLPTR)
    {
        return QList<GtObject*>();
    }

    QList<GtObject*> retval;

    QStringList allowedClasses = m_prop->allowedClasses();

    gtInfo() << "Allowed classes" << allowedClasses;

    if (allowedClasses.contains(obj->metaObject()->className()))
    {
        retval << obj;
    }
    else /// if the class is not directly allowed it might inherit from
        /// one of the allowed classes
    {
        GtObjectFactory* factory = GtObjectFactory::instance();

        foreach(QString s, allowedClasses)
        {
            gtInfo() << "Check class:" << s;
            GtObject* allowed = factory->newObject(s);

            if (allowed == nullptr)
            {
                gtError() << "Cannot read object of class" << s << "in factory";
                continue;
            }

            if (obj->metaObject()->inherits(allowed->metaObject()))
            {
                gtDebug() << "Add " << obj->objectName() << "to list";
                retval << obj;
            }
            else
            {
                gtInfo() << obj->metaObject()->className() << "does not "
                         << "inherit " << allowed->metaObject()->className();
            }

            if (allowed != nullptr)
            {
                delete allowed;
            }
        }
    }

    foreach (GtObject* child, obj->findDirectChildren<GtObject*>())
    {
        retval.append(allowedObjects(child));
    }

    return retval;
}

void
GtPropertyObjectLinkEditor::selectObjectLink()
{
    QList<GtObject*> allowedObjs = allowedObjects(m_scope);

    gtDebug() << "####  allowedObjs size = " << allowedObjs.size();

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

            if (obj != Q_NULLPTR)
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
