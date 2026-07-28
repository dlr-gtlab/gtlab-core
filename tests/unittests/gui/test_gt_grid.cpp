/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <QPen>

#include <gt_grid.h>

#include <array>

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
    100., 50. ,  20. ,  10.,
     10.,  5. ,   2. ,   2.,
      1.,  0.5,   0.2,   0.2
};

std::pair<
    GtGrid::ScalingStrategy, decltype(reference_fixed)> strategies[] = {
    { GtGrid::ScalingStrategy::Fixed,      reference_fixed  },
    { GtGrid::ScalingStrategy::Base2,      reference_base2  },
    { GtGrid::ScalingStrategy::Base10,     reference_base10 },
    { GtGrid::ScalingStrategy::OneTwoFive, reference_1_2_5  },
};

/// zero scaling will return default scaling spacing
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

/// zero spacing and subdivions
TEST(Grid, zero_spacing_and_subdivs)
{
    GtGrid grid;
    grid.setSpacing(0);
    grid.setSubdivisions(0);

    EXPECT_EQ(grid.hSpacing(), 1u);
    EXPECT_EQ(grid.vSpacing(), 1u);
    EXPECT_EQ(grid.hSubdivisions(), 1u);
    EXPECT_EQ(grid.vSubdivisions(), 1u);
}

/// zero zoom
TEST(Grid, zero_zoom)
{
    GtGrid grid;
    grid.setSpacing(10);
    grid.setSubdivisions(10);

    grid.setCurrentZoom(0.0);
    EXPECT_GT(grid.currentGridSpacing().hSpacing, 0.0);
    EXPECT_GT(grid.currentGridSpacing().vSpacing, 0.0);
    EXPECT_GT(grid.currentMinorGridSpacing().hSpacing, 0.0);
    EXPECT_GT(grid.currentMinorGridSpacing().vSpacing, 0.0);
}

/// checks if scaling strategies generate expected spacings at different zoom
/// levels for different base grid spacings
TEST(Grid, scaled_spacing_strategies_by_zoom_levels)
{
    constexpr unsigned subdivs = 5;
    GtGrid grid;
    grid.setSubdivisions(subdivs);

    // change grid by fixed ammount
    for (double modifier : { 1.0, 0.2, 2.0 })
    {
        grid.setSpacing(reference_spacing.hSpacing * modifier);

        // test all strategies
        for (auto [strategy, references] : strategies)
        {
            // test all zoom levels
            unsigned idx = 0;
            for (double zoom : reference_zoom_values)
            {
                grid.setScalingStrategy(strategy);

                // reference spacing should be scaled with modifier
                GtGridSpacing result = grid.scaledGridSpacing(zoom);
                auto reference = references[idx] * modifier;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);

                // minor grid is calcualted by main spacing
                result = grid.scaledMinorGridSpacing(zoom);
                reference = references[idx] * modifier / subdivs;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);
                idx++;
            }
        }
    }
}

/// variation, check if scaling strategy correctly
/// updates the resulting grid spacing
TEST(Grid, scaled_spacing_strategies_by_strategies)
{
    constexpr unsigned subdivs = 5;
    GtGrid grid;
    grid.setSubdivisions(subdivs);

    // test all zoom levels
    unsigned idx = 0;
    for (double zoom : reference_zoom_values)
    {
        // change grid by fixed ammount
        for (double modifier : { 1.0, 0.2, 2.0 })
        {
            grid.setSpacing(reference_spacing.hSpacing * modifier);

            // test all strategies
            for (auto [strategy, references] : strategies)
            {
                grid.setScalingStrategy(strategy);

                // reference spacing should be scaled with modifier
                GtGridSpacing result = grid.scaledGridSpacing(zoom);
                auto reference = references[idx] * modifier;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);

                // minor grid is calcualted by main spacing
                result = grid.scaledMinorGridSpacing(zoom);
                reference = references[idx] * modifier / subdivs;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);
            }
        }
        idx++;
    }
}

/// variation, check if base spacing correctly
/// updates the resulting grid spacing
TEST(Grid, scaled_spacing_strategies_by_spacing)
{
    constexpr unsigned subdivs = 5;
    GtGrid grid;
    grid.setSubdivisions(subdivs);

    // test all zoom levels
    unsigned idx = 0;
    for (double zoom : reference_zoom_values)
    {
        // test all strategies
        for (auto [strategy, references] : strategies)
        {
            grid.setScalingStrategy(strategy);

            // change grid by fixed ammount
            for (double modifier : { 1.0, 0.2, 2.0 })
            {
                grid.setSpacing(reference_spacing.hSpacing * modifier);

                // reference spacing should be scaled with modifier
                GtGridSpacing result = grid.scaledGridSpacing(zoom);
                auto reference = references[idx] * modifier;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);

                // minor grid is calcualted by main spacing
                result = grid.scaledMinorGridSpacing(zoom);
                reference = references[idx] * modifier / subdivs;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);
            }
        }
        idx++;
    }
}

