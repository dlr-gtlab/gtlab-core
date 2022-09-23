#include <gt_object.h>

#include "gtest/gtest.h"
#include <gt_doubleproperty.h>
#include <gt_polyvector.h>
#include <gt_dynamicpropertycontainer.h>
#include <gt_exceptions.h>

#include <iostream>
#include <initializer_list>


std::ostream&
operator<<(std::ostream& stream, const QString& s)
{
    stream << s.toStdString();
    return stream;
}

struct A
{
    A()
    {
    }
    //A(A&& ) = default;
    virtual int value() const
    {
        return 1;
    }

private:
    A(const A& b) = delete;
};

class B : public A
{
public:
    B()
    {
    }
    B(int value) :
        val(value)
    {
    }
    //B(B&&) {}

    int value() const override
    {
        return val;
    }

private:
    int val{2};
    B(const B& b) = delete;
};


class DynamicSizeProperties
{
};


void
PrintTo(const QString& s, ::std::ostream* stream)
{
    *stream << s.toStdString();
}

TEST(GTObject, PropertyStruct)
{
    GtPropertyStructDefinition point3dClz("point_3d");

    auto makeDoubleProperty = [](double value) {
        return [=](const QString& id) {
            auto p = new GtDoubleProperty(id, id);
            p->setValueFromVariant(value, "");
            return p;
        };
    };

    point3dClz.defineMember("x", makeDoubleProperty(1.0));
    point3dClz.defineMember("y", makeDoubleProperty(2.0));

    // lets define z via the property factory
    point3dClz.defineMember("z", "GtDoubleProperty", 3.0);

    auto p1 = point3dClz.newInstance("p1");

    EXPECT_EQ("point_3d", p1->typeName().toStdString());
    EXPECT_EQ("p1", p1->ident().toStdString());


    EXPECT_TRUE(p1->findProperty("_this_does_not_exist_") == nullptr);

    auto props = p1->properties();
    EXPECT_EQ(3, props.size());
    EXPECT_EQ("x", props[0]->ident().toStdString());
    EXPECT_EQ("y", props[1]->ident().toStdString());
    EXPECT_EQ("z", props[2]->ident().toStdString());

    auto px = p1->findProperty("x");
    auto py = p1->findProperty("y");
    auto pz = p1->findProperty("z");

    ASSERT_TRUE(px != nullptr);
    ASSERT_TRUE(py != nullptr);
    ASSERT_TRUE(pz != nullptr);

    EXPECT_EQ("x", px->ident().toStdString());
    EXPECT_EQ("y", py->ident().toStdString());
    EXPECT_EQ("z", pz->ident().toStdString());

    // check values
    EXPECT_EQ(1.0, px->value<double>());
    EXPECT_EQ(2.0, py->value<double>());
    EXPECT_EQ(3.0, pz->value<double>());
}
