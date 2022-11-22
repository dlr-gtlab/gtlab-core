#include "gt_exceptions.h"


#include <gtest/gtest.h>

void myThrowingFunc()
{
    throw GTlabException("myThrowingFunc", "This is an error");
}

TEST(GtException, catchStdException)
{
    ASSERT_THROW(myThrowingFunc(), std::exception);
}


TEST(GtException, checkWhat)
{
    bool hasThrown = false;
    try
    {
        myThrowingFunc();
    }
    catch(std::exception& e)
    {
        hasThrown = true;
        EXPECT_STREQ("This is an error", e.what());
    }

    ASSERT_TRUE(hasThrown);
}