/// variation, check if base spacing correctly
/// updates the resulting grid spacing
TEST(Grid, scaled_spacing_strategies_by_subdivions)
{
    constexpr unsigned subdivs = 5;
    GtGrid grid;
    grid.setSubdivisions(subdivs);

    // test all zoom levels
    unsigned idx = 0;
    for (double zoom : reference_zoom_values)
    {
        // test all strategies
        for (auto [strategy, references] : strategies)
        {
            grid.setScalingStrategy(strategy);
            grid.setSpacing(reference_spacing.hSpacing);

            // change grid by fixed ammount
            for (unsigned subdivs : { 1, 2, 5, 10 })
            {
                grid.setSubdivisions(subdivs);

                // minor grid is calcualted by main spacing
                GtGridSpacing result = grid.scaledMinorGridSpacing(zoom);
                auto reference = references[idx] / subdivs;
                EXPECT_DOUBLE_EQ(result.hSpacing, reference);
                EXPECT_DOUBLE_EQ(result.vSpacing, reference);
            }
        }
        idx++;
    }
}

/// check if updated signal is emitted for all changes to the grid
TEST(Grid, emit_updated)
{
    GtGrid grid;

    size_t emissionCount = 0;

    auto onUpdated = [&emissionCount](){
        emissionCount++;
    };

    QObject::connect(&grid, &GtGrid::updated, &grid, onUpdated);

    EXPECT_EQ(emissionCount, 0u);

    grid.setShowGrid(false);
    grid.setShowMinorGrid(false);
    grid.setShowAxis(false);
    grid.show();
    grid.hide();

    grid.setMajorLineColor({});
    grid.setMinorLineColor({});
    grid.setAxisColor({});

    grid.setMajorPen({});
    grid.setMinorPen({});
    grid.setAxisPen({});

    grid.setVisibleAxis({});
    grid.setMinorGridCutoffDensity(1.0);
    grid.setScalingStrategy(GtGrid::ScalingStrategy::Fixed);

    grid.setHSpacing(1u);
    grid.setVSpacing(1u);
    grid.setHSubdivisions(1u);
    grid.setVSubdivisions(1u);

    EXPECT_EQ(emissionCount, 18u);
}

/// check if compute nearest points correctly calculates the nearest point
TEST(Grid, compute_nearest_point)
{
    GtGrid grid;

    grid.setHSpacing(10);
    grid.setVSpacing(5);
    grid.setHSubdivisions(5);
    grid.setVSubdivisions(2);

    grid.setCurrentZoom(1.0);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({0.0, 0.0}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({0.0, 0.0}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({0.0, 0.0}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({0.0, 0.0}).y(), 0.0);


    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({10.0, 5.0}).x(), 10.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({10.0, 5.0}).y(),  5.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({10.0, 5.0}).x(), 10.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({10.0, 5.0}).y(),  5.0);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-10.0, -5.0}).x(), -10.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-10.0, -5.0}).y(),  -5.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-10.0, -5.0}).x(), -10.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-10.0, -5.0}).y(),  -5.0);


    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({2.0, 2.6}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({2.0, 2.6}).y(), 5.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({2.0, 2.6}).x(), 2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({2.0, 2.6}).y(), 2.5);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-2.0, -2.6}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-2.0, -2.6}).y(), -5.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-2.0, -2.6}).x(), -2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-2.0, -2.6}).y(), -2.5);


    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({2.0, 2.4}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({2.0, 2.4}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({2.0, 2.4}).x(), 2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({2.0, 2.4}).y(), 2.5);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-2.0, -2.4}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-2.0, -2.4}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-2.0, -2.4}).x(), -2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-2.0, -2.4}).y(), -2.5);


    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({1.5, 1.76}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({1.5, 1.76}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({1.5, 1.76}).x(), 2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({1.5, 1.76}).y(), 2.5);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-1.5, -1.26}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-1.5, -1.26}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-1.5, -1.26}).x(), -2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-1.5, -1.26}).y(), -2.5);


    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({1.5, 1.24}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({1.5, 1.24}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({1.5, 1.24}).x(), 2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({1.5, 1.24}).y(), 0.0);

    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-1.5, -1.24}).x(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({-1.5, -1.24}).y(), 0.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-1.5, -1.24}).x(), -2.0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({-1.5, -1.24}).y(),  0.0);
}
