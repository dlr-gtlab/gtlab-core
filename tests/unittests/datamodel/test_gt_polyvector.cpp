/* GTlab - Gas Turbine laboratory
 * Source File: gt_structproperty.h
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */


#include <gt_polyvector.h>

#include "gtest/gtest.h"

struct TestPolyVector : public testing::Test
{
};

struct Base // is copyable
{
    Base() = default;
    virtual ~Base() = default;

    virtual std::string clz() const
    {
        return "Base";
    }
};

struct DerivedCopyable : public Base
{
    explicit DerivedCopyable(std::string name = "DerivedCopyable")
        : m_name(name)
    {}

    std::string clz() const override
    {
        return m_name;
    }

    std::string m_name;
};

struct DerivedNotCopyable : public Base
{
    explicit DerivedNotCopyable(std::string name = "DerivedNotCopyable")
        : m_name(name)
    {}

    DerivedNotCopyable(const DerivedNotCopyable&) = delete;

    std::string clz() const override
    {
        return m_name;
    }

    std::string m_name;
};

TEST_F(TestPolyVector, emplace)
{
    gt::PolyVector<Base> vec;

    vec.emplace_back<Base>();
    vec.emplace_back<DerivedCopyable>();
    vec.emplace_back<DerivedCopyable>("AnotherName");
    vec.emplace_back<DerivedNotCopyable>();

    ASSERT_EQ(4u, vec.size());

    EXPECT_EQ("Base", vec[0].clz());
    EXPECT_EQ("DerivedCopyable", vec[1].clz());
    EXPECT_EQ("AnotherName", vec[2].clz());
    EXPECT_EQ("DerivedNotCopyable", vec[3].clz());
}

TEST_F(TestPolyVector, push_back)
{
    gt::PolyVector<Base> vec;

    vec.push_back(std::make_unique<Base>());
    vec.push_back(std::make_unique<DerivedCopyable>());
    vec.push_back(std::make_unique<DerivedCopyable>("AnotherName"));
    vec.push_back(std::make_unique<DerivedNotCopyable>());

    DerivedCopyable d("YetAnotherName");
    vec.push_back(d);

    ASSERT_EQ(5u, vec.size());

    EXPECT_EQ("Base", vec[0].clz());
    EXPECT_EQ("DerivedCopyable", vec[1].clz());
    EXPECT_EQ("AnotherName", vec[2].clz());
    EXPECT_EQ("DerivedNotCopyable", vec[3].clz());
    EXPECT_EQ("YetAnotherName", vec[4].clz());

    /* These lines must not compile.
     * I don't know, if there's a way to test this, except from uncommenting
     * this
     */
    //DerivedNotCopyable nc;
    //vec.push_back(nc);
}
