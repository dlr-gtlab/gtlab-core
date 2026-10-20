/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include <QMutex>
#include <QByteArray>
#include <QString>
#include <QObject>

#include "gtest/gtest.h"

#include "datamodel/test_gt_object.h"
#include "gt_object.h"
#include "gt_utilities.h"
#include "gt_qtutilities.h"

class TestGtUtilities : public testing::Test
{

};

TEST_F(TestGtUtilities, quoted)
{
    QString qstr{"Hello World"};
    EXPECT_EQ("(" + qstr + ")", gt::brackets(qstr));
    EXPECT_EQ("'" + qstr + "'", gt::squoted(qstr));
    EXPECT_EQ('"' + qstr + '"', gt::quoted(qstr));
    EXPECT_EQ("test_" + qstr + "_test", gt::quoted(qstr, "test_", "_test"));

    QByteArray qba{"My Fancy String"};
    EXPECT_EQ("(" + qba + ")", gt::brackets(qba));
    EXPECT_EQ("'" + qba + "'", gt::squoted(qba));
    EXPECT_EQ('"' + qba + '"', gt::quoted(qba));
    EXPECT_EQ("test_" + qba + "_test", gt::quoted(qba, "test_", "_test"));

    std::string std{"<something_inspirational>"};
    EXPECT_EQ("(" + std + ")", gt::brackets(std));
    EXPECT_EQ("'" + std + "'", gt::squoted(std));
    EXPECT_EQ('"' + std + '"', gt::quoted(std));
    EXPECT_EQ("test_" + std + "_test", gt::quoted(std, "test_", "_test"));
}

TEST_F(TestGtUtilities, valueSuccess)
{
    bool _ok = false;
    bool* ok = &_ok;

    EXPECT_EQ(gt::valueError(12, nullptr), 12);
    EXPECT_FALSE(_ok);
    EXPECT_EQ(gt::valueSuccess(42, ok), 42);
    EXPECT_TRUE(_ok);
}

TEST_F(TestGtUtilities, valueError)
{
    bool _ok = true;
    bool* ok = &_ok;

    EXPECT_EQ(gt::valueError(12, nullptr), 12);
    EXPECT_TRUE(_ok);

    EXPECT_EQ(gt::valueError(42, ok), 42);
    EXPECT_FALSE(_ok);
}

TEST_F(TestGtUtilities, valueSetSuccess)
{
    int _ok = 0;
    int* ok = &_ok;

    EXPECT_EQ(gt::valueSetSuccess(42, 12, ok), 42);
    EXPECT_EQ(_ok, 12);

    // check return type signature
    QString _str = "Hello World";
    QString& str = gt::valueSetSuccess(_str, 0, ok);
    EXPECT_EQ(_str, str);
}

TEST_F(TestGtUtilities, checkNumericalLimits)
{
    EXPECT_TRUE(gt::checkNumericalLimits<int>(-1));
    EXPECT_TRUE(gt::checkNumericalLimits<double>(42.0));

    // an unsigned int can go below 0
    EXPECT_FALSE(gt::checkNumericalLimits<unsigned int>(-1));

    // larger number than an int16
    EXPECT_FALSE(gt::checkNumericalLimits<int16_t>(
                     std::numeric_limits<int16_t>::max() + int32_t{1}));

    // lower number than an int32
    EXPECT_FALSE(gt::checkNumericalLimits<int32_t>(
                     std::numeric_limits<int32_t>::min() - int64_t{1}));
}

TEST_F(TestGtUtilities, objectNames)
{
    QObject o1, o2, o3;
    o1.setObjectName("Bla");
    o2.setObjectName("Blu");
    o3.setObjectName("Blup");
    QList<QObject*> ptrs = {&o1, &o2, &o3};

    QStringList expected = {o1.objectName(), o2.objectName(), o3.objectName()};
    QStringList names = gt::objectNames(ptrs);

    EXPECT_EQ(names, expected);
}

TEST_F(TestGtUtilities, filterObjects)
{
    GtObject g1, g2;

    QObject o1, o2;

    TestSpecialGtObject s1;

    QList<QObject*> objects = {&g1, &g2, &o1, &o2, &s1};

    EXPECT_EQ(objects, gt::filterObjects<QObject*>(objects));

    auto gtobjects = gt::filterObjects<GtObject*>(objects);

    EXPECT_EQ(gtobjects, (QList<GtObject*>{&g1, &g2, &s1}));

    auto sobjects = gt::filterObjects<TestSpecialGtObject*>(gtobjects);

    EXPECT_EQ(sobjects, (QList<TestSpecialGtObject*>{&s1}));
}

