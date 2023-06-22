/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_filechooserproperty.h
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
     * @param File extension filter list (eg. ".exe", or "Executable (*.exe)")
     */
    GtFileChooserProperty(const QString& id,
                          const QString& name,
                          const QString& brief,
                          const QStringList& fileFilter);

private:
    /// File extension filter.
    QStringList m_filter;

};

namespace gt
{

namespace detail
{


 /**
 * @brief This function takes several file filters and concatenates them
 *        to be used for the filedialog filter entries
 *
 * It supports old-style filters (e.g. ".txt")
 * and Qt-style filters (e.g. "Textfile (*.txt)");
 *
 * The filter styles can be combined arbitrarily
 *
 * @param filterList List of filters
 * @return Concatenated filter string to be used by QFileDialog
 */
GT_DATAMODEL_EXPORT QString
buildFileDialogFileFilter(const QStringList& filterList);

} // namespace detail
} // namespace gt

#endif // GTFILECHOOSERPROPERTY_H
