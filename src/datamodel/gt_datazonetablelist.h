#ifndef GTDATAZONETABLELIST_H
#define GTDATAZONETABLELIST_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"
#include <QStringList>
#include <QVector>

class GtDataZoneTable;

/**
 * @brief The GtDataZoneTableList class
 */
class GT_DATAMODEL_EXPORT GtDataZoneTableList : public GtObject
{
    Q_OBJECT


public:
    /**
     * @brief GtDataZoneTableList
     */
    Q_INVOKABLE GtDataZoneTableList();

    /**
     * @brief addDzt
     * @param dzt
     */
    bool addDzt(GtDataZoneTable* dzt, QString uuid);

    /**
     * @brief size
     * @return
     */
    int size();

    /**
     * @brief empty
     * @return
     */
    bool isEmpty();

    /**
     * @brief nXDims
     * @return
     */
    int numXticks();

    /**
     * @brief nYDims
     * @return
     */
    int numYticks();

    /**
     * @brief nZDims
     * @return
     */
    int numZticks();

    /**
     * @brief xAxisIndexFromString
     * @param str
     * @return
     */
    int xAxisIndexFromString(QString str);

    /**
     * @brief yAxisIndexFromString
     * @param str
     * @return
     */
    int yAxisIndexFromString(QString str);

    /**
     * @brief zAxisIndexFromString
     * @param str
     * @return
     */
    int zAxisIndexFromString(QString str);

    /**
     * @brief list
     * @return
     */
    QList<GtDataZoneTable*> list();

    /**
     * @brief dzt
     * @return
     */
    GtDataZoneTable* dzt();

    bool onlyXaxisActive();

    /**
     * @brief dzt
     * @param param
     * @return
     */
    GtDataZoneTable* dzt(QString param);


    GtDataZoneTable* dztFromName(QString dztName);

    /**
     * @brief nSubDims
     * @return
     */
    int nSubDims();

    /**
     * @brief get all UNIQUE params of all Datazones that are in all
     * datazonetables of this list
     * @return
     */
    QStringList params();

    /**
     * @brief paramsAndUnits
     * @return
     */
    QMap<QString, QString> paramsAndUnits();

    /**
     * @brief unitFromParam
     * @return
     */
    QString unitFromParam(QString param);

    /**
     * @brief paramsWithUnits
     * @param params
     * @return
     */
    QStringList paramsWithUnits(QStringList params);

    /**
     * @brief is0D
     * @return
     */
    bool is0D();

    void clearList();

private:

    void onObjectDataMerged();

    /// list of datazonetable
    QList<GtDataZoneTable*> m_dztList;

    /**
     * @brief isValid: checks if given datazoneTable has same
     * axes and axes-ticks as the preexisting onces
     * @param dzt
     */
    bool isValid(GtDataZoneTable* newDzt);

    /**
     * @brief fixMainValues
     * @param fixedMain
     * @param fixXmain
     * @param fixYmain
     * @param fixZmain
     */
    void fixMainValues(QMap<QString, QString> fixedMain,
                       int& fixXmain, int& fixYmain, int& fixZmain);

};

#endif // GTDATAZONETABLELIST_H
