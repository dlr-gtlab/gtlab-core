/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 23.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifdef GT_H5

#include "gtest/gtest.h"

#include "gt_h5externalizehelper.h"
#include "gt_externalizationmanager.h"

#include "test_externalizedobject.h"

/// This is a test fixture that does a init for each test
struct TestGtH5ExternalizeHelper : public ::testing::Test
{
    void SetUp() override
    {
        gtExternalizationManager->enableExternalization(true);

        obj = std::make_unique<TestExternalizedObject>();
        obj->setObjectName("TestObject");
    }

    std::unique_ptr<TestExternalizedObject> obj{};
};

TEST_F(TestGtH5ExternalizeHelper, openFile)
{
    GtH5ExternalizeHelper helper{*obj};

    try
    {
        auto file = helper.openFile({GenH5::Create | GenH5::Open});
        EXPECT_FALSE(file.filePath().isEmpty());
    }
    catch (GenH5::Exception const& e)
    {
        gtError() << "EXPECTION:" << e.what();
        ASSERT_TRUE(false);
    }
}

TEST_F(TestGtH5ExternalizeHelper, overwriteDataSet_issue_453)
{
    GtH5ExternalizeHelper helper{*obj};

    try
    {
        GenH5::DataType dtype = GenH5::dataType<int>();
        GenH5::DataSpace dspace{4, 5};

        QVariant ref;

        // make sure file is empty
        {
            auto file = helper.openFile({GenH5::Create | GenH5::Open});
            EXPECT_TRUE(file.root().findChildNodes(GenH5::FindRecursive).empty());
        }

        // dset will be created
        {
            auto dset = helper.overwriteDataSet(dtype, dspace, ref);

            EXPECT_TRUE(dset.dataType() == dtype);
            EXPECT_TRUE(dset.dataSpace() == dspace);
        }

        // dset must be overwritten due to changed datatype
        {
            dtype = GenH5::dataType<uint>();
            auto dset = helper.overwriteDataSet(dtype, dspace, ref);

            EXPECT_TRUE(dset.dataType() == dtype);
            EXPECT_TRUE(dset.dataSpace() == dspace);
        }

        // dset must be overwritten due to changed dataspace
        {
            dspace = GenH5::DataSpace::linear(42);
            auto dset = helper.overwriteDataSet(dtype, dspace, ref);

            EXPECT_TRUE(dset.dataType() == dtype);
            EXPECT_TRUE(dset.dataSpace() == dspace);
        }

        // file should have a single dataset named like our object
        {
            auto file = helper.openFile({GenH5::Open});
            auto dsets = file.root().findChildNodes(GenH5::FindRecursive,
                                                    GenH5::FilterDataSets);
            ASSERT_EQ(dsets.size(), 1);
            EXPECT_TRUE(dsets[0].path.contains(obj->uuid().toUtf8()));
        }
    }
    catch (GenH5::Exception const& e)
    {
        gtError() << "EXPECTION:" << e.what();
        ASSERT_NO_THROW(throw e);
    }
}

#endif
