#include "gt_abstractchartprovider.h"
#include "gt_datazonetablelist.h"
#include "gt_datazonetable.h"
#include "gt_stringcontainer.h"
#include "gt_objectgroup.h"
#include "gt_datamodel.h"
#include "gt_project.h"

#include <QDebug>
#include <QsLog.h>


GtAbstractChartProvider::GtAbstractChartProvider() :
    m_showmarkers(true),
    m_showwidgets(true),
    m_dztUUIDsContainer(Q_NULLPTR)
{
    GtObjectGroup* UUIDsContainer = new GtObjectGroup;
    UUIDsContainer->setDefault(true);
    UUIDsContainer->setObjectName("UUID");
    appendChild(UUIDsContainer);
}

QStringList
GtAbstractChartProvider::uuids() const
{
    QStringList retval;

    GtObjectGroup* o = UUIDsContainer();

    foreach (GtStringContainer* container,
             o->findDirectChildren<GtStringContainer*>())
    {
        if (container != Q_NULLPTR)
        {
            retval.append(container->entry());
        }
    }

    return retval;
}

void
GtAbstractChartProvider::setUUIDs(const QStringList& uuids)
{
    foreach (GtStringContainer* container,
             UUIDsContainer()->findDirectChildren<GtStringContainer*>())
    {
        delete container;
        container = Q_NULLPTR;
    }

    foreach (QString str, uuids)
    {
        UUIDsContainer()->appendChild(new GtStringContainer(str));
    }
}

void
GtAbstractChartProvider::addUUID(const QString& uuid)
{
    foreach (GtStringContainer* container,
             UUIDsContainer()->findDirectChildren<GtStringContainer*>())
    {
        if (container != Q_NULLPTR)
        {
            if (container->entry() == uuid)
            {
                gtWarning() << tr("Datazone uuid does already exist") << ". '"
                            << uuid << "'";

                return;
            }
        }
    }

    UUIDsContainer()->appendChild(new GtStringContainer(uuid));
}

QStringList
GtAbstractChartProvider::objectNamesFromContainer()
{
    QStringList retval;

    foreach (GtStringContainer* container,
             UUIDsContainer()->findDirectChildren<GtStringContainer*>())
    {
        if (container == Q_NULLPTR)
        {
            continue;
        }

        QString uuidContainer = container->entry();

        GtObject* obj = gtDataModel->objectByUuid(uuidContainer);

        if (obj == Q_NULLPTR)
        {
            continue;
        }

        retval.append(obj->objectName());
    }

    return retval;
}

void
GtAbstractChartProvider::clearUUIDsContainer()
{
    foreach (GtStringContainer* container,
             UUIDsContainer()->findDirectChildren<GtStringContainer*>())
    {
        if (container == Q_NULLPTR)
        {
            continue;
        }

        delete container;
    }
}

QColor
GtAbstractChartProvider::colorUseRAt(int i)
{
    if (i >= m_colorsR.size())
    {
        gtDebug() << tr("Chart Provider: Right colors out of bounds, selecting"
                        " first color of color use.");

        return m_colorsR.first();
    }

    return m_colorsR.at(i);
}

QList<QString>
GtAbstractChartProvider::colorsUseR() const
{
    return m_colorsR;
}

void
GtAbstractChartProvider::setColorsUseR(const QList<QString>& colorsUse)
{
    m_colorsR = colorsUse;
}

void
GtAbstractChartProvider::addColorUseR(QColor col)
{
    m_colorsR.append(col.name());
}

QColor
GtAbstractChartProvider::colorUseLAt(int i)
{
    if (i >= m_colorsL.size())
    {
        gtWarning() << tr("Chart Provider: Left colors out of bounds, selecting"
                        " first color of color use.");
        return QColor(m_colorsL.first());
    }

    return QColor(m_colorsL.at(i));
}

