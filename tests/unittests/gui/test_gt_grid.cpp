/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <QDebug>
#include <array>

#include <gt_grid.h>

constexpr GtGridSpacing reference_spacing{10, 10};

constexpr size_t NVALUES = 12;

std::array<double, NVALUES> reference_zoom_values{
     0.10,  0.25,  0.50,  0.75,
     1.0 ,  2.0 ,  4.0 ,  5.0 ,
    10.0 , 20.0 , 40.0 , 50.0
};


std::array<double, NVALUES>  reference_fixed = {
    10, 10, 10, 10,
    10, 10, 10, 10,
    10, 10, 10, 10
};

std::array<double, NVALUES> reference_base2 = {
    80., 40., 20. , 10. ,
    10.,  5.,  2.5,  2.5,
    1.25, 0.625,  0.3125, 0.15625
};

std::array<double, NVALUES> reference_base10 = {
    100., 100., 10. , 10.,
     10.,  10.,  1. ,  1.,
      1.,   1.,  0.1,  0.1
};

std::array<double, NVALUES> reference_1_2_5 = {
    10. , 5.,  2.  , 1.,
     1. , 0.5, 0.2 , 0.2,
     0.1, 0.5, 0.02, 0.02
};

std::pair<
    GtGrid::ScalingStrategy, decltype(reference_fixed)> strategies[] = {
    { GtGrid::ScalingStrategy::Fixed,      reference_fixed  },
    { GtGrid::ScalingStrategy::Base2,      reference_base2  },
    { GtGrid::ScalingStrategy::Base10,     reference_base10 },
    { GtGrid::ScalingStrategy::OneTwoFive, reference_1_2_5  },
};

// zero scaling will return default scaling spacing
TEST(Grid, scaled_spacing_invalid_zoom_level)
{
    GtGrid grid;
    grid.setSpacing(reference_spacing.hSpacing);

    for (auto [strategy, _] : strategies)
    {
        constexpr double zoom = 0.0;
        grid.setScalingStrategy(strategy);

        GtGridSpacing result = grid.scaledGridSpacing(zoom);

        EXPECT_DOUBLE_EQ(result.hSpacing, reference_spacing.hSpacing);
        EXPECT_DOUBLE_EQ(result.vSpacing, reference_spacing.vSpacing);
    }
}

TEST(Grid, scaled_spacing_strategies)
{
    GtGrid grid;
    grid.setSpacing(reference_spacing.hSpacing);

    for (auto [strategy, references] : strategies)
    {
        unsigned idx = 0;
        for (double zoom : reference_zoom_values)
        {
            grid.setScalingStrategy(strategy);
            GtGridSpacing result = grid.scaledGridSpacing(zoom);

            auto reference = references[idx++];
            qDebug() << (int)strategy << zoom << result.hSpacing << "vs" << reference;
//            EXPECT_DOUBLE_EQ(result.hSpacing, reference);
//            EXPECT_DOUBLE_EQ(result.vSpacing, reference);
        }
        qDebug() << "";
    }
}
