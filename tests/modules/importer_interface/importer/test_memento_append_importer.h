/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_APPEND_IMPORTER_H
#define TEST_MEMENTO_APPEND_IMPORTER_H

#include "importer_interface_exports.h"

#include "gt_abstractimporter.h"

/**
 * @brief Importer that restores an object from an exported memento (XML)
 * and adds it as child to the targeted object. Uses the default wizard
 * pages of the importer interface.
 */
class TEST_IMPORTER_INTERFACE_EXPORT TestMementoAppendImporter
    : public GtAbstractImporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMementoAppendImporter();

    /**
     * @brief Returns list of allowed target classes of importer.
     * @return List of allowed target classes.
     */
    QStringList targets() const override;

    /**
     * @brief Returns icon of importer.
     * @return Icon of importer.
     */
    QIcon icon() const override;

    /**
     * @brief Main import function. Adds the object stored in @p file as
     * child of @p data.
     * @param Pointer to datamodel member (object clone used by the wizard).
     * @param Reference to file selected by user.
     * @return Whether import successfully finished or not.
     */
    bool import(GtObject* data, QFile& file) override;

    /**
     * @brief Returns list of allowed file extensions of importer.
     * @return List of allowed file extensions.
     */
    QStringList extensions() const override;
};

#endif // TEST_MEMENTO_APPEND_IMPORTER_H