TEST_F(TestGtUtilities, container_const_cast)
{
    QVector<GtObject*> qvector = {(GtObject*)0x02};
    QVector<GtObject const*>& const_qvector = gt::container_const_cast(qvector);
    EXPECT_EQ(qvector[0], const_qvector[0]);

    // & does not work here :)
    // QList<int const*>& qlist_tmp  = gt::container_const_cast(QList<int*>{});
    QList<int const*> qlist_tmp  = gt::container_const_cast(QList<int*>{nullptr});
    EXPECT_EQ(qlist_tmp[0], nullptr);

    std::vector<int*, std::allocator<int*>> stdvector = {(int*)0x42};
    std::vector<int const*,
                std::allocator<int const*>
                >& const_stdvector = gt::container_const_cast(stdvector);
    EXPECT_EQ(stdvector[0], const_stdvector[0]);

    int myint = 42;
    std::vector<std::reference_wrapper<int>> const stdrefwrapper = { myint };
    std::vector<std::reference_wrapper<int const>,
                std::allocator<std::reference_wrapper<int const>>
                > const& const_stdrefwrapper = gt::container_const_cast(stdrefwrapper);
    EXPECT_EQ(stdrefwrapper[0], const_stdrefwrapper[0]);
}

TEST_F(TestGtUtilities, finally_typetraits_member_function)
{
    // non-throwing member function
    QMutex m;
    m.lock();

    auto notthrowing = gt::finally(&m, &QMutex::unlock);
    static_assert(
        std::is_nothrow_invocable<decltype(&QMutex::unlock), QMutex*>::value,
        "Must use noexcept member function");
    static_assert(std::is_nothrow_invocable<
                      typename decltype(notthrowing)::function_type>::value,
                  "Expected wrapping lambda to be noexcept too");

    // throwing member function
    QString s;
    auto throwable = gt::finally(&s, &QString::clear);
    static_assert(
        !std::is_nothrow_invocable<decltype(&QString::clear), QString*>::value,
        "Must use noexcept(false) member function");
    static_assert(!std::is_nothrow_invocable<
                      typename decltype(throwable)::function_type>::value,
                  "Expected wrapping lambda to be noexcept(false) too");
}

TEST_F(TestGtUtilities, finally_typetraits_lambda)
{
    // by default lambda are noexcept(false)
    auto notthrowing = gt::finally([]() noexcept {});
    static_assert(std::is_nothrow_invocable<
                      typename decltype(notthrowing)::function_type>::value,
                  "Expected lambda to be noexcept");

    // by default lambda are noexcept(false)
    auto throwable = gt::finally([]() {});
    static_assert(!std::is_nothrow_invocable<
                      typename decltype(throwable)::function_type>::value,
                  "Expected lambda to be noexcept(false)");
}

TEST_F(TestGtUtilities, finally_typetraits_std_function)
{
    // std::functions are always noexcept(false) and may throw
    auto notthrowing = gt::finally(std::function<void()>([]() noexcept {}));
    static_assert(!std::is_nothrow_invocable<
                      typename decltype(notthrowing)::function_type>::value,
                  "Expected std::function to be noexcept(false)");

    // std::functions are always noexcept(false) and may throw
    auto throwable = gt::finally(std::function<void()>([]() {}));
    static_assert(!std::is_nothrow_invocable<
                      typename decltype(throwable)::function_type>::value,
                  "Expected std::function to be noexcept(false)");
}

// gt::Finally must not throw on invocation -> should swallow all exceptions
TEST_F(TestGtUtilities, finally_invoke_with_exception)
{
    EXPECT_NO_THROW({
        auto finally = gt::finally(
            []() { throw std::logic_error("this is a test exception"); });
    });
    EXPECT_NO_THROW({ auto finally = gt::finally([]() { throw 42; }); });
}

// only copy ctor is noexcept(true)
class CopyConstructible
{
public:
    CopyConstructible() noexcept(true)
    {
    }
    CopyConstructible(CopyConstructible const&) noexcept(true)
    {
    }
    CopyConstructible(CopyConstructible&&) noexcept(false)
    {
        throw std::logic_error{"should not have been called!"};
    }
    CopyConstructible& operator=(CopyConstructible const&) = delete;
    CopyConstructible& operator=(CopyConstructible&&) = delete;
    ~CopyConstructible() noexcept
    {
    }

    void operator()() noexcept
    {
    }

    // custom swap function is needed
    friend void swap(CopyConstructible& a, CopyConstructible& b) noexcept
    {
    }
};

