/* GTlab - Gas Turbine laboratory
 * Source File: gt_filechooserproperty.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFILECHOOSERPROPERTY_H
#define GTFILECHOOSERPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"

/**
 * @brief The GtFileChooserProperty class
 */
class GT_DATAMODEL_EXPORT GtFileChooserProperty : public GtStringProperty
{
    Q_OBJECT

public:
    /**
     * @brief Returns list of all file extension filter.
     * @return List of file extension filter.
     */
    const QStringList& filter();

protected:
    /**
     * @brief Constructor.
     * @param Property identification string.
     * @param Property name.
     * @param Property description.
     * @param File extension filter (eg. ".exe")
     */
    GtFileChooserProperty(const QString& id,
                          const QString& name,
                          const QString& brief,
                          const QStringList& fileFilter);

private:
    /// File extension filter.
    QStringList m_filter;

};

GT_DATAMODEL_EXPORT QString
buildDialogFileFilter(const QStringList& filterList);

#endif // GTFILECHOOSERPROPERTY_H
