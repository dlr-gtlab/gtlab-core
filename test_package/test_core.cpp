/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_filesystem.h"
#include "gt_coreapplication.h"
#include "gt_versionnumber.h"

#include <iostream>

int main()
{
    std::cout << "RUNNING CORE TESTS\n\n";

	std::cout << "This is GTlab-Core " << gtApp->version().toString().toStdString() << "\n\n";

    auto files = gt::filesystem::directoryEntries(QDir("."), false);

    std::cout << "result of gt::filesystem::directoryEntries():\n";
    for (auto file : files)
    {
        std::cout << "  " << file.toStdString() << "\n";
    }

    return 0;
}
