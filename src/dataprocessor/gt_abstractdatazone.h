/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTABSTRACTDATAZONE_H
#define GTABSTRACTDATAZONE_H

#include "gt_externalizedobject.h"

#include <QStringList>


class GtAbstractDataZone;
class GT_DATAMODEL_EXPORT GtAbstractDataZoneData :
        public GtExternalizedObjectData
{    
    GT_DECL_BASECLASS(GtAbstractDataZone)

public:

    explicit GtAbstractDataZoneData(GtAbstractDataZone* base);

    /**
     * @brief Returns true if size of params, units and values matches.
     * @return Data validity indicator.
     */
    bool isValid() const override;

    /**
     * @brief Returns unit of param
     * @return unit
     */
    QString unit(const QString& param) const;

    /**
     * @brief Appends the module name to all parameters
     * @param moduleName name
     */
    virtual void addModuleName(const QString& moduleName);

    /**
     * @brief Returns all parameters
     * @return parameters
     */
    const QStringList& params() const;

    /**
     * @brief Setter for parameters
     * @param params new params
     */
    void setParams(const QStringList& params);

    /**
     * @brief Returns all units
     * @return units
     */
    const QStringList& units() const;

    /**
     * @brief Setter for units
     * @param params new units
     */
    void setUnits(const QStringList& params);

    /**
     * @brief hasParam - check if parameter list contains a given parameter
     * @param param - parameter to check
     * @return true if parameter list contains param
     */
    bool hasParam(const QString& param) const;
};


/**
 * @brief The GtAbstractDataZone class
 */
class GT_DATAMODEL_EXPORT GtAbstractDataZone : public GtExternalizedObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList params MEMBER m_params)

    Q_PROPERTY(QStringList units MEMBER m_units)

    GT_DECL_DATACLASS(GtAbstractDataZoneData)

public:

    /**
     * @brief is0D
     * @return whether this object represents a 0D datazone
     */
    virtual bool is0D() const = 0;

    /**
     * @brief nDims
     * @return number of axis/dimensions
     */
    Q_INVOKABLE virtual int nDims() const = 0;

protected:
    /**
     * @brief GtAbstractDataZone
     */
    Q_INVOKABLE GtAbstractDataZone();

    /// Parameter names
    QStringList m_params{};

    /// Parameter units
    QStringList m_units{};
};

#endif // GTABSTRACTDATAZONE_H
