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
 * and adds it as child to the targeted object. The name of the appended
 * object can be modified by a suffix using the importer wizard page.
 */
class TEST_IMPORTER_INTERFACE_EXPORT TestMementoAppendImporter :
        public GtAbstractImporter
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

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    QList<GtAbstractImporterPage*> pages(GtObject* data) override;

    /**
     * @brief Sets the suffix which is appended to the name of the
     * imported object.
     * @param suffix Name suffix
     */
    void setSuffix(const QString& suffix);

    /**
     * @brief Returns the suffix which is appended to the name of the
     * imported object.
     * @return Name suffix
     */
    QString suffix() const;

private:
    /// Importer option, set by the importer wizard page
    QString m_suffix = QStringLiteral("_imported");
};

#endif // TEST_MEMENTO_APPEND_IMPORTER_H
