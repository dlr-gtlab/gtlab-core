/* GTlab - Gas Turbine laboratory
 * Source File: gt_openfilenameproperty.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 12.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOPENFILENAMEPROPERTY_H
#define GTOPENFILENAMEPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_filechooserproperty.h"

/**
 * @brief The GtOpenFileNameProperty class
 */
class GT_DATAMODEL_EXPORT GtOpenFileNameProperty : public GtFileChooserProperty
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Property identification string.
     * @param Property name.
     * @param Property description.
     * @param File extension filter (eg. ".exe").
     */
    GtOpenFileNameProperty(const QString& id,
                           const QString& name,
                           const QString& brief,
                           const QStringList& fileFilter);

};

#endif // GTOPENFILENAMEPROPERTY_H
