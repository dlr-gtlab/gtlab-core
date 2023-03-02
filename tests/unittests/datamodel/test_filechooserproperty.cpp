#include <gtest/gtest.h>

#include "gt_filechooserproperty.h"

TEST(FileChooserProperty, createFileFilter)
{
    using gt::detail::buildFileDialogFileFilter;

    // Qt style
    EXPECT_EQ("File1 (*.txt);;File2 (*.bat)", buildFileDialogFileFilter(
        QStringList() << "File1 (*.txt)" << "File2 (*.bat)").toStdString());

    // old style
    EXPECT_EQ("*.txt; *.bat", buildFileDialogFileFilter(
        QStringList() << ".txt" << ".bat").toStdString());

    // mixed styles
    EXPECT_EQ("*.txt; *.bat;;Python (python*)", buildFileDialogFileFilter(
        QStringList() << ".txt" << ".bat" << "Python (python*)").toStdString());

    // mixed styles
    EXPECT_EQ("Python (python*);;*.txt; *.bat", buildFileDialogFileFilter(
        QStringList() << "Python (python*)" << ".txt" << ".bat").toStdString());

    // mixed styles
    EXPECT_EQ("Python (python*);;*.txt; *.bat;;Executable(*.exe *.bat)",
        buildFileDialogFileFilter(QStringList()
            << "Python (python*)"
            << ".txt"
            << ".bat"
            << "Executable(*.exe *.bat)").toStdString());

    // mixed styles
    EXPECT_EQ("*.png;;Python (python*);;GTlab (GTlab*);;"
              "*.txt; *.bat;;Executable(*.exe *.bat);;*.jpg",
          buildFileDialogFileFilter(QStringList()
            << ".png"
            << "Python (python*)"
            << "GTlab (GTlab*)"
            << ".txt"
            << ".bat"
            << "Executable(*.exe *.bat)"
            << ".jpg").toStdString());
}