void
GtAbstractChartProvider::fitColorsL(int params)
{
    int colors = m_colorsL.size();

    if (params < colors)
    {
        // delete too many colors
        for (int i = params; i < colors; ++i)
        {
            m_colorsL.removeLast();
        }
    }
    else
    {
        // add colors to match parameter sizes
        for (int i = colors; i < params; ++i)
        {
            QColor color = QColor::fromHsv(qrand() % 256, 255, 190);
            addColorUseL(color);
        }
    }
}

void
GtAbstractChartProvider::fitColorsR(int params)
{
    int colors = m_colorsR.size();

    if (params < colors)
    {
        // delete too many colors
        for (int i = params; i < colors; ++i)
        {
            m_colorsR.removeLast();
        }
    }
    else
    {
        // add colors to match parameter sizes
        for (int i = colors; i < params; ++i)
        {
            QColor color = QColor::fromHsv(qrand() % 256, 255, 190);
            addColorUseR(color);
        }
    }
}

QList<QString>
GtAbstractChartProvider::colorsUseL() const
{
    return m_colorsL;
}

void
GtAbstractChartProvider::setColorsUseL(const QList<QString>& colorsUse)
{
    m_colorsL = colorsUse;
}

void
GtAbstractChartProvider::addColorUseL(QColor col)
{
    m_colorsL.append(col.name());
}

//void
//GtAbstractChartProvider::clearColorsUse()
//{
//    clearColorsUseL();
//    clearColorsUseR();
//}

void
GtAbstractChartProvider::clearColorsUseL()
{
    m_colorsL.clear();
}

void
GtAbstractChartProvider::clearColorsUseR()
{
    m_colorsR.clear();
}

void
GtAbstractChartProvider::setSingleColorsUseR(int index, QColor col)
{
    m_colorsR.replace(index, col.name());
}

void
GtAbstractChartProvider::setSingleColorsUseL(int index, QColor col)
{
    m_colorsL.replace(index, col.name());
}

int
GtAbstractChartProvider::bottomIterator(GtDataZoneTableList* dztList,
                                        QString bottomAxis,
                                        QVector<double>& axisTicks)
{
    // get how many data sets there are at bottom

    int bottomIterator = 1;

    if (bottomAxis == "mainX")
    {
        // bottom axis is main x axis, use main x ticks number
        bottomIterator = dztList->numXticks();
    }
    else if (bottomAxis == "mainY")
    {
        // bottom axis is main y axis, use main y ticks number
        bottomIterator = dztList->numYticks();
    }
    else if (bottomAxis == "mainZ")
    {
        // bottom axis is main z axis, use main z ticks number
        bottomIterator = dztList->numZticks();
    }
    else
    {
        // bottom axis is one of the sub axes
        if (!dztList->dzt()->allAxisTicksMap().contains(bottomAxis))
        {
            // bottom axis is neither main nor sub axis
            gtError() << tr("Bottom axis is neither a main axis, "
                            "nor a sub axis, abort!");
            return bottomIterator;
        }
        else
        {
            // bottom axis is sub axis, use its axis ticks
            axisTicks = dztList->dzt()->allAxisTicksMap().value(bottomAxis);
            bottomIterator = axisTicks.size();
        }
    }

    return bottomIterator;
}

int
GtAbstractChartProvider::bottomIterator(
        GtDataZoneTableList *dztList, QString bottomAxis)
{
    QVector<double> axisTicks;

    return bottomIterator(dztList, bottomAxis, axisTicks);
}

