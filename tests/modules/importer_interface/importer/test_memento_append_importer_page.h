/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_MEMENTO_APPEND_IMPORTER_PAGE_H
#define TEST_MEMENTO_APPEND_IMPORTER_PAGE_H

#include "importer_interface_exports.h"
#include "test_memento_append_importer.h"

#include "gt_abstractimporterpage.h"

#include <QPointer>

class QLabel;
class QLineEdit;

/**
 * @brief Wizard page of the append importer. Allows to modify the name of
 * the appended object by a suffix, to avoid duplicate names in the tree.
 *
 * The page modifies the importer directly, as the importer instance lives
 * until the import wizard has been closed.
 */
class TEST_IMPORTER_INTERFACE_EXPORT TestMementoAppendImporterPage :
        public GtAbstractImporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object.
     * @param Importer to be modified by the page options.
     * @param Parent widget.
     */
    explicit TestMementoAppendImporterPage(
            GtObject* obj,
            TestMementoAppendImporter* importer,
            QWidget* parent = nullptr);

private:
    /// Label of the suffix line edit
    QLabel* m_label = nullptr;

    /// Line edit for the name suffix
    QLineEdit* m_suffix = nullptr;

    /// Pointer to importer
    QPointer<TestMementoAppendImporter> m_importer;
};

#endif // TEST_MEMENTO_APPEND_IMPORTER_PAGE_H
