#include <gtest/gtest.h>

#include "gt_regexp.h"


TEST(RegExp, fileFilter)
{
    auto re = gt::re::forFileDialogFilters();

    // check for correct qt dialog file filter
    EXPECT_TRUE(re.match("Python (python* python3)").hasMatch());
    EXPECT_TRUE(re.match(" Python (python* python3) ").hasMatch());
    EXPECT_TRUE(re.match("Text-File (*.txt)").hasMatch());
    EXPECT_TRUE(re.match("Document (*.txt *.text *.md *.doc)").hasMatch());
    EXPECT_TRUE(re.match("Document  (*.txt *.text *.md *.doc)").hasMatch());

    // check for other, that dont match
    EXPECT_FALSE(re.match(".txt .bat").hasMatch());
    EXPECT_FALSE(re.match("*.exe").hasMatch());
    EXPECT_FALSE(re.match("Document  (*.txt *.text *.md *.doc").hasMatch());
}

