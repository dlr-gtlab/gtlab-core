#include "gt_abstractchartprovider.h"
#include "gt_stringcontainer.h"
#include "gt_objectgroup.h"
#include "gt_datamodel.h"
#include "gt_colors.h"

#include <gt_logging.h>

#include <QRandomGenerator>

GtAbstractChartProvider::GtAbstractChartProvider() :
    m_dztUUIDsContainer(nullptr),
    m_showmarkers(true),
    m_showwidgets(true)
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
        if (container)
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
        container = nullptr;
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
        if (container)
        {
            if (container->entry() == uuid)
            {
                gtWarning() << tr("Object uuid does already exist") << ". '"
                            << uuid << "'";

                return;
            }
        }
    }

    UUIDsContainer()->appendChild(new GtStringContainer(uuid));
}

QStringList
GtAbstractChartProvider::objectNamesFromContainer() const
{
    QStringList retval;

    foreach (GtStringContainer* container,
             UUIDsContainer()->findDirectChildren<GtStringContainer*>())
    {
        if (!container)
        {
            continue;
        }

        QString uuidContainer = container->entry();

        GtObject* obj = gtDataModel->objectByUuid(uuidContainer);

        if (!obj)
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
        if (!container)
        {
            continue;
        }

        delete container;
    }
}

QColor
GtAbstractChartProvider::colorUseRAt(int i) const
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
GtAbstractChartProvider::addColorUseR(QColor const& col)
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
            addColorUseL(gt::gui::color::randomColor());
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
            addColorUseR(gt::gui::color::randomColor());
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
GtAbstractChartProvider::addColorUseL(QColor const& col)
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
GtAbstractChartProvider::setSingleColorsUseR(int index, const QColor& col)
{
    m_colorsR.replace(index, col.name());
}

void
GtAbstractChartProvider::setSingleColorsUseL(int index, const QColor& col)
{
    m_colorsL.replace(index, col.name());
}

void
GtAbstractChartProvider::fixMainValues(const QMap<QString, QString>& fixedMain,
                                       QString& fixXmain, QString& fixYmain,
                                       QString& fixZmain)
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

    if (!g)
    {
        gtError() << "Group 'UUID' not found in Abstract Chart provider";
        return nullptr;
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
GtAbstractChartProvider::prefixToStringList(const QStringList& list,
                                            const QString &prefix) const
{
    QStringList stringsMod;

    foreach (QString s, list)
    {
        stringsMod.append(prefix + s);
    }

    return stringsMod;
}
