/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_versionnumber.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 05.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_versionnumber.h"

/// This is a test fixture that does a init for each test
class TestGtVersionNumber : public ::testing::Test
{
protected:
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {

    }

};

TEST_F(TestGtVersionNumber, toString)
{
    ASSERT_STREQ(GtVersionNumber(1, 0, 0).toString().toStdString().c_str(),
                 "1.0.0");
    ASSERT_STREQ(GtVersionNumber(1, 0, 0, "a").toString().toStdString().c_str(),
                 "1.0.0-a");
    ASSERT_STREQ(GtVersionNumber(1, 0, 0, "a", "b").toString().toStdString().c_str(),
                 "1.0.0-a+b");
    ASSERT_STREQ(GtVersionNumber(1, 0, 0, "", "b").toString().toStdString().c_str(),
                 "1.0.0+b");
    ASSERT_STREQ(GtVersionNumber(1, 0, 0, "", "b-a").toString().toStdString().c_str(),
                 "1.0.0+b-a");
    ASSERT_STREQ(GtVersionNumber(1, 0).toString().toStdString().c_str(),
                 "1.0");
    ASSERT_STREQ(GtVersionNumber(1, 0, "a").toString().toStdString().c_str(),
                 "1.0-a");
    ASSERT_STREQ(GtVersionNumber(1, 0, "a", "b").toString().toStdString().c_str(),
                 "1.0-a+b");
    ASSERT_STREQ(GtVersionNumber(1, 0, "", "b").toString().toStdString().c_str(),
                 "1.0+b");
    ASSERT_STREQ(GtVersionNumber(1, 0, "", "b-a").toString().toStdString().c_str(),
                 "1.0+b-a");
    ASSERT_STREQ(GtVersionNumber(1).toString().toStdString().c_str(),
                 "1");
    ASSERT_STREQ(GtVersionNumber(1, "a").toString().toStdString().c_str(),
                 "1-a");
    ASSERT_STREQ(GtVersionNumber(1, "a", "b").toString().toStdString().c_str(),
                 "1-a+b");
    ASSERT_STREQ(GtVersionNumber(1, "", "b").toString().toStdString().c_str(),
                 "1+b");
    ASSERT_STREQ(GtVersionNumber(1, "", "b-a").toString().toStdString().c_str(),
                 "1+b-a");
    ASSERT_STREQ(GtVersionNumber().toString().toStdString().c_str(),
                 "");

    ASSERT_STREQ(GtVersionNumber("1.0.0").toString().toStdString().c_str(),
                 "1.0.0");
    ASSERT_STREQ(GtVersionNumber("1.0.0-a").toString().toStdString().c_str(),
                 "1.0.0-a");
    ASSERT_STREQ(GtVersionNumber("1.0.0-a+b").toString().toStdString().c_str(),
                 "1.0.0-a+b");
    ASSERT_STREQ(GtVersionNumber("1.0.0+b").toString().toStdString().c_str(),
                 "1.0.0+b");
    ASSERT_STREQ(GtVersionNumber("1.0.0+b-a").toString().toStdString().c_str(),
                 "1.0.0+b-a");
    ASSERT_STREQ(GtVersionNumber("1.0").toString().toStdString().c_str(),
                 "1.0");
    ASSERT_STREQ(GtVersionNumber("1.0-a").toString().toStdString().c_str(),
                 "1.0-a");
    ASSERT_STREQ(GtVersionNumber("1.0-a+b").toString().toStdString().c_str(),
                 "1.0-a+b");
    ASSERT_STREQ(GtVersionNumber("1.0+b").toString().toStdString().c_str(),
                 "1.0+b");
    ASSERT_STREQ(GtVersionNumber("1.0+b-a").toString().toStdString().c_str(),
                 "1.0+b-a");
    ASSERT_STREQ(GtVersionNumber("1").toString().toStdString().c_str(),
                 "1");
    ASSERT_STREQ(GtVersionNumber("1-a").toString().toStdString().c_str(),
                 "1-a");
    ASSERT_STREQ(GtVersionNumber("1-a+b").toString().toStdString().c_str(),
                 "1-a+b");
    ASSERT_STREQ(GtVersionNumber("1+b").toString().toStdString().c_str(),
                 "1+b");
    ASSERT_STREQ(GtVersionNumber("1+b-a").toString().toStdString().c_str(),
                 "1+b-a");
}

