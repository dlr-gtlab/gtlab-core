#include "gt_datazonetablelist.h"
#include "gt_datazonetable.h"
#include <QMap>
#include <QsLog.h>
#include "gt_objectfactory.h"

GtDataZoneTableList::GtDataZoneTableList() :
    m_dztList(QList<GtDataZoneTable*>())
{
    setFactory(gtObjectFactory);
    setObjectName("Data Zone Table List");
}

bool
GtDataZoneTableList::addDzt(GtDataZoneTable* dzt, const QString& uuid)
{
    if (dzt == nullptr)
    {
        return false;
    }

    if (!isValid(dzt))
    {
        // 27.01.2017 do not check if the grid is the same to allow for
        // maps being plotted (each label is a GtDataZoneTable)
        gtWarning() << tr("New dataZoneTable does not fit to existing ones!");
        return false;
    }

    dzt->setObjectName(uuid);
    m_dztList.append(dzt);
    appendChild(dzt);

    return true;
}

int
GtDataZoneTableList::size() const
{
    return m_dztList.size();
}

bool
GtDataZoneTableList::isEmpty() const
{
    if (size() == 0)
    {
        return true;
    }

    return false;
}


int
GtDataZoneTableList::numXticks() const
{
    int retval = 0;

    if (dzt() != nullptr)
    {
        retval = dzt()->nXDims();
    }

    return retval;
}

int
GtDataZoneTableList::numYticks() const
{
    int retval = 0;

    if (dzt() != nullptr)
    {
        retval = dzt()->nYDims();
    }

    return retval;
}

int
GtDataZoneTableList::numZticks() const
{
    int retval = 0;

    if (dzt() != nullptr)
    {
        retval = dzt()->nZDims();
    }

    return retval;
}

int
GtDataZoneTableList::xAxisIndexFromString(const QString& str) const
{
    return dzt()->xAxisIndexFromString(str);
}

int
GtDataZoneTableList::yAxisIndexFromString(const QString& str) const
{
    return dzt()->yAxisIndexFromString(str);
}

int
GtDataZoneTableList::zAxisIndexFromString(const QString& str) const
{
    return dzt()->zAxisIndexFromString(str);
}

QList<GtDataZoneTable*>
GtDataZoneTableList::list() const
{
    return m_dztList;
}

bool
GtDataZoneTableList::isValid(GtDataZoneTable* newDzt)
{
    if (m_dztList.size() == 0)
    {
        // first datazoneTable, so no check nessecary, everything is fine
        return true;
    }

    if (newDzt == nullptr)
    {
        // something went wrong, first dzt in dztList is a Q_NULLPTR
        gtWarning() << tr("New DataZoneTable is a Nullptr, returning false ");
        return false;
    }

    GtDataZoneTable* dztTemp = dzt();

    if (dztTemp == nullptr)
    {
        gtWarning() << tr("dztTemp DataZoneTable is a Nullptr, "
                          "returning false ");
        return false;
    }

    int nSubTemp = dztTemp->nSubDims();
    int nSubNew = newDzt->nSubDims();
    if (nSubTemp != nSubNew)
    {
        gtWarning() << tr("Sub dimensions of new DataZoneTable does not fit "
                          "to existing one. Existing = ") << nSubTemp
                    << tr(", new one = ") << nSubNew << tr(". Returning false.");
        return false;
    }

    if (dztTemp->xAxisName() != newDzt->xAxisName())
    {
        return false;
    }

    if (dztTemp->yAxisName() != newDzt->yAxisName())
    {
        return false;
    }

    if (dztTemp->zAxisName() != newDzt->zAxisName())
    {
        return false;
    }

    if (dztTemp->xAxis() != newDzt->xAxis())
    {
        return false;
    }

    if (dztTemp->yAxis() != newDzt->yAxis())
    {
        return false;
    }

    if (dztTemp->zAxis() != newDzt->zAxis())
    {
        return false;
    }

    if (dztTemp->subAxisNames() != newDzt->subAxisNames())
    {
        return false;
    }

    for (int i = 0; i < dztTemp->nSubDims(); i++)
    {
        QStringList listTemp = dztTemp->allAxisTicksString().at(i);
        QStringList listNew = newDzt->allAxisTicksString().at(i);

        if (listTemp != listNew)
        {
            return false;
        }
    }

    return true;
}

