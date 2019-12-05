/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QIcon>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_projectchangedevent.h"
#include "gt_objectchangedevent.h"
#include "gt_statehandler.h"

GtMdiItem::GtMdiItem() : m_frame(new QFrame), m_d(Q_NULLPTR),
    m_subWin(Q_NULLPTR), m_queueEvents(false)
{
    m_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

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
    return m_frame;
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
//    gtError() << "MDI ITEM DESTROYED!";
    if (m_frame && m_frame->parent() == NULL)
    {
        delete m_frame;
    }

    qDeleteAll(m_eventQueue);
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

QMdiSubWindow*
GtMdiItem::subWin()
{
    return m_subWin;
}

bool
GtMdiItem::allowsMultipleInstances()
{
    return false;
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
GtMdiItem::setSubWin(QMdiSubWindow* subWin)
{
    m_subWin = subWin;
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
    Q_UNUSED(project);
}

void
GtMdiItem::onProjectChanged(GtProject* project)
{
    if (m_queueEvents)
    {
        GtProjectChangedEvent* old = takeEvent<GtProjectChangedEvent>();
        if (old)
        {
            delete old;
        }

        m_eventQueue << new GtProjectChangedEvent(this, project);
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
    if (m_queueEvents)
    {
        GtObjectChangedEvent* old = takeEvent<GtObjectChangedEvent>();
        if (old)
        {
            delete old;
        }

        m_eventQueue << new GtObjectChangedEvent(this, obj);
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

    m_queueEvents = true;
}

void
GtMdiItem::windowAboutToActive()
{
    foreach (GtQueuedMdiEvent* e, m_eventQueue)
    {
        e->handle();
    }

    qDeleteAll(m_eventQueue);
    m_eventQueue.clear();

    m_queueEvents = false;
}

void
GtMdiItem::print()
{
    QMessageBox::information(0, tr("Print error"),
                             tr("MDI Item not printable!"),
                             QMessageBox::Ok);
}