// only move ctor is noexcept(true)
class MoveConstructible
{
public:
    MoveConstructible() noexcept(true)
    {
    }
    MoveConstructible(MoveConstructible const&) noexcept(false)
    {
        throw std::logic_error{"should not have been called!"};
    }
    MoveConstructible(MoveConstructible&&) noexcept(true) {};
    MoveConstructible& operator=(MoveConstructible const&) = delete;
    MoveConstructible& operator=(MoveConstructible&&) = delete;
    ~MoveConstructible() noexcept
    {
    }

    void operator()() noexcept
    {
    }

    // custom swap function is needed
    friend void swap(MoveConstructible& a, MoveConstructible& b) noexcept
    {
    }
};

TEST_F(TestGtUtilities, finally_nothrow_copy_constructible)
{
    // must not use `gt::finally wrapper` function,
    // as it would call the move constructor
    auto finally = gt::Finally<CopyConstructible>{CopyConstructible{}};
    Q_UNUSED(finally);

    // test move assignment
    decltype(finally) target{};
    EXPECT_TRUE(target.isNull());
    EXPECT_FALSE(finally.isNull());

    target = std::move(finally);
    EXPECT_FALSE(target.isNull());
    EXPECT_TRUE(finally.isNull());
}

TEST_F(TestGtUtilities, finally_nothrow_move_constructible)
{
    auto finally = gt::finally(MoveConstructible{});
    Q_UNUSED(finally);

    // test move assignment
    decltype(finally) target{};
    EXPECT_TRUE(target.isNull());
    EXPECT_FALSE(finally.isNull());

    target = std::move(finally);
    EXPECT_FALSE(target.isNull());
    EXPECT_TRUE(finally.isNull());
}

TEST_F(TestGtUtilities, finally_member_function)
{
    QMutex m;
    m.lock();

    {
        // alredy locked
        EXPECT_FALSE(m.tryLock());

        auto finally = gt::finally(&m, &QMutex::unlock);
        ASSERT_FALSE(finally.isNull());
        finally.clear();
        EXPECT_TRUE(finally.isNull());
    }

    // still locked
    EXPECT_FALSE(m.tryLock());

    {
        auto finally = gt::finally(&m, &QMutex::unlock);
        ASSERT_FALSE(finally.isNull());

        // still locked
        EXPECT_FALSE(m.tryLock());
    }

    // no longer locked
    EXPECT_TRUE(m.tryLock());
    m.unlock();
}

TEST_F(TestGtUtilities, finally_lambda)
{
    int i = 1;

    auto lambda = [&](){ i--; };

    {
        EXPECT_EQ(i, 1);

        auto finally = gt::finally(lambda);
        ASSERT_FALSE(finally.isNull());
        finally.clear();
        EXPECT_TRUE(finally.isNull());
    }

    EXPECT_EQ(i, 1);

    {
        auto finally = gt::finally(lambda);
        ASSERT_FALSE(finally.isNull());

        EXPECT_EQ(i, 1);
    }

    EXPECT_EQ(i, 0);
}

/// constructing and moving a move-only lambda is supported
TEST_F(TestGtUtilities, finally_move_lambda)
{
    bool called = false;
    std::unique_ptr<GtObject> movable;

    auto finally1 =
        gt::finally([&called, moved = std::move(movable)]() { called = true; });

    auto finally2 = std::move(finally1);

    finally1.finalize();
    EXPECT_FALSE(called);

    finally2.finalize();

    EXPECT_TRUE(called);
}

TEST_F(TestGtUtilities, finally_move_std_function)
{
    bool called = false;
    std::function<void()> myFunction = [&]() { called = true; };

    auto finally1 = gt::finally(myFunction);

    auto finally2 = std::move(finally1);

    finally1.finalize();
    EXPECT_FALSE(called);

    finally2.finalize();

    EXPECT_TRUE(called);

    called = false;

    decltype(finally1) finally3;
    finally3 = gt::finally(myFunction);

    finally3.finalize();

    EXPECT_TRUE(called);
}

TEST_F(TestGtUtilities, finally_move_self)
{
    bool called = false;
    std::function<void()> myFunction = [&]() { called = true; };

    auto finally1 = gt::finally(myFunction);

    finally1 = std::move(finally1);

    finally1.finalize();
    EXPECT_TRUE(called);
}

struct MyMoveOnlyFunctor
{
    bool moved = false, called = false;

    MyMoveOnlyFunctor() noexcept = default;

    MyMoveOnlyFunctor(MyMoveOnlyFunctor const&) = delete;
    MyMoveOnlyFunctor& operator=(MyMoveOnlyFunctor const&) = delete;

