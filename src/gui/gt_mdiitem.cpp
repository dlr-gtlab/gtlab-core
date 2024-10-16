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


struct GtMdiItem::Impl
{
    Impl() :
        m_subWin(nullptr),
        m_queueEvents(false)
    {}

    ~Impl()
    {
        qDeleteAll(m_eventQueue);
    }

    QWidget* m_subWin;
    QList<GtQueuedMdiEvent*> m_eventQueue;
    bool m_queueEvents;

    /// The list of registered toolbar actions, non-owning
    std::vector<GtQmlAction*> m_toolbarActions;

    template <class T>
    T* takeEvent()
    {
        for (GtQueuedMdiEvent* e: qAsConst(m_eventQueue))
        {
            if (T* ce = qobject_cast<T*>(e))
            {
                m_eventQueue.removeOne(e);
                return ce;
            }
        }

        return NULL;
    }
};

GtMdiItem::GtMdiItem() :
    m_frame(new GtMdiWidget),
    m_d(nullptr),
    pimpl(std::make_unique<Impl>())
{
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
    return m_frame.data();
}

QIcon
GtMdiItem::icon() const
{
    return QIcon();
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
    if (m_frame && !m_frame->parent() && parent() != m_frame)
    {
        delete m_frame;
    }
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

QWidget*
GtMdiItem::subWin()
{
    return pimpl->m_subWin;
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
    return pimpl->m_toolbarActions;
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
GtMdiItem::setSubWin(QWidget* subWin)
{
    pimpl->m_subWin = subWin;
}

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
    pimpl->m_toolbarActions.push_back(action);
    return action;
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
    if (pimpl->m_queueEvents)
    {
        delete pimpl->takeEvent<GtProjectChangedEvent>();

        pimpl->m_eventQueue << new GtProjectChangedEvent(this, project);
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
    if (pimpl->m_queueEvents)
    {
        delete pimpl->takeEvent<GtObjectChangedEvent>();

        pimpl->m_eventQueue << new GtObjectChangedEvent(this, obj);
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

    pimpl->m_queueEvents = true;
}

void
GtMdiItem::windowAboutToActive()
{
    foreach (GtQueuedMdiEvent* e, pimpl->m_eventQueue)
    {
        e->handle();
    }

    qDeleteAll(pimpl->m_eventQueue);
    pimpl->m_eventQueue.clear();

    pimpl->m_queueEvents = false;
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

