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
    bool addDzt(GtDataZoneTable* dzt, const QString& uuid);

    /**
     * @brief size
     * @return
     */
    int size() const;

    /**
     * @brief empty
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief nXDims
     * @return
     */
    int numXticks() const;

    /**
     * @brief nYDims
     * @return
     */
    int numYticks() const;

    /**
     * @brief nZDims
     * @return
     */
    int numZticks() const;

    /**
     * @brief xAxisIndexFromString
     * @param str
     * @return
     */
    int xAxisIndexFromString(const QString& str) const;

    /**
     * @brief yAxisIndexFromString
     * @param str
     * @return
     */
    int yAxisIndexFromString(const QString& str) const;

    /**
     * @brief zAxisIndexFromString
     * @param str
     * @return
     */
    int zAxisIndexFromString(const QString &str) const;

    /**
     * @brief list
     * @return
     */
    QList<GtDataZoneTable*> list() const;

    /**
     * @brief dzt
     * @return
     */
    GtDataZoneTable* dzt() const;

    bool onlyXaxisActive() const;

    /**
     * @brief dzt
     * @param param
     * @return
     */
    GtDataZoneTable* dzt(const QString& param) const;


    GtDataZoneTable* dztFromName(const QString& dztName) const;

    /**
     * @brief nSubDims
     * @return
     */
    int nSubDims() const;

    /**
     * @brief get all UNIQUE params of all Datazones that are in all
     * datazonetables of this list
     * @return
     */
    QStringList params() const;

    /**
     * @brief paramsAndUnits
     * @return
     */
    QMap<QString, QString> paramsAndUnits() const;

    /**
     * @brief unitFromParam
     * @return
     */
    QString unitFromParam(const QString& param) const;

    /**
     * @brief paramsWithUnits
     * @param params
     * @return
     */
    QStringList paramsWithUnits(const QStringList& params) const;

    /**
     * @brief is0D
     * @return
     */
    bool is0D() const;

    void clearList();

private:

    void onObjectDataMerged() override;

    /// list of datazonetable
    QList<GtDataZoneTable*> m_dztList;

    /**
     * @brief isValid: checks if given datazoneTable has same
     * axes and axes-ticks as the preexisting onces
     * @param newDzt
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
