/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDebug>

#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_abstractcollectionsettings.h"

// editor plugins
#include "gt_mementoviewer.h"
#include "gt_processeditor.h"
#include "gt_templateviewer.h"
#include "gt_labelusageviewer.h"
#include "gt_sessionviewer.h"
#include "gt_stateviewer.h"
#include "gt_controlscheduleeditor.h"
#include "gt_resultviewer.h"
#include "gt_processmonitoring.h"
#include "gt_collectioneditor.h"
#include "gt_examplesmdiwidget.h"
#include "gt_splineeditor.h"

// dock widgets
#include "gt_outputdock.h"
#include "gt_explorerdock.h"
#include "gt_processdock.h"
#include "gt_propertiesdock.h"
#include "gt_labelsdock.h"
#include "gt_postdock.h"
#include "gt_calculatorsdock.h"

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    m_area(NULL)
{
    // static mdi items
    m_knownClasses.insert(GT_CLASSNAME(GtControlScheduleEditor),
                          GT_METADATA(GtControlScheduleEditor));

    m_knownClasses.insert(GT_CLASSNAME(GtMementoViewer),
                          GT_METADATA(GtMementoViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtProcessEditor),
                          GT_METADATA(GtProcessEditor));

    m_knownClasses.insert(GT_CLASSNAME(GtTemplateViewer),
                          GT_METADATA(GtTemplateViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtLabelUsageViewer),
                          GT_METADATA(GtLabelUsageViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtSessionViewer),
                          GT_METADATA(GtSessionViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtStateViewer),
                          GT_METADATA(GtStateViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtResultViewer),
                          GT_METADATA(GtResultViewer));

    m_knownClasses.insert(GT_CLASSNAME(GtProcessMonitoring),
                          GT_METADATA(GtProcessMonitoring));

    m_knownClasses.insert(GT_CLASSNAME(GtCollectionEditor),
                          GT_METADATA(GtCollectionEditor));

    m_knownClasses.insert(GT_CLASSNAME(GtExamplesMdiWidget),
                          GT_METADATA(GtExamplesMdiWidget));

    m_knownClasses.insert(GT_CLASSNAME(GtSplineEditor),
                          GT_METADATA(GtSplineEditor));

    // static dock widgets
    m_dockWidgets.insert(GT_CLASSNAME(GtOutputDock),
                         GT_METADATA(GtOutputDock));

    m_dockWidgets.insert(GT_CLASSNAME(GtExplorerDock),
                         GT_METADATA(GtExplorerDock));

    m_dockWidgets.insert(GT_CLASSNAME(GtProcessDock),
                         GT_METADATA(GtProcessDock));

    m_dockWidgets.insert(GT_CLASSNAME(GtPropertiesDock),
                         GT_METADATA(GtPropertiesDock));

    m_dockWidgets.insert(GT_CLASSNAME(GtLabelsDock),
                         GT_METADATA(GtLabelsDock));

    m_dockWidgets.insert(GT_CLASSNAME(GtPostDock),
                         GT_METADATA(GtPostDock));

//    m_dockWidgets.insert(GtCalculatorsDock::staticMetaObject.className(),
    //                         GtCalculatorsDock::staticMetaObject);
}

QString
GtMdiLauncher::generateMdiItemId(GtMdiItem* mdiItem)
{
    if (mdiItem == Q_NULLPTR)
    {
        return QString();
    }

    QString retval = mdiItem->metaObject()->className();

    if (!mdiItem->m_cid.isEmpty())
    {
        retval = retval + QStringLiteral("#") + mdiItem->m_cid;
    }
    else
    {
        QString customMdiId = mdiItem->customId(mdiItem->m_d);

        if (!customMdiId.isEmpty())
        {
            retval = retval + QStringLiteral("#") + customMdiId;
        }
        else
        {
            if (mdiItem->m_d != Q_NULLPTR)
            {
                retval = retval + QStringLiteral("#") + mdiItem->m_d->uuid();
            }
        }
    }

    return retval;
}

bool
GtMdiLauncher::mdiItemAllowed(GtMdiItem* mdiItem)
{
    if (mdiItem == Q_NULLPTR)
    {
        return false;
    }

    if (mdiItem->allowsMultipleInstances())
    {
        return true;
    }

    QString mdiId = generateMdiItemId(mdiItem);

    if (mdiId.isEmpty())
    {
        return false;
    }

    if (openItemIds().contains(mdiId))
    {
        return false;
    }

    return true;
}

QStringList
GtMdiLauncher::openItemIds()
{
    QStringList retval;

    QList<GtMdiItem*> openItems = m_openItems.values();

    foreach (GtMdiItem* openItem, openItems)
    {
        retval << generateMdiItemId(openItem);
    }

    return retval;
}

void
GtMdiLauncher::setFocus(const QString& mdiId)
{
    if (mdiId.isEmpty())
    {
        return;
    }

    if (m_area == Q_NULLPTR)
    {
        return;
    }

    QList<QMdiSubWindow*> list = m_area->subWindowList();

    for (auto e : m_openItems.keys())
    {
        if (generateMdiItemId(m_openItems.value(e)) == mdiId)
        {
            foreach (QMdiSubWindow* listItem, list)
            {
                if (listItem == e)
                {
                    listItem->setFocus();
                }
            }
        }
    }
}