TEST_F(TestGtVersionNumber, isNull)
{
    GtVersionNumber v(1, 0, 0);

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber(1, 0, 0, "a");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber();

    ASSERT_TRUE(v.isNull());

    v = GtVersionNumber("a");

    ASSERT_TRUE(v.isNull());

    v = GtVersionNumber("1");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber("1.3.12-a");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber("1.3.12-a+b");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber("1.3.12+b");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber("1.3.12+b-a");

    ASSERT_FALSE(v.isNull());
}

TEST_F(TestGtVersionNumber, compare)
{
    GtVersionNumber v1(1, 0, 0);
    GtVersionNumber v2(1, 0, 0);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), 0);

    v1 = GtVersionNumber(2, 0, 1);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), 1);

    v1 = GtVersionNumber(0, 0, 1);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), -1);
}

TEST_F(TestGtVersionNumber, operators)
{
    GtVersionNumber v1(1, 0, 0);
    GtVersionNumber v2(1, 0, 0);

    ASSERT_TRUE(v1 == v2);
    ASSERT_FALSE(v1 != v2);
    ASSERT_TRUE(v1 >= v2);
    ASSERT_TRUE(v1 <= v2);
    ASSERT_FALSE(v1 > v2);
    ASSERT_FALSE(v1 < v2);

    v1 = GtVersionNumber(2, 0, 0);

    ASSERT_FALSE(v1 == v2);
    ASSERT_TRUE(v1 != v2);
    ASSERT_TRUE(v1 >= v2);
    ASSERT_FALSE(v1 <= v2);
    ASSERT_TRUE(v1 > v2);
    ASSERT_FALSE(v1 < v2);

    v1 = GtVersionNumber(0, 1, 0);

    ASSERT_FALSE(v1 == v2);
    ASSERT_TRUE(v1 != v2);
    ASSERT_FALSE(v1 >= v2);
    ASSERT_TRUE(v1 <= v2);
    ASSERT_FALSE(v1 > v2);
    ASSERT_TRUE(v1 < v2);
}

