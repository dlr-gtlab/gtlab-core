/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QMessageBox>

#include "gt_mdiwidget.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_projectchangedevent.h"
#include "gt_objectchangedevent.h"
#include "gt_statehandler.h"
#include "gt_qmlaction.h"
#include "gt_icons.h"


struct GtMdiItem::Impl
{
    ~Impl()
    {
        qDeleteAll(eventQueue);
    }

    ///
    QPointer<QFrame> frame{new GtMdiWidget};

    /// Mdi item data
    QPointer<GtObject> dataObject;


    /// Mdi item custom identification string
    QString cid;

    QList<GtQueuedMdiEvent*> eventQueue;

    /// The list of registered toolbar actions, non-owning
    std::vector<GtQmlAction*> toolbarActions;

    Flags flags{NoFlag};

    bool initialized{false};
    bool queueEvents{false};

    template <class T>
    T* takeEvent()
    {
        for (GtQueuedMdiEvent* e : qAsConst(eventQueue))
        {
            if (T* ce = qobject_cast<T*>(e))
            {
                eventQueue.removeOne(e);
                return ce;
            }
        }

        return NULL;
    }
};

GtMdiItem::GtMdiItem() :
    pimpl(std::make_unique<Impl>())
{
    pimpl->frame->hide();

    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            SLOT(onProjectChanged(GtProject*)));
    connect(gtApp, SIGNAL(objectSelected(GtObject*)),
            SLOT(onObjectChanged(GtObject*)));
    connect(gtDataModel, SIGNAL(projectSaved(GtProject*)),
            SLOT(onProjectSaved(GtProject*)));
}

QWidget*
GtMdiItem::widget()
{
    return pimpl->frame.data();
}

GtObject*
GtMdiItem::currentObject() noexcept
{
    return pimpl->dataObject;
}

const GtObject*
GtMdiItem::currentObject() const noexcept
{
    return pimpl->dataObject;
}

QIcon
GtMdiItem::icon() const
{
    return gt::gui::icon::layers();
}

void
GtMdiItem::setData(GtObject* /*obj*/)
{

}

GtMdiItem::~GtMdiItem()
{
    // if this object is a child of frame, the QPointer of frame was not set to
    // null before this function is called, resulting in a double deletion of
    // frame.
    if (pimpl->frame && !pimpl->frame->parent() && parent() != pimpl->frame)
    {
        delete pimpl->frame;
    }
}

bool
GtMdiItem::setCurrentObject(GtObject* object, QString customId)
{
    pimpl->dataObject = object;
    if (!customId.isEmpty())
    {
        pimpl->cid = std::move(customId);
    }
    else
    {
        regenerateId();
    }

    if (!pimpl->initialized)
    {
        pimpl->initialized = true;
        initialized();
    }
    return true;
}

bool
GtMdiItem::regenerateId()
{
    assert(metaObject());
    QString newId = metaObject()->className();
    QString suffix = this->customId(currentObject());

    const GtObject* object = qAsConst(*this).currentObject();

    if (suffix.isEmpty() && object)
    {
        suffix = object->uuid();
    }

    if (!suffix.isEmpty()) newId.append(u'#').append(suffix);

    if (newId == pimpl->cid) return false;

    pimpl->cid = newId;
    return true;
}

const QString&
GtMdiItem::currentId() const noexcept
{
    return pimpl->cid;
}

void
GtMdiItem::setFlag(Flag flag, bool enable) noexcept
{
    pimpl->flags.setFlag(flag, enable);
}

void
GtMdiItem::setFlags(Flags flags) noexcept
{
    pimpl->flags = flags;
}

GtMdiItem::Flags
GtMdiItem::flags() const noexcept
{
    return pimpl->flags |
           (allowsMultipleInstances() ? AllowsMultipleInstances : NoFlag);
}

GtState*
GtMdiItem::initializeState(const QString& id, const QString& path,
                           const QVariant& initValue, bool forceInitialization)
{
    return gtStateHandler->initializeState(metaObject()->className(),
                                           id, path, initValue, this,
                                           forceInitialization);
}

