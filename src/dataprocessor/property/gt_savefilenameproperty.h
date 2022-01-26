/* GTlab - Gas Turbine laboratory
 * Source File: gt_savefilenameproperty.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 12.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSAVEFILENAMEPROPERTY_H
#define GTSAVEFILENAMEPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_filechooserproperty.h"

/**
 * @brief The GtSaveFileNameProperty class
 */
class GT_DATAMODEL_EXPORT GtSaveFileNameProperty : public GtFileChooserProperty
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Property identification string.
     * @param Property name.
     * @param Property description.
     * @param File extension filter (eg. ".exe").
     * @param Initial filename.
     */
    GtSaveFileNameProperty(const QString& id,
                           const QString& name,
                           const QString& brief,
                           const QStringList& fileFilter,
                           const QString& initFileName);

    /**
     * @brief Returns initial filename.
     * @return Initial filename.
     */
    const QString& initFileName();

private:
    /// Initial file name.
    QString m_initFileName;

};

#endif // GTSAVEFILENAMEPROPERTY_H