TEST_F(TestGtVersionNumber, MajorMinorPatch)
{
    const auto version = GtVersionNumber(1, 2, 3);
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinor)
{
    const auto version = GtVersionNumber(1, 2);
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, Major)
{
    const auto version = GtVersionNumber(1);
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchPreRelease)
{
    const auto version = GtVersionNumber(1, 2, 3, "beta");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("beta", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchBuild)
{
    const auto version = GtVersionNumber(1, 2, 3, "", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchPreReleaseBuild)
{
    const auto version = GtVersionNumber(1, 2, 3, "alpha", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorBuild)
{
    const auto version = GtVersionNumber(1, 2, "", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPreRelease)
{
    const auto version = GtVersionNumber(1, 2, "alpha");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPreReleaseBuild)
{
    const auto version = GtVersionNumber(1, 2, "alpha", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorBuild)
{
    const auto version = GtVersionNumber(1, "", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorPreRelease)
{
    const auto version = GtVersionNumber(1, "alpha");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorPreReleaseBuild)
{
    const auto version = GtVersionNumber(1, "alpha", "4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}







TEST_F(TestGtVersionNumber, MajorMinorPatchStr)
{
    const auto version = GtVersionNumber("1.2.3");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorStr)
{
    const auto version = GtVersionNumber("1.2");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorStr)
{
    const auto version = GtVersionNumber("1");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchPreReleaseStr)
{
    const auto version = GtVersionNumber("1.2.3-beta");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("beta", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchBuildStr)
{
    const auto version = GtVersionNumber("1.2.3+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchPreReleaseBuildStr)
{
    const auto version = GtVersionNumber("1.2.3-alpha+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorBuildStr)
{
    const auto version = GtVersionNumber("1.2+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPreReleaseStr)
{
    const auto version = GtVersionNumber("1.2-alpha");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPreReleaseBuildStr)
{
    const auto version = GtVersionNumber("1.2-alpha+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorBuildStr)
{
    const auto version = GtVersionNumber("1+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorPreReleaseStr)
{
    const auto version = GtVersionNumber("1-alpha");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorPreReleaseBuildStr)
{
    const auto version = GtVersionNumber("1-alpha+4");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(0, version.minor());
    EXPECT_EQ(0, version.patch());
    EXPECT_STREQ("alpha", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("4", version.build().toStdString().c_str());
}

TEST_F(TestGtVersionNumber, MajorMinorPatchLabelBuild_Complicated)
{
    const auto version = GtVersionNumber("1.2.3----RC-SNAPSHOT.12.9.1--.12+788");
    EXPECT_EQ(1, version.major());
    EXPECT_EQ(2, version.minor());
    EXPECT_EQ(3, version.patch());
    EXPECT_STREQ("---RC-SNAPSHOT.12.9.1--.12", version.preRelease().toStdString().c_str());
    EXPECT_STREQ("788", version.build().toStdString().c_str());
}

/**
 * Tests valid semver version strings
 *
 * From: https://regex101.com/r/vkijKf/1/
 *
 * Modified to allow omitting patch level
 */
TEST_F(TestGtVersionNumber, ValidFormat)
{
    ASSERT_FALSE(GtVersionNumber("0.0.4").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3").isNull());
    ASSERT_FALSE(GtVersionNumber("10.20.30").isNull());
    ASSERT_FALSE(GtVersionNumber("1.1.2-prerelease+meta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.1.2+meta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.1.2+meta-valid").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-beta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha.beta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha.beta.1").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha.1").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha0.valid").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha.0valid").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha-a.b-c-somethinglong+build.1-aef.1-its-okay").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-rc.1+build.1").isNull());
    ASSERT_FALSE(GtVersionNumber("2.0.0-rc.1+build.123").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3-beta").isNull());
    ASSERT_FALSE(GtVersionNumber("10.2.3-DEV-SNAPSHOT").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3-SNAPSHOT-123").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0").isNull());
    ASSERT_FALSE(GtVersionNumber("2.0.0").isNull());
    ASSERT_FALSE(GtVersionNumber("1.1.7").isNull());
    ASSERT_FALSE(GtVersionNumber("2.0.0+build.1848").isNull());
    ASSERT_FALSE(GtVersionNumber("2.0.1-alpha.1227").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-alpha+beta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3----RC-SNAPSHOT.12.9.1--.12+788").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3----R-S.12.9.1--.12+meta").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2.3----RC-SNAPSHOT.12.9.1--.12").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0+0.build.1-rc.10000aaa-kk-0.1").isNull());
    ASSERT_FALSE(GtVersionNumber("99999999999999999999999.999999999999999999.99999999999999999").isNull());
    ASSERT_FALSE(GtVersionNumber("1.0.0-0A.is.legal").isNull());

    // These are modifications from the actual semver 2.0.0 definition that allow omitting the patch version
    ASSERT_FALSE(GtVersionNumber("1.2").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2-SNAPSHOT").isNull());
    ASSERT_FALSE(GtVersionNumber("1.2-RC-SNAPSHOT").isNull());
}

/**
 * Tests invalid semver version strings
 *
 * From: https://regex101.com/r/vkijKf/1/
 *
 * Modified to allow omitting patch level
 */
TEST_F(TestGtVersionNumber, InvalidFormat)
{
    ASSERT_TRUE(GtVersionNumber("1.2.3-0123").isNull());
    ASSERT_TRUE(GtVersionNumber("1.2.3-0123.0123").isNull());
    ASSERT_TRUE(GtVersionNumber("1.1.2+.123").isNull());
    ASSERT_TRUE(GtVersionNumber("+invalid").isNull());
    ASSERT_TRUE(GtVersionNumber("-invalid").isNull());
    ASSERT_TRUE(GtVersionNumber("-invalid+invalid").isNull());
    ASSERT_TRUE(GtVersionNumber("-invalid.01").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha.beta").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha.beta.1").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha.1").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha+beta").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha_beta").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha.").isNull());
    ASSERT_TRUE(GtVersionNumber("alpha..").isNull());
    ASSERT_TRUE(GtVersionNumber("beta").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha_beta").isNull());
    ASSERT_TRUE(GtVersionNumber("-alpha.").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha..").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha..1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha...1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha....1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha.....1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha......1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.0.0-alpha.......1").isNull());
    ASSERT_TRUE(GtVersionNumber("01.1.1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.01.1").isNull());
    ASSERT_TRUE(GtVersionNumber("1.1.01").isNull());
    ASSERT_TRUE(GtVersionNumber("1.2.3.DEV").isNull());
    ASSERT_TRUE(GtVersionNumber("1.2.31.2.3----RC-SNAPSHOT.12.09.1--..12+788").isNull());
    ASSERT_TRUE(GtVersionNumber("-1.0.3-gamma+b7718").isNull());
    ASSERT_TRUE(GtVersionNumber("+justmeta").isNull());
    ASSERT_TRUE(GtVersionNumber("9.8.7+meta+meta").isNull());
    ASSERT_TRUE(GtVersionNumber("9.8.7-whatever+meta+meta").isNull());
    ASSERT_TRUE(GtVersionNumber("99999999999999999999999.999999999999999999.99999999999999999----RC-SNAPSHOT.12.09.1--------------------------------..12").isNull());
}

TEST_F(TestGtVersionNumber, CompareExtended)
{
    EXPECT_TRUE(GtVersionNumber("1.2.3") < GtVersionNumber("2.2.3"));
    EXPECT_TRUE(GtVersionNumber("2.2.3") > GtVersionNumber("1.2.3"));
    EXPECT_TRUE(GtVersionNumber("1.2.3") == GtVersionNumber("1.2.3"));

    EXPECT_FALSE(GtVersionNumber("1.2.3") > GtVersionNumber("2.2.3"));
    EXPECT_FALSE(GtVersionNumber("2.2.3") < GtVersionNumber("1.2.3"));

    // compare minor level
    EXPECT_TRUE(GtVersionNumber("1.2.3") < GtVersionNumber("1.3.0"));
    EXPECT_FALSE(GtVersionNumber("1.2.3") > GtVersionNumber("1.3.0"));

    // compare patch level
    EXPECT_TRUE(GtVersionNumber("1.2.3") < GtVersionNumber("1.2.4"));
    EXPECT_FALSE(GtVersionNumber("1.2.3") > GtVersionNumber("1.2.4"));

    EXPECT_TRUE(GtVersionNumber("1.2") < GtVersionNumber("1.2.3"));
    EXPECT_FALSE(GtVersionNumber("1.2") > GtVersionNumber("1.2.3"));

    // From https://semver.org/#spec-item-11:
    // 1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta < 1.0.0-beta < 1.0.0-beta.2 < 1.0.0-beta.11 < 1.0.0-rc.1 < 1.0.0
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha") < GtVersionNumber("1.0.0-alpha.1"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha.1") < GtVersionNumber("1.0.0-alpha.beta"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha.beta") < GtVersionNumber("1.0.0-beta"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-beta") < GtVersionNumber("1.0.0-beta.2"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-beta.2") < GtVersionNumber("1.0.0-beta.11"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-beta.11") < GtVersionNumber("1.0.0-rc.1"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-rc1") < GtVersionNumber("1.0.0"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-dp1") < GtVersionNumber("1.0.0"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-dp1") < GtVersionNumber("1.0.0-alpha"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-dp1") < GtVersionNumber("1.0.0-dp3"));
    EXPECT_TRUE(GtVersionNumber("1.0.0") > GtVersionNumber("1.0.0-dp1"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha") > GtVersionNumber("1.0.0-dp1"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-dp3") > GtVersionNumber("1.0.0-dp1"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-0.dp1") < GtVersionNumber("1.0.0-0.alpha"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-0.DP1") < GtVersionNumber("1.0.0-0.alpha"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha") > GtVersionNumber("1.0.0-ALPHA"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-alpha") > GtVersionNumber("1.0.0-ALPHA"));
    EXPECT_TRUE(GtVersionNumber("1.0.0-0.DP1") < GtVersionNumber("1.0.0-0.dp1"));

    EXPECT_FALSE(GtVersionNumber("1.0.0-alpha") > GtVersionNumber("1.0.0-alpha.1"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-alpha.1") > GtVersionNumber("1.0.0-alpha.beta"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-alpha.beta") > GtVersionNumber("1.0.0-beta"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-beta") > GtVersionNumber("1.0.0-beta.2"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-beta.2") > GtVersionNumber("1.0.0-beta.11"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-beta.11") > GtVersionNumber("1.0.0-rc.1"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-rc1") > GtVersionNumber("1.0.0"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-dp1") > GtVersionNumber("1.0.0"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-dp1") > GtVersionNumber("1.0.0-alpha"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-dp1") > GtVersionNumber("1.0.0-dp3"));
    EXPECT_FALSE(GtVersionNumber("1.0.0") < GtVersionNumber("1.0.0-dp1"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-alpha") < GtVersionNumber("1.0.0-dp1"));
    EXPECT_FALSE(GtVersionNumber("1.0.0-dp3") < GtVersionNumber("1.0.0-dp1"));

    // Build Versions must be ignored
    EXPECT_TRUE(GtVersionNumber("1.2.3+1") == GtVersionNumber("1.2.3+2"));
    EXPECT_TRUE(GtVersionNumber("1.2.3-alpha+1") == GtVersionNumber("1.2.3-alpha+2"));
    EXPECT_TRUE(GtVersionNumber("1.2.3-alpha+2") < GtVersionNumber("1.2.3-beta+1"));

    EXPECT_TRUE(GtVersionNumber("2.0.0-0") < GtVersionNumber("2.0.0-alpha"));
}
