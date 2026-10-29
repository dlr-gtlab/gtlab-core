/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_RESTORE_IMPORTER_H
#define TEST_MEMENTO_RESTORE_IMPORTER_H

#include "importer_interface_exports.h"

#include "gt_abstractimporter.h"

/**
 * @brief Importer that restores the data of the targeted object from an
 * exported memento (XML). Acts on the data objects themselves, comparable
 * to a restore feature: the object identity (uuid) is always kept, the
 * object name can be kept or be overwritten with the exported name.
 *
 * Used to test the importer interface of GTlab.
 */
class TEST_IMPORTER_INTERFACE_EXPORT TestMementoRestoreImporter
    : public GtAbstractImporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMementoRestoreImporter();

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
     * @brief Main import function. Overwrites the data of @p data with
     * the contents of the memento file.
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

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    QList<GtAbstractImporterPage*> pages(GtObject* data) override;

    /**
     * @brief Sets whether the object name of the target is kept.
     * @param keep Keep object name indicator
     */
    void setKeepObjectName(bool keep);

    /**
     * @brief Returns whether the object name of the target is kept.
     * @return Keep object name indicator
     */
    bool keepObjectName() const;

private:
    /// Importer option, set by the importer wizard page
    bool m_keepObjectName = true;
};

#endif // TEST_MEMENTO_RESTORE_IMPORTER_H
