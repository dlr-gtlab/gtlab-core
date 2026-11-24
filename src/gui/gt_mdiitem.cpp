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
#include "gt_project.h"
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

    /// widget
    QPointer<GtMdiWidget> widget{new GtMdiWidget};

    /// current data object to operate on
    QPointer<GtObject> dataObject;

    /// widget containing this item's widget
    QPointer<QWidget> containingWidget{};

    /// item identification string
    QString id;

    /// event queue
    QList<GtQueuedMdiEvent*> eventQueue;

    /// The list of registered toolbar actions, non-owning
    std::vector<GtQmlAction*> toolbarActions;

    /// flags
    GuiItemFlags flags{NoFlag};

    bool initialized{false};
    bool queueEvents{false};

    template <class T>
    T* takeEvent()
    {
        for (GtQueuedMdiEvent* e : eventQueue)
        {
            if (T* ce = qobject_cast<T*>(e))
            {
                eventQueue.removeOne(e);
                return ce;
            }
        }

        return nullptr;
    }
};

GtMdiItem::GtMdiItem() :
    pimpl(std::make_unique<Impl>())
{
    pimpl->widget->hide();

    // delete item if widget is deleted (before QWidget's destructor)
    connect(pimpl->widget, &GtMdiWidget::widgetAboutToBeDeleted, this,
            [this]() {
                pimpl->widget = nullptr;
                delete this;
            });

    assert(gtApp);
    assert(gtDataModel);
    connect(gtApp, &GtApplication::currentProjectChanged, this,
            &GtMdiItem::onProjectChanged);
    connect(gtApp, &GtApplication::objectSelected, this,
            &GtMdiItem::onObjectChanged);
    connect(gtApp, &GtApplication::themeChanged, this,
            &GtMdiItem::onThemeChanged);
    connect(gtDataModel, &GtDataModel::projectSaved, this,
            &GtMdiItem::onProjectSaved);
}

GtMdiItem::~GtMdiItem()
{
    // delete widget if item is deleted
    if (pimpl->widget)
    {
        pimpl->widget->disconnect(this);
        // so that widget's destructor does not delete us in case we are a child
        setParent(nullptr);
        delete pimpl->widget;
    }
}

void
GtMdiItem::setItemFlag(GuiItemFlag flag, bool enable) noexcept
{
    pimpl->flags.setFlag(flag, enable);
}

void
GtMdiItem::setItemFlags(GuiItemFlags flags) noexcept
{
    pimpl->flags = flags;
}

GtMdiItem::GuiItemFlags
GtMdiItem::itemFlags() const noexcept
{
    return pimpl->flags |
           (allowsMultipleInstances() ? AllowsMultipleInstances : NoFlag);
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

const QString&
GtMdiItem::currentId() const noexcept
{
    return pimpl->id;
}

bool
GtMdiItem::setCurrentObject(GtObject* object, QString customId)
{
    if (pimpl->dataObject) pimpl->dataObject->disconnect(this);

    pimpl->dataObject = object;

    if (itemFlags().testFlag(BoundToObject))
    {
        if (!object) return false;

        connect(object, &QObject::destroyed, this, &QObject::deleteLater);
    }

    // regnerate new id
    assert(metaObject());

    QString newId = metaObject()->className();
    QString suffix = std::move(customId);

    if (suffix.isEmpty())
    {
        GtObject* object = this->currentObject();

        suffix = this->customId(object);

        if (suffix.isEmpty() && object)
        {
            suffix = object->uuid();
        }
    }

    if (!suffix.isEmpty()) newId.append(u'#').append(suffix);

    if (newId != pimpl->id) pimpl->id = std::move(newId);

    return true;
}

QWidget*
GtMdiItem::widget()
{
    return pimpl->widget.data();
}

QIcon
GtMdiItem::icon() const
{
    return gt::gui::icon::layers();
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
    return pimpl->flags.testFlag(AllowsMultipleInstances);
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
GtMdiItem::setData(GtObject* /*obj*/)
{
    // nothing to do here
}

void
GtMdiItem::initialized()
{
    // do nothing
}

void
GtMdiItem::projectChangedEvent(GtProject* /*project*/)
{
    // nothing to do here
}

void
GtMdiItem::objectChangedEvent(GtObject* /*obj*/)
{
    // nothing to do here
}

void
GtMdiItem::showEvent()
{
    // nothing to do here
}

QString
GtMdiItem::customId(GtObject* /*obj*/)
{
    return QString();
}

void
GtMdiItem::setSubWin(QWidget* w)
{
    Q_UNUSED(w);
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
GtMdiItem::setContainingWidget(QWidget* cwidget)
{
    pimpl->containingWidget = cwidget;
}

QWidget*
GtMdiItem::subWin()
{
    return widget();
}

QWidget*
GtMdiItem::containingWidget()
{
    return pimpl->containingWidget ? pimpl->containingWidget.data() : widget();
}

void
GtMdiItem::projectSavedEvent(GtProject* project)
{
    Q_UNUSED(project)
}

void
GtMdiItem::onThemeChanged()
{
    // nothing to do here
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

void
GtMdiItem::windowStateChanged(Qt::WindowStates /*oldState*/,
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