void
GtMdiLauncher::onSubWindowClose(QObject* obj)
{
    if (m_openItems.contains(obj))
    {
        m_openItems.remove(obj);
    }
}

GtMdiLauncher*
GtMdiLauncher::instance()
{
    static GtMdiLauncher* retval = 0;
    if (retval == 0)
    {
        retval = new GtMdiLauncher(qApp);
    }
    return retval;
}

void
GtMdiLauncher::setMdiArea(QMdiArea* area)
{
    if (m_area)
    {
        return;
    }

    m_area = area;
}

QStringList
GtMdiLauncher::dockWidgetIds()
{
    return m_dockWidgets.keys();
}

QMetaObject
GtMdiLauncher::dockWidget(const QString& id)
{
    return m_dockWidgets.value(id);
}

QStringList
GtMdiLauncher::collectionIds()
{
    return m_collections.keys();
}

QIcon
GtMdiLauncher::collectionIcon(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return QIcon();
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (coll == Q_NULLPTR)
    {
        return QIcon();
    }

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return QMap<QString, QMetaType::Type>();
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (coll == Q_NULLPTR)
    {
        return QMap<QString, QMetaType::Type>();
    }

    return coll->collectionStructure();
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return Q_NULLPTR;
    }

    return m_collections.value(id);
}

void
GtMdiLauncher::print(QMdiSubWindow* subWindow)
{
    if (subWindow == Q_NULLPTR)
    {
        return;
    }

    if (m_openItems.contains(subWindow))
    {
        m_openItems.value(subWindow)->print();
    }
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    foreach (const QMetaObject& mobj, metaData)
    {
        QString classname = mobj.className();
        if (m_dockWidgets.contains(classname))
        {
            qWarning() << "dock widget class name already exists!";
            return true;
        }
    }
    return false;
}

bool
GtMdiLauncher::registerDockWidgets(const QList<QMetaObject>& metaData)
{
    if (dockWidgetsExist(metaData))
    {
        return false;
    }

    foreach (const QMetaObject& metaObj, metaData)
    {
        m_dockWidgets.insert(metaObj.className(), metaObj);
    }

    return true;
}

bool
GtMdiLauncher::registerCollection(const QString& str,
                                  GtCollectionInterface* coll)
{
    if (m_collections.contains(str))
    {
        return false;
    }

    m_collections.insert(str, coll);

    return true;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, QString customId)
{
    if (m_area == Q_NULLPTR)
    {
        return NULL;
    }

    if (!knownClass(id))
    {
        return Q_NULLPTR;
    }

    GtObject* obj = newObject(id);

    if (!obj)
    {
        return Q_NULLPTR;
    }

    GtMdiItem* mdiItem = qobject_cast<GtMdiItem*>(obj);

    if (!mdiItem)
    {
        delete obj;
        return Q_NULLPTR;
    }

    if (data != Q_NULLPTR)
    {
        mdiItem->m_d = data;
    }

    if (!customId.isEmpty())
    {
        mdiItem->m_cid = customId;
    }

    qDebug()  << "mdiItem id = " << generateMdiItemId(mdiItem);

    if (!mdiItemAllowed(mdiItem))
    {
        if ((data != nullptr) && (mdiItem->objectName() == "Result Viewer"))
        {
            QList<GtMdiItem*> openItems = m_openItems.values();

            foreach (GtMdiItem* openItem, openItems)
            {
                if (openItem->objectName() == mdiItem->objectName())
                {
                    emit openItem->tryReopening(openItem->m_d.data());
					break;
                }
            }
        }

        setFocus(generateMdiItemId(mdiItem));

        delete obj;
        return Q_NULLPTR;
    }

    QWidget* wid = mdiItem->widget();

    QMdiSubWindow* subWin = m_area->addSubWindow(wid);
    subWin->setWindowTitle(mdiItem->objectName());

    QIcon icon = mdiItem->icon();

    if (!icon.isNull())
    {
        subWin->setWindowIcon(mdiItem->icon());
    }
    else
    {
        subWin->setWindowIcon(gtApp->icon(QStringLiteral("frameIcon.png")));
    }

    mdiItem->initialized();

    connect(subWin,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)),
            mdiItem,
            SLOT(windowStateChanged(Qt::WindowStates, Qt::WindowStates)));
    connect(subWin, SIGNAL(aboutToActivate()), mdiItem,
            SLOT(windowAboutToActive()));
    connect(subWin, SIGNAL(destroyed(QObject*)), mdiItem,
           SLOT(deleteLater()));
    connect(subWin, SIGNAL(destroyed(QObject*)),
            SLOT(onSubWindowClose(QObject*)));
    connect(mdiItem, SIGNAL(objectNameChanged(QString)),
            subWin, SLOT(setWindowTitle(QString)));
    connect(mdiItem, SIGNAL(destroyed(QObject*)), subWin,
            SLOT(deleteLater()));

    m_openItems.insert(subWin, mdiItem);

    mdiItem->setSubWin(subWin);

    subWin->show();
    mdiItem->showEvent();

    if (data != Q_NULLPTR)
    {
        mdiItem->setData(data);
    }

    return mdiItem;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, QString customId)
{
    return open(id, Q_NULLPTR, customId);
}
