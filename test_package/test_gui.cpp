/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_changelogparser.h"
#include <QString>

#include <iostream>

int main()
{
    std::cout << "\n\nRUNNING GUI TESTS\n\n";

    QString testValid {
        "## [1.7.2] - 2021-05-25"
        "### Changed"
        "- Update check functionality uses maintenance tool command line interface instead of parsing log files - #121"
        "- Bug report url changed and enabled for non dev mode - #122"
        ""
        "### Added"
        "- Functionality to export framework footprint information  - #124"
        "- Functionality to display framework footprint information in command line mode - #125"
        ""
        "## [1.7.1] - 2021-05-17"
        "### Fixed"
        "- RPATH fix for all core libraries"
    };

    GtChangeLogParser p(testValid);

    auto releases = p.allReleasesDM();
    for (const auto& release : releases)
    {
        std::cout << "  Version: " << release.version().toString().toStdString() << "\n";
    }

    return 0;
}
