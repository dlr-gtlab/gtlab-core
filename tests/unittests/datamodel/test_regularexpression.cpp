/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#include <gt_regularexpression.h>

#include <gtest/gtest.h>

TEST(RegularExpression, woUmlauts)
{
    auto& exp = gt::rex::woUmlauts();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "hello"));
    EXPECT_FALSE(gt::rex::exactMatch(exp, "hällo"));
}


TEST(RegularExpression, umlauts)
{
    auto& umExp = gt::rex::umlauts();

    EXPECT_TRUE(umExp.match("öäUä").hasMatch());
    EXPECT_FALSE(umExp.match("hallo").hasMatch());
}

TEST(RegularExpression, umlautsAndSpecialChar)
{
    auto& exp = gt::rex::umlautsAndSpecialChar();

    EXPECT_TRUE(exp.match("öäUä").hasMatch());
    EXPECT_TRUE(exp.match("-/?").hasMatch());
    EXPECT_TRUE(exp.match("hallo?").hasMatch());

    EXPECT_FALSE(exp.match("hallo").hasMatch());
}


TEST(RegularExpression, onlyLetters)
{
    auto& exp = gt::rex::onlyLetters();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "hello"));

    EXPECT_FALSE(gt::rex::exactMatch(exp, "he__o"));
    EXPECT_FALSE(gt::rex::exactMatch(exp, "he12o"));
}

TEST(RegularExpression, onlyLettersAndNumbers)
{
    auto& exp = gt::rex::onlyLettersAndNumbers();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "hello123"));
    EXPECT_FALSE(gt::rex::exactMatch(exp, "he??o"));
}

TEST(RegularExpression, onlyLettersAndNumbersAndDot)
{
    auto& exp = gt::rex::onlyLettersAndNumbersAndDot();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "hello123."));
    EXPECT_FALSE(gt::rex::exactMatch(exp, "he??o"));
}

TEST(RegularExpression, onlyLettersAndNumbersAndSpace)
{
    auto& exp = gt::rex::onlyLettersAndNumbersAndSpace();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "hello 123"));
    EXPECT_FALSE(gt::rex::exactMatch(exp, "he1 ??o"));
}

TEST(RegularExpression, forUnits)
{
    auto& exp = gt::rex::forUnits();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "m"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "km"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "miles"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "°C"));
}

TEST(RegularExpression, forExpressions)
{
    auto& exp = gt::rex::forExpressions();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "1+2.3"));
}

TEST(RegularExpression, forStations)
{
    auto& exp = gt::rex::forStations();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "S1"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "S12"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "S123"));

    EXPECT_FALSE(gt::rex::exactMatch(exp, "S1234"));
}

TEST(RegularExpression, forDoubles)
{
    auto& exp = gt::rex::forDoubles();

    EXPECT_TRUE(gt::rex::exactMatch(exp, "1"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1.2"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1.2e-10"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1.2E10"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "3.14e+23"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, ".123"));

    EXPECT_FALSE(gt::rex::exactMatch(exp, "1.2.3"));
}

TEST(RegularExpression, forDoublesLists)
{
    auto& exp = gt::rex::forDoublesLists();

    // Valid cases
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1.2"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "-3.14"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "3.14e+10"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "-2.71e-3"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, ".123"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1;2;3.14"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "1.2;3.4e-5;-10.0"));
    EXPECT_TRUE(gt::rex::exactMatch(exp, "0.0;1.0;2.0;3.0"));

    // Invalid cases
    EXPECT_FALSE(gt::rex::exactMatch(exp, "1.2;"));          // trailing ;
    EXPECT_FALSE(gt::rex::exactMatch(exp, ";1.2"));          // leading ;
    EXPECT_FALSE(gt::rex::exactMatch(exp, "1.2;;3.4"));      // double ;;
    EXPECT_FALSE(gt::rex::exactMatch(exp, "1.2;abc"));       // non-numeric
    EXPECT_FALSE(gt::rex::exactMatch(exp, "1.2;3.4.5"));     // invalid number
    EXPECT_FALSE(gt::rex::exactMatch(exp, ""));              // empty
}

