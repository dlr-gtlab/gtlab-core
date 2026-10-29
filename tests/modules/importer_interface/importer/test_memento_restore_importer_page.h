/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_RESTORE_IMPORTER_PAGE_H
#define TEST_MEMENTO_RESTORE_IMPORTER_PAGE_H

#include "importer_interface_exports.h"
#include "test_memento_restore_importer.h"

#include "gt_abstractimporterpage.h"

#include <QPointer>

class QCheckBox;

/**
 * @brief Wizard page of the memento restore importer. Allows to keep the
 * name of the target object instead of overwriting it with the name stored
 * in the memento.
 */
class TEST_IMPORTER_INTERFACE_EXPORT TestMementoRestoreImporterPage
    : public GtAbstractImporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object.
     * @param Importer to be modified by the page options.
     * @param Parent widget.
     */
    explicit TestMementoRestoreImporterPage(
        GtObject* obj, TestMementoRestoreImporter* importer,
        QWidget* parent = nullptr);

private:
    /// Checkbox to keep the object name
    QCheckBox* m_keepName = nullptr;

    /// Pointer to importer
    QPointer<TestMementoRestoreImporter> m_importer;
};

#endif // TEST_MEMENTO_RESTORE_IMPORTER_PAGE_H