    MyMoveOnlyFunctor(MyMoveOnlyFunctor&& o) noexcept : called{o.called}
    {
        o.moved = true;
    }

    MyMoveOnlyFunctor& operator=(MyMoveOnlyFunctor&& o) noexcept
    {
        called = o.called;
        moved = false;
        o.moved = true;
        return *this;
    }

    void operator()() noexcept
    {
        called = true;
    }
};

/// constructing, assigning and moving a move-only functor is supported
TEST_F(TestGtUtilities, finally_move_functor)
{
    gt::Finally<MyMoveOnlyFunctor> finally1{MyMoveOnlyFunctor{}};
    EXPECT_FALSE(finally1.get().moved);
    EXPECT_FALSE(finally1.get().called);

    finally1.finalize();
    EXPECT_FALSE(finally1.get().moved);
    EXPECT_TRUE(finally1.get().called);

    gt::Finally<MyMoveOnlyFunctor> finally2{std::move(finally1)};
    EXPECT_TRUE(finally1.get().moved);
    EXPECT_TRUE(finally1.get().called);
    EXPECT_FALSE(finally2.get().moved);
    EXPECT_TRUE(finally2.get().called);

    gt::Finally<MyMoveOnlyFunctor> finally3{};
    EXPECT_FALSE(finally3.get().moved);
    EXPECT_FALSE(finally3.get().called);
    finally3 = std::move(finally2);

    EXPECT_TRUE(finally2.get().moved);
    EXPECT_TRUE(finally2.get().called);
    EXPECT_FALSE(finally3.get().moved);
    EXPECT_TRUE(finally3.get().called);
}

/// constructing, assigning and moving a move-only functor is supported
TEST_F(TestGtUtilities, finally_move_assignment_cleanup)
{
    bool aInitialized = false;
    bool bInitialized = false;

    {
        auto a = gt::finally<std::function<void()>>(
            [&aInitialized]() { aInitialized = true; });
        auto b = gt::finally<std::function<void()>>(
            [&bInitialized]() { bInitialized = true; });

        EXPECT_FALSE(aInitialized);
        EXPECT_FALSE(bInitialized);

        b = std::move(a);

        EXPECT_FALSE(aInitialized);
        EXPECT_TRUE(bInitialized);
    }

    EXPECT_TRUE(aInitialized);
    EXPECT_TRUE(bInitialized);
}

TEST_F(TestGtUtilities, clamp)
{
    EXPECT_EQ(gt::clamp(1, -9, 7), 1);

    EXPECT_EQ(gt::clamp(-9, 1, 7), 1);

    EXPECT_EQ(gt::clamp(7, -9, 1), 1);

    EXPECT_DOUBLE_EQ(gt::clamp(-1234.12341, 2341.4, 42411.57), 2341.4);

    EXPECT_DOUBLE_EQ(gt::clamp(250034.12341, 2341.4, 42411.57), 42411.57);

    EXPECT_DOUBLE_EQ(gt::clamp(5234.12341, 2341.4, 42411.57), 5234.12341);

    // clamp double to int
    EXPECT_EQ(gt::clamp<int>(523.1234, 2341, 42411), 2341);

    // clamp double to int
    EXPECT_DOUBLE_EQ(gt::clamp<double>(523, 12.134, 321.12345), 321.12345);
}

TEST_F(TestGtUtilities, range)
{
    std::array<size_t, 10> array;
    std::iota(array.begin(), array.end(), 0);
    ASSERT_EQ(array[0], 0);
    ASSERT_EQ(array[9], 9);

    auto range = gt::range(10);
    EXPECT_EQ(range.size(), 10);
    EXPECT_EQ(std::distance(range.begin(), range.end()), 10);
    EXPECT_TRUE(std::equal(range.begin(), range.end(), array.begin(), array.end()));

    range = gt::range(5, 10);
    EXPECT_EQ(range.size(), 5);
    EXPECT_EQ(std::distance(range.begin(), range.end()), 5);
    EXPECT_TRUE(std::equal(range.begin(), range.end(), array.begin() + 5, array.end()));

    range = gt::range(10, 10);
    EXPECT_EQ(range.size(), 0);
    EXPECT_EQ(std::distance(range.begin(), range.end()), 0);

    range = gt::range(10, 20);
    EXPECT_TRUE(std::copy(range.begin(), range.end(), array.begin()) == array.end());

    EXPECT_TRUE(std::equal(range.begin(), range.end(), array.begin(), array.end()));

    EXPECT_EQ(*gt::range(9, 10).begin(), 9);
}