TEST(RegularExpression, forSemVers)
{
    const auto& re = gt::rex::forSemVers();

    {
        QRegularExpressionMatch match = re.match("2.1.3-alpha+4");
        ASSERT_TRUE(match.hasMatch());

        EXPECT_EQ(match.captured(1), "2");        // Major
        EXPECT_EQ(match.captured(3), "1");        // Minor
        EXPECT_EQ(match.captured(5), "3");        // Patch
        EXPECT_EQ(match.captured(6), "alpha");    // Prerelease
        EXPECT_EQ(match.captured(7), "4");        // Build
    }

    {
        QRegularExpressionMatch match = re.match("1.0.0");
        ASSERT_TRUE(match.hasMatch());

        EXPECT_EQ(match.captured(1), "1");        // Major
        EXPECT_EQ(match.captured(3), "0");        // Minor
        EXPECT_EQ(match.captured(5), "0");        // Patch
        EXPECT_TRUE(match.captured(6).isEmpty()); // No Prerelease
        EXPECT_TRUE(match.captured(7).isEmpty()); // No Build
    }

    {
        QRegularExpressionMatch match = re.match("3.7.2-beta");
        ASSERT_TRUE(match.hasMatch());

        EXPECT_EQ(match.captured(1), "3");        // Major
        EXPECT_EQ(match.captured(3), "7");        // Minor
        EXPECT_EQ(match.captured(5), "2");        // Patch
        EXPECT_EQ(match.captured(6), "beta");     // Prerelease
        EXPECT_TRUE(match.captured(7).isEmpty()); // No Build
    }

    {
        QRegularExpressionMatch match = re.match("0.9.1+build42");
        ASSERT_TRUE(match.hasMatch());

        EXPECT_EQ(match.captured(1), "0");        // Major
        EXPECT_EQ(match.captured(3), "9");        // Minor
        EXPECT_EQ(match.captured(5), "1");        // Patch
        EXPECT_TRUE(match.captured(6).isEmpty()); // No Prerelease
        EXPECT_EQ(match.captured(7), "build42");  // Build
    }

    {
        QRegularExpressionMatch match = re.match("2.0.0-alpha.beta+exp.sha.5114f85");
        ASSERT_TRUE(match.hasMatch());

        EXPECT_EQ(match.captured(1), "2");            // Major
        EXPECT_EQ(match.captured(3), "0");            // Minor
        EXPECT_EQ(match.captured(5), "0");            // Patch
        EXPECT_EQ(match.captured(6), "alpha.beta");   // Prerelease
        EXPECT_EQ(match.captured(7), "exp.sha.5114f85"); // Build
    }

    {
        // Invalid string (missing minor and patch)
        EXPECT_TRUE(gt::rex::exactMatch(re, "1"));
    }
}

TEST(RegularExpression, forFileDialogFilters)
{
    const auto& re = gt::rex::forFileDialogFilters();

    // Valid cases
    EXPECT_TRUE(gt::rex::exactMatch(re, "Images (*.png *.jpg *.bmp)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Text_Files (*.txt *.md)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Any-Files (*.*)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Documents (*.doc *.docx *.pdf)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Compressed_Archives (*.zip *.rar *.7z)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Data_Files (*.csv *.tsv)"));
    EXPECT_TRUE(gt::rex::exactMatch(re, "Code_Files (*.cpp *.h *.hpp *.cxx)"));

    // Invalid cases

    // Missing (pattern)
    EXPECT_FALSE(gt::rex::exactMatch(re, "JustAName"));

    // Wrong ( moved
    EXPECT_FALSE(gt::rex::exactMatch(re, "Images *(.png .jpg .bmp)"));

    // Missing closing )
    EXPECT_FALSE(gt::rex::exactMatch(re, "Images (.png .jpg .bmp"));

    // Missing filter name
    EXPECT_FALSE(gt::rex::exactMatch(re, "(*.png *.jpg *.bmp)"));

    // Space after (, before ) - depending if you want strict
    EXPECT_FALSE(gt::rex::exactMatch(re, "Images ( *.png *.jpg *.bmp )"));

    // Slightly weird, but technically acceptable
    EXPECT_TRUE(gt::rex::exactMatch(re, "Data (*.dat)"));
}

TEST(RegularExpression, forHexColorCode)
{
    const auto& re = gt::rex::forHexColorCode();

    // Valid cases
    {
        QRegularExpressionMatch match = re.match("#123");
        ASSERT_TRUE(match.hasMatch());
        EXPECT_EQ(match.captured(1), "123");
    }

    {
        QRegularExpressionMatch match = re.match("#1a2b");
        ASSERT_TRUE(match.hasMatch());
        EXPECT_EQ(match.captured(1), "1a2b");
    }

    {
        QRegularExpressionMatch match = re.match("#abcdef");
        ASSERT_TRUE(match.hasMatch());
        EXPECT_EQ(match.captured(1), "abcdef");
    }

    {
        QRegularExpressionMatch match = re.match("#ABCDEF12");
        ASSERT_TRUE(match.hasMatch());
        EXPECT_EQ(match.captured(1), "ABCDEF12");
    }

    {
        QRegularExpressionMatch match = re.match("#A1B2");
        ASSERT_TRUE(match.hasMatch());
        EXPECT_EQ(match.captured(1), "A1B2");
    }

    // Invalid cases
    {
        EXPECT_FALSE(re.match("123456").hasMatch());    // Missing '#'
        EXPECT_FALSE(re.match("#12").hasMatch());       // Too short
        EXPECT_FALSE(re.match("#12345").hasMatch());    // 5 digits invalid
        EXPECT_FALSE(re.match("#1234567").hasMatch());  // 7 digits invalid
        EXPECT_FALSE(re.match("#ggg").hasMatch());      // Invalid characters
        EXPECT_FALSE(re.match("#123456789").hasMatch());// Too many digits
        EXPECT_FALSE(re.match("##123456").hasMatch());  // Double #
        EXPECT_FALSE(re.match("#12G").hasMatch());      // Non-hex letter
    }
}