int
GtAbstractChartProvider::additionalIterator(GtDataZoneTableList* dztList,
                                            QString additionalAxis,
                                            QStringList scharParameters,
                                            QVector<double>& axisTicks)
{
    // get how many data sets there are additional (that means schar)

    int additionalIterator = 1;

    if (additionalAxis == "mainX")
    {
        // additional axis is main x axis, use main x ticks number
        additionalIterator = dztList->numXticks();
    }
    else if (additionalAxis == "mainY")
    {
        // additional axis is main y axis, use main y ticks number
        additionalIterator = dztList->numYticks();
    }
    else if (additionalAxis == "mainZ")
    {
        // additional axis is main z axis, use main z ticks number
        additionalIterator = dztList->numZticks();
    }
    else if (additionalAxis == "OtherParams")
    {
        if (scharParameters.size() == 0)
        {
            // 'OtherParams' is selected but its size is zero -> error
            gtError() << "Additional params should be more than 0";
        }

        additionalIterator = scharParameters.size();
    }
    else
    {
        // additional parameter is one of the sub axes or NONE

        if (!dztList->dzt()->allAxisTicksMap().contains(additionalAxis))
        {
            // additional parameter is None!
            return additionalIterator;
        }
        else
        {
            // additional parameter is one of the sub axes, collect
            // ticks of 'additional' axis
            axisTicks = dztList->dzt()->allAxisTicksMap().value(additionalAxis);
            additionalIterator = axisTicks.size();
        }
    }

    return additionalIterator;
}

int
GtAbstractChartProvider::additionalIterator(GtDataZoneTableList *dztList,
                                            QString additionalAxis,
                                            QStringList scharParameters)
{

    QVector<double> axisTicks;

    return additionalIterator(dztList, additionalAxis,
                              scharParameters, axisTicks);
}

void
GtAbstractChartProvider::fixMainValues(GtDataZoneTableList* dztList,
                                       QMap<QString, QString> fixedMain,
                                       int& fixXmain, int& fixYmain,
                                       int& fixZmain)
{
    // get the fix X, Y, Z axis of the main datazonetable
    if (fixedMain.contains("mainX"))
    {
        // x axis is fixed, get corresponding x axis index of value in fixedMain
        fixXmain = dztList->xAxisIndexFromString(fixedMain.value("mainX"));
    }

    if (fixedMain.contains("mainY"))
    {
        // y axis is fixed, get corresponding y axis index of value in fixedMain
        fixYmain = dztList->yAxisIndexFromString(fixedMain.value("mainY"));
    }

    if (fixedMain.contains("mainZ"))
    {
        // z axis is fixed, get corresponding z axis index of value in fixedMain
        fixZmain = dztList->zAxisIndexFromString(fixedMain.value("mainZ"));
    }
}

void
GtAbstractChartProvider::fixMainValues(QMap<QString, QString> fixedMain,
                                       QString &fixXmain, QString &fixYmain,
                                       QString &fixZmain)
{
    // get the fix X, Y, Z axis of the main datazonetable
    if (fixedMain.contains("mainX"))
    {
        // x axis is fixed, get corresponding x axis index of value in fixedMain
        fixXmain = fixedMain.value("mainX");
    }

    if (fixedMain.contains("mainY"))
    {
        // y axis is fixed, get corresponding y axis index of value in fixedMain
        fixYmain = fixedMain.value("mainY");
    }

    if (fixedMain.contains("mainZ"))
    {
        // z axis is fixed, get corresponding z axis index of value in fixedMain
        fixZmain = fixedMain.value("mainZ");
    }
}

bool
GtAbstractChartProvider::showwidgets() const
{
    return m_showwidgets;
}

void
GtAbstractChartProvider::setShowwidgets(bool showwidgets)
{
    m_showwidgets = showwidgets;
}

GtObjectGroup*
GtAbstractChartProvider::UUIDsContainer() const
{
    GtObjectGroup* g = findDirectChild<GtObjectGroup*>("UUID");

    if (g == Q_NULLPTR)
    {
        gtError() << "Group 'UUID' not found in Abstract Chart provider";
        return Q_NULLPTR;
    }

    return g;
}

bool
GtAbstractChartProvider::showmarkers() const
{
    return m_showmarkers;
}

void
GtAbstractChartProvider::setShowmarkers(bool mod)
{
    m_showmarkers = mod;
}

QStringList
GtAbstractChartProvider::prefixToStringList(QStringList list, QString prefix)
{
    QStringList stringsMod;

    foreach (QString s, list)
    {
        stringsMod.append(prefix + s);
    }

    return stringsMod;
}