/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <gtest/gtest.h>

#include "gt_regexp.h"


TEST(RegExp, fileFilter)
{
    auto re = gt::re::forFileDialogFilters();

    auto match = [](const auto& re, const auto& string) {
        return re.indexIn(string) >= 0;
    };

    // check for correct qt dialog file filter
    EXPECT_TRUE(match(re, "Python (python* python3)"));
    EXPECT_TRUE(match(re, " Python (python* python3) "));
    EXPECT_TRUE(match(re, "Text-File (*.txt)"));
    EXPECT_TRUE(match(re, "Document (*.txt *.text *.md *.doc)"));
    EXPECT_TRUE(match(re, "Document  (*.txt *.text *.md *.doc)"));

    // check for other, that dont match
    EXPECT_FALSE(match(re, ".txt .bat"));
    EXPECT_FALSE(match(re, "*.exe"));
    EXPECT_FALSE(match(re, "Document  (*.txt *.text *.md *.doc"));
}

TEST(RegExp, restrictRegExpWithObjectNames)
{
    auto re = gt::re::onlyLetters();

    QStringList list{"Hello", "World"};

    gt::re::restrictRegExpWithObjectNames(list, re);

    auto match = [](const auto& re, const auto& string) {
        return re.indexIn(string) >= 0;
    };

    EXPECT_TRUE(match(re, "Python"));
    EXPECT_TRUE(match(re, "HiEverybody"));
    EXPECT_TRUE(match(re, "NiceToSeeYou"));
    EXPECT_FALSE(match(re, "Hello"));
    EXPECT_FALSE(match(re, "World"));
}

TEST(RegExp, restrictRegExpWithObjectSiblingsNames)
{
    auto re = gt::re::onlyLetters();

    auto* parent = new GtObject;
    auto* c1 = new GtObject;
    c1->setObjectName("Hello");
    parent->appendChild(c1);

    auto* c2 = new GtObject;
    c2->setObjectName("World");
    parent->appendChild(c2);

    auto* c3 = new GtObject;
    c3->setObjectName("Foo");
    parent->appendChild(c3);

    gt::re::restrictRegExpWithObjectSiblingsNames<GtObject>(*c3, re);

    auto match = [](const auto& re, const auto& string) {
        return re.indexIn(string) >= 0;
    };

    /// Other names should still be possible
    EXPECT_TRUE(match(re, "Python"));
    EXPECT_TRUE(match(re, "HiEverybody"));

    /// The existing name should still be possible
    EXPECT_TRUE(match(re, "Foo"));

    /// The name of the siblings should not be allowed
    EXPECT_FALSE(match(re, "Hello"));
    EXPECT_FALSE(match(re, "World"));

    delete parent;
}

