#include <gtest/gtest.h>

#include "gt_filechooserproperty.h"

TEST(FileChooserProperty, createFileFilter)
{
    // Qt style
    EXPECT_EQ("File1 (*.txt);;File2 (*.bat)", buildDialogFileFilter(
        QStringList() << "File1 (*.txt)" << "File2 (*.bat)").toStdString());

    // old style
    EXPECT_EQ("*.txt; *.bat", buildDialogFileFilter(
        QStringList() << ".txt" << ".bat").toStdString());

    // mixed styles
    EXPECT_EQ("*.txt; *.bat;;Python (python*)", buildDialogFileFilter(
        QStringList() << ".txt" << ".bat" << "Python (python*)").toStdString());

    // mixed styles
    EXPECT_EQ("Python (python*);;*.txt; *.bat", buildDialogFileFilter(
        QStringList() << "Python (python*)" << ".txt" << ".bat").toStdString());

    // mixed styles
    EXPECT_EQ("Python (python*);;*.txt; *.bat;;Executable(*.exe *.bat)",
        buildDialogFileFilter(QStringList()
            << "Python (python*)"
            << ".txt"
            << ".bat"
            << "Executable(*.exe *.bat)").toStdString());

    // mixed styles
    EXPECT_EQ("*.png;;Python (python*);;GTlab (GTlab*);;"
              "*.txt; *.bat;;Executable(*.exe *.bat);;*.jpg",
          buildDialogFileFilter(QStringList()
            << ".png"
            << "Python (python*)"
            << "GTlab (GTlab*)"
            << ".txt"
            << ".bat"
            << "Executable(*.exe *.bat)"
            << ".jpg").toStdString());
}