void
GtDataZoneTableList::fixMainValues(QMap<QString, QString> fixedMain,
                                   int& fixXmain, int& fixYmain, int& fixZmain)
{
    // get the fix X, Y, Z axis of the main datazonetable
    if (fixedMain.contains("mainX"))
    {
        fixXmain = xAxisIndexFromString(fixedMain.value("mainX"));
    }

    if (fixedMain.contains("mainY"))
    {
        fixYmain = yAxisIndexFromString(fixedMain.value("mainY"));
    }

    if (fixedMain.contains("mainZ"))
    {
        fixZmain = zAxisIndexFromString(fixedMain.value("mainZ"));
    }
}

GtDataZoneTable*
GtDataZoneTableList::dzt() const
{
    GtDataZoneTable* retval = nullptr;

    if (m_dztList.size() > 0)
    {
        retval = m_dztList.first();
    }

    return retval;
}

bool
GtDataZoneTableList::onlyXaxisActive() const
{
    GtDataZoneTable* dztt = dzt();

    if (dztt == nullptr)
    {
        return false;
    }

    return dztt->onlyXaxisActive();
}

GtDataZoneTable*
GtDataZoneTableList::dzt(const QString& param) const
{
    for (GtDataZoneTable* t : m_dztList)
    {
        if (t->params().contains(param))
        {
            return t;
        }
    }

    gtDebug() << tr("No datazonetable contains parameter '") << param << "'";

    return nullptr;
}

GtDataZoneTable*
GtDataZoneTableList::dztFromName(const QString& dztName) const
{
    for (GtDataZoneTable* t : m_dztList)
    {
        if (t->objectName() == dztName)
        {
            return t;
        }
    }

    gtError() << tr("No datazonetable is named '") << dztName << "'";

    return nullptr;
}

void
GtDataZoneTableList::clearList()
{
    for (GtDataZoneTable* t : m_dztList)
    {
        delete t;
    }

    m_dztList.clear();
}

void
GtDataZoneTableList::onObjectDataMerged()
{
    m_dztList = findDirectChildren<GtDataZoneTable*>();
}

int
GtDataZoneTableList::nSubDims() const
{
    return dzt()->nSubDims();
}

QStringList
GtDataZoneTableList::params() const
{
    QStringList paramsUnique;

    for (GtDataZoneTable* t : list())
    {
        if (t != nullptr)
        {
            for (const QString& str : t->params())
            {
                if (!paramsUnique.contains(str))
                {
                    paramsUnique.append(str);
                }
            }
        }
    }

    return paramsUnique;
}

QMap<QString, QString>
GtDataZoneTableList::paramsAndUnits() const
{
    QMap<QString, QString> paramsUnits;

    QStringList allParams = params();

    for (const QString& str : allParams)
    {
        QString unit = unitFromParam(str);

        paramsUnits.insert(str, unit);
    }

    return paramsUnits;
}

QString
GtDataZoneTableList::unitFromParam(const QString& param) const
{
    QString unit;

    GtDataZoneTable* t = dzt(param);

    if (t != nullptr)
    {
        unit = t->unitFromParam(param);
    }
    else
    {
        gtDebug() << tr("Param ") << param
                  << tr(" cannot be found in any DataZoneTable, "
                        "no unit retrieved.");
    }

    return unit;
}

QStringList
GtDataZoneTableList::paramsWithUnits(const QStringList& params) const
{
    QStringList paramsUnits;

    for (const QString& str : params)
    {
        QString unit = unitFromParam(str);

        paramsUnits.append(str + " " + unit);
    }

    return paramsUnits;
}

bool
GtDataZoneTableList::is0D() const
{
    if (dzt() != nullptr)
    {
        return dzt()->is0D();
    }
    else
    {
        gtWarning() << tr("Fist element of DataZoneTableList is a nulltr.");
        return false;
    }
}
