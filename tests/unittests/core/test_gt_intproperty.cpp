#include "gtest/gtest.h"
#include "gt_intproperty.h"
#include "gt_property.h"
#include <chrono>
#include <iostream>
#include <QVariant>

TEST(TestIntPropertyTiming, getVal) {
    const int count = 1000000;

    GtIntProperty prop{"prop","m"};
    (void) prop.setValueFromVariant(QVariant(2),"m");

    // Measure getVal() loop
    auto getStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; i++)
    {
        volatile int val = prop.getVal();
    }
    auto getEnd = std::chrono::high_resolution_clock::now();
    auto getElapsed = std::chrono::duration_cast<std::chrono::microseconds>(getEnd - getStart).count();
    std::cout << "Get loop took " << getElapsed << " us" << std::endl;
}
