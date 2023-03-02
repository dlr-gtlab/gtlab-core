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

