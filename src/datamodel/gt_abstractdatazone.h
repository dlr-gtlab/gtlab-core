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

#include "gt_datamodel_exports.h"

#include "gt_externalizedh5object.h"

#include <QStringList>

/**
 * @brief The GtAbstractDataZone class
 */
class GT_DATAMODEL_EXPORT GtAbstractDataZone : public GtExternalizedH5Object
{
    Q_OBJECT

    Q_PROPERTY(QStringList params READ params WRITE setParams)

    Q_PROPERTY(QStringList units READ units WRITE setUnits)

public:

    QStringList params();

    void setParams(const QStringList &params);

    QStringList units();

    void setUnits(const QStringList &params);

    virtual QString unit(QString param) = 0;

    virtual bool is0D() = 0;

    virtual int nDims() = 0;

    virtual void addModuleName(QString moduleName) = 0;

protected:
    /**
     * @brief GtAbstractDataZone
     */
    Q_INVOKABLE GtAbstractDataZone();

    /**
     * @brief doFetch method to implement for fetching the externalized data.
     * @return success
     */
    virtual bool doFetchData() = 0;

    /**
     * @brief doExternalize method to implement for externalizing the data.
     * @return success
     */
    virtual bool doExternalizeData() = 0;

    /**
     * @brief doClearExternalizedData method to implement for clearing the
     * externalized data. Should not emit a signal or set any object flags.
     */
    virtual void doClearExternalizedData() = 0;

    /// Parameter names
    QStringList m_params;

    /// Parameter units
    QStringList m_units;
};

#endif // GTABSTRACTDATAZONE_H