GtState*
GtMdiItem::initializeState(GtProject* project, const QString& id,
                           const QString& path, const QVariant& initValue,
                           bool forceInitialization)
{
    return gtStateHandler->initializeState(project, metaObject()->className(),
                                           id, path, initValue, this,
                                           forceInitialization);
}

bool
GtMdiItem::allowsMultipleInstances() const
{
    return false;
}

bool
GtMdiItem::isPrintable() const
{
    return false;
}

const std::vector<GtQmlAction *>&
GtMdiItem::toolbarActions() const
{
    return pimpl->toolbarActions;
}

void
GtMdiItem::initialized()
{
    // do nothing
}

QString
GtMdiItem::customId(GtObject* /*obj*/)
{
    return QString();
}

void
GtMdiItem::setSubWin(QWidget*)
{ }

QKeySequence
GtMdiItem::registerShortCut(const QString& id,
                            const QKeySequence& k,
                            bool readOnly)
{
    const QMetaObject* m = metaObject();
    gtApp->addShortCut(id, m->className(), k.toString(), readOnly);
    return getShortCut(id);
}

QKeySequence
GtMdiItem::registerShortCut(const GtShortCutSettingsData& data)
{
    gtApp->extendShortCuts(data);
    return getShortCut(data.id);
}

void
GtMdiItem::registerShortCuts(const QList<GtShortCutSettingsData>& list)
{
    gtApp->extendShortCuts(list);
}

GtQmlAction*
GtMdiItem::addToolbarAction(const QString& text, const QUrl& url)
{
    auto action = new GtQmlAction(text, url, this);
    pimpl->toolbarActions.push_back(action);
    return action;
}

void
GtMdiItem::addToolbarSeparator()
{
    auto action = GtQmlAction::makeSeparator(this);
    pimpl->toolbarActions.push_back(action);
}

QKeySequence
GtMdiItem::getShortCut(const QString& id)
{
    const QMetaObject* m = metaObject();
    return gtApp->getShortCutSequence(id, m->className());
}

void
GtMdiItem::projectChangedEvent(GtProject* /*project*/)
{

}

void
GtMdiItem::objectChangedEvent(GtObject* /*obj*/)
{

}

void
GtMdiItem::showEvent()
{
    // nothing to do here
}

void
GtMdiItem::projectSavedEvent(GtProject* project)
{
    Q_UNUSED(project)
}

void
GtMdiItem::onProjectChanged(GtProject* project)
{
    if (pimpl->queueEvents)
    {
        delete pimpl->takeEvent<GtProjectChangedEvent>();

        pimpl->eventQueue << new GtProjectChangedEvent(this, project);
    }
    else
    {
        projectChangedEvent(project);
    }
}

void
GtMdiItem::onProjectSaved(GtProject* project)
{
    projectSavedEvent(project);
}

void
GtMdiItem::onObjectChanged(GtObject* obj)
{
    if (pimpl->queueEvents)
    {
        delete pimpl->takeEvent<GtObjectChangedEvent>();

        pimpl->eventQueue << new GtObjectChangedEvent(this, obj);
    }
    else
    {
        objectChangedEvent(obj);
    }
}

void GtMdiItem::windowStateChanged(Qt::WindowStates /*oldState*/,
                                   Qt::WindowStates newState)
{
    if (newState != Qt::WindowNoState)
    {
        return;
    }

    pimpl->queueEvents = true;
}

void
GtMdiItem::windowAboutToActive()
{
    foreach (GtQueuedMdiEvent* e, pimpl->eventQueue)
    {
        e->handle();
    }

    qDeleteAll(pimpl->eventQueue);
    pimpl->eventQueue.clear();

    pimpl->queueEvents = false;
}

void
GtMdiItem::print()
{
    QMessageBox::information(nullptr, tr("Print error"),
                             tr("MDI Item not printable!"),
                             QMessageBox::Ok);
}

void
GtMdiItem::onThemeChanged()
{

}

