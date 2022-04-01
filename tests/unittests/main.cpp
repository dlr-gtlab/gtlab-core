#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QDir>

#include "gt_testhelper.h"
#include "gt_h5filemanager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    GtTestHelper* helper = GtTestHelper::instance();

    // set custom path for hdf5 files
    gtH5FileManager->reset(nullptr, helper->newTempDir().path());

    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();

    delete helper;

    return retval;
}
