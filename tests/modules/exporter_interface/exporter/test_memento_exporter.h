/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_EXPORTER_H
#define TEST_MEMENTO_EXPORTER_H

#include "exporter_interface_exports.h"

#include "gt_abstractexporter.h"

/**
 * @brief Exporter with explicit target list. Writes the memento (XML) of
 * objects of the test data model interface (TestDmiExternalObject,
 * TestDmiData). Relies on the default exporter wizard page for file
 * selection.
 */
class TEST_EXPORTER_INTERFACE_EXPORT TestMementoExporter
    : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMementoExporter();

    /**
     * @brief Returns list of allowed target classes of exporter.
     * @return List of allowed target classes.
     */
    QStringList targets() const override;

    /**
     * @brief Returns icon of exporter.
     * @return Icon of exporter.
     */
    QIcon icon() const override;

    /**
     * @brief Main export function.
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @return Whether export successfully finished or not.
     */
    bool doExport(GtObject* data, QFile& file) override;

    /**
     * @brief Returns file extensions of exporter.
     * @return File extensions of exporter.
     */
    QString extension() const override;
};

#endif // TEST_MEMENTO_EXPORTER_H
