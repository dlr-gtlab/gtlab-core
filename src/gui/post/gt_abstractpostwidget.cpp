/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on:
 *  Author:  (AT-TWK)
 *  Tel.:
 */

#include "gt_abstractpostwidget.h"
#include "gt_posttemplateitem.h"
#include "QDebug"
#include "gt_datamodel.h"
#include "gt_project.h"
//#include "gt_datazonetablelist.h"
#include "gt_abstractchartprovider.h"

#include <QWidget>

#include <gt_logging.h>

GtAbstractPostWidget::GtAbstractPostWidget(QWidget* w) :
    QWidget(w),
    //m_dztList(nullptr),
    m_data(nullptr),
    m_printable(true),
    m_iconString(QString())
{
    addConfigAction(tr("Configuration Menu"),
                    QStringLiteral("showConfigurationMenu"),
                    QStringLiteral("configIcon_16.png"),
                    QStringLiteral("canShowConfigurationMenu"));

    addConfigAction(tr("Hide Widgets"),
                    QStringLiteral("hideAxisLabels"),
                    QStringLiteral("eyeOffIcon_16.png"),
                    QStringLiteral("canHideAxisLabels"),
                    QStringLiteral("canHideAxisLabels"));

    addConfigAction(tr("Show Widgets"),
                    QStringLiteral("showAxisLabels"),
                    QStringLiteral("eyeIcon_16.png"),
                    QStringLiteral("canShowAxisLabels"),
                    QStringLiteral("canShowAxisLabels"));

    addConfigAction(tr("Hide Markers"),
                    QStringLiteral("hideMarkers"),
                    QStringLiteral("eyeOffIcon_16.png"),
                    QStringLiteral("canHideMarkers"),
                    QStringLiteral("canHideMarkers"));

    addConfigAction(tr("Show Markers"),
                    QStringLiteral("showMarkers"),
                    QStringLiteral("eyeIcon_16.png"),
                    QStringLiteral("canShowMarkers"),
                    QStringLiteral("canShowMarkers"));
}

GtActionList
GtAbstractPostWidget::configActions()
{
    return m_actionList;
}

GtPostTemplateItem*
GtAbstractPostWidget::data()
{
    return m_data;
}

void
GtAbstractPostWidget::setData(GtPostTemplateItem* dm)
{
    m_data = dm;
}

bool
GtAbstractPostWidget::addConfigAction(const QString& actionText,
                                      const QString& actionMethod,
                                      const QString& actionIcon,
                                      const QString& actionVerification)
{
    if (actionText.isEmpty() || actionIcon.isEmpty() || actionMethod.isEmpty())
    {
        return false;
    }

    m_actionList << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                     actionVerification);

    return true;
}

bool
GtAbstractPostWidget::addConfigAction(const QString& actionText,
                                      const QString& actionMethod,
                                      const QString& actionIcon,
                                      const QString& actionVerification,
                                      const QString& actionVisibility)
{
    if (actionText.isEmpty() || actionIcon.isEmpty() || actionMethod.isEmpty())
    {
        return false;
    }

    m_actionList << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                     actionVerification, actionVisibility);

    return true;
}

bool
GtAbstractPostWidget::hasConfigMenu()
{
    return !m_actionList.isEmpty();
}

bool
GtAbstractPostWidget::isPrintable()
{
    return m_printable;
}

QString
GtAbstractPostWidget::iconString()
{
    return m_iconString;
}

void
GtAbstractPostWidget::setIconString(const QString& iconString)
{
    m_iconString = iconString;
}

void
GtAbstractPostWidget::initShowHideMarkers()
{

}

QString
GtAbstractPostWidget::providerName()
{
    return QString("");
}

void
GtAbstractPostWidget::createDztList(const QStringList& dztUUIDs)
{
    deleteDztList();

    //m_dztList = new GtDataZoneTableList();

    if (dztUUIDs.isEmpty())
    {
        return;
    }

    bool first = true;

    foreach (QString uuid, dztUUIDs)
    {
        GtObject* o = gtDataModel->objectByUuid(uuid);

        if (!o)
        {
            gtDebug() << "Cannot find data for uuid" << uuid;
            continue;
        }

        GtObject* parent = o->findParent<GtObject*>();

        if (!parent)
        {
            gtWarning() << tr("Parent of object'") << o->objectName()
                        << tr(" could not be found");
        }

        if (first)
        {
            // disconnect possible signal slot connection
            disconnect(parent, SIGNAL(dataChanged(GtObject*)),
                    this, SIGNAL(refreshRequest()));

            connect(parent, SIGNAL(dataChanged(GtObject*)),
                    this, SIGNAL(refreshRequest()));
        }

        //GtDataZoneTable* dzt = o->createDzt();

        //if (!dzt)
        //{
        //    gtWarning() << tr("DataZoneTable cannot be collected. "
        //                    "Path to object: '") << uuid;
        //    continue;
        //}

        //m_dztList->addDzt(dzt, uuid);

        first = false;
    }
}

void
GtAbstractPostWidget::deleteDztList()
{
    //if (m_dztList)
    //{
    //    delete m_dztList;
    //    m_dztList = nullptr;
    //}
}

void
GtAbstractPostWidget::updatePlots()
{
    updatePlot();
}

bool
GtAbstractPostWidget::canShowMarkers(GtObject*)
{
    return false;
}

bool
GtAbstractPostWidget::canHideMarkers(GtObject*)
{
    return false;
}

GtAbstractChartProvider*
GtAbstractPostWidget::chartProvider()
{
    if (!m_data)
    {
        gtWarning() << tr("Data is NULL in Abstract Post Widget");

        return nullptr;
    }

    GtAbstractChartProvider* acp =
            qobject_cast<GtAbstractChartProvider*>(m_data);

    if (!acp)
    {
        gtWarning() << tr("Data cannot be casted into Abstract Chart Provider");

        return nullptr;
    }

    return acp;
}

void
GtAbstractPostWidget::hideMarkers(GtObject*)
{

}

void
GtAbstractPostWidget::showMarkers(GtObject*)
{

}
