/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_commandlinefunction
 * copyright 2009-2022 by DLR
 *
 *  Created on: 16.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include <QStringList>

#include "gt_changelogparser.h"
#include "gt_versionnumber.h"

/// This is a test fixture that does a init for each test
class TestGtChangeLogParser : public ::testing::Test
{

protected:
    QString testOnlyUnreleased{
        "## [Unreleased]"
        "### Fixed"
        "- Initial structure to avoid loading of GUI elements in console mode - #2"
        "- Exporter: Setting an invalid or empty file extension will no longer crash GTlab - #130"
        "- Fixed a crash when zooming in on a graphics scene containing a dense grid - #179"
        "- Fixed bug in dynamic type registration in meta object system - #248"
    };

    QString testValid{
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

    QString emptyTest{""};

};

TEST_F(TestGtChangeLogParser, empty)
{
    GtChangeLogParser p(emptyTest);

    ASSERT_TRUE(p.currentReleaseMD().content().isEmpty());
    ASSERT_TRUE(p.allReleasesDM().isEmpty());
    ASSERT_TRUE(p.versions().isEmpty());
}

TEST_F(TestGtChangeLogParser, invalid)
{
    GtChangeLogParser p(testOnlyUnreleased);

    ASSERT_TRUE(p.currentReleaseMD().content().isEmpty());
    ASSERT_TRUE(p.allReleasesDM().isEmpty());
    ASSERT_TRUE(p.versions().isEmpty());
}

TEST_F(TestGtChangeLogParser, testValid)
{
    GtChangeLogParser p(testValid);

    QList<GtChangeLogEntry> entries = p.allReleasesDM();
    GtChangeLogEntry current = p.currentReleaseMD();

    ASSERT_FALSE(current.content().isEmpty());
    ASSERT_FALSE(entries.isEmpty());

    QList<GtVersionNumber> v = p.versions();

    ASSERT_EQ(v.size(), 2);

    GtVersionNumber compare1 (1, 7, 2);
    GtVersionNumber compare2 (1, 7, 1);

    ASSERT_TRUE(current.version() == compare1);
    ASSERT_TRUE(v.at(1) == compare2);

    QDate date1 (2021, 5, 17);
    QDate date2 (2021, 5, 25);

    ASSERT_TRUE(entries.at(0).date() == date1);
    ASSERT_TRUE(current.date() == date2);

    QString compareText{
        //"## [1.7.2] - 2021-05-25"
        "### Changed"
        "- Update check functionality uses maintenance tool command line interface instead of parsing log files - #121"
        "- Bug report url changed and enabled for non dev mode - #122"
        ""
        "### Added"
        "- Functionality to export framework footprint information  - #124"
        "- Functionality to display framework footprint information in command line mode - #125"
        ""
    };

    QString compareText2{
        //## [1.7.1] - 2021-05-17"
        "### Fixed"
        "- RPATH fix for all core libraries"
    };

    ASSERT_EQ(compareText2, entries.at(0).content());
    ASSERT_EQ(compareText, current.content());

}
