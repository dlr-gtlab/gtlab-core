/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_EXPORTER_PAGE_H
#define TEST_MEMENTO_EXPORTER_PAGE_H

#include "exporter_interface_exports.h"
#include "test_memento_exporter.h"

#include "gt_abstractexporterpage.h"

#include <QPointer>

class QCheckBox;

/**
 * @brief Wizard page of the memento exporter. Allows to keep the UUIDs of
 * the exported object or to request new ones.
 *
 * The page modifies the exporter directly, as the exporter instance lives
 * until the export wizard has been closed.
 */
class TEST_EXPORTER_INTERFACE_EXPORT TestMementoExporterPage
    : public GtAbstractExporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object.
     * @param Exporter to be modified by the page options.
     * @param Parent widget.
     */
    explicit TestMementoExporterPage(GtObject* obj,
                                     TestMementoExporter* exporter,
                                     QWidget* parent = nullptr);

private:
    /// Checkbox to preserve the object UUIDs
    QCheckBox* m_preserveUuids = nullptr;

    /// Pointer to exporter
    QPointer<TestMementoExporter> m_exporter;
};

#endif // TEST_MEMENTO_EXPORTER_PAGE_H
