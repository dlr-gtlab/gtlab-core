#include "gtest/gtest.h"

#include <QCoreApplication>

#include "gt_testhelper.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    GtTestHelper* helper = GtTestHelper::instance();

    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();

    delete helper;

    return retval;
}
