#include <gtest/gtest.h>

#include "gt_regexp.h"


TEST(RegExp, fileFilter)
{
    auto re = gt::re::forDialogFileFilters();

    // check for correct qt dialog file filter
    EXPECT_EQ(0, re.indexIn("Python (python* python3)"));
    EXPECT_EQ(1, re.indexIn(" Python (python* python3) "));
    EXPECT_EQ(0, re.indexIn("Text-File (*.txt)"));
    EXPECT_EQ(0, re.indexIn("Document (*.txt *.text *.md *.doc)"));
    EXPECT_EQ(0, re.indexIn("Document  (*.txt *.text *.md *.doc)"));

    // check for other, that dont match
    EXPECT_EQ(-1, re.indexIn(".txt .bat"));
    EXPECT_EQ(-1, re.indexIn("*.exe"));
    EXPECT_EQ(-1, re.indexIn("Document  (*.txt *.text *.md *.doc"));
}

