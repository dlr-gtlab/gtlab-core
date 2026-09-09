/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <gt_grid.h>

#include <QPainter>
#include <QImage>

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

/// setting invalid spacing and subdivions should clamp the spacing values
TEST(Grid, invalid_spacing_and_subdivs)
{
    GtGrid grid;
    grid.setSpacing(0);
    grid.setSubdivisions(0);

    EXPECT_EQ(grid.hSpacing(), 1u);
    EXPECT_EQ(grid.vSpacing(), 1u);
    EXPECT_EQ(grid.hSubdivisions(), 1u);
    EXPECT_EQ(grid.vSubdivisions(), 1u);
}

/// setting an invalid zoom level should clamp the zoom and spacing values
TEST(Grid, invalid_zoom)
{
    GtGrid grid;
    grid.setSpacing(10);
    grid.setSubdivisions(10);

    grid.setCurrentZoom(0.0);
    EXPECT_GT(grid.scaledGridSpacing().hSpacing, 0.0);
    EXPECT_GT(grid.scaledGridSpacing().vSpacing, 0.0);
    EXPECT_GT(grid.scaledMinorGridSpacing().hSpacing, 0.0);
    EXPECT_GT(grid.scaledMinorGridSpacing().vSpacing, 0.0);
}

using GridTestFunctor = std::function<void(GtGrid&)>;

struct GridScaledSpacingTest : public testing::TestWithParam<GridTestFunctor>
{
    GtGrid grid;
};

// test suit for different configurations of grids
INSTANTIATE_TEST_SUITE_P(
    Grid,
    GridScaledSpacingTest,
    testing::Values(
        [](GtGrid& grid){ }, // default initialized grid
        [](GtGrid& grid){ grid.setHSpacing(50.0); }, // custom hspacing
        [](GtGrid& grid){ grid.setVSpacing(25.0); }, // custom vspacing
        [](GtGrid& grid){ grid.setHSubdivisions(5); }, // custom hsubdivs
        [](GtGrid& grid){ grid.setVSubdivisions(3); }, // custom vsubdivs
        [](GtGrid& grid){ grid.setScalingStrategy(GtGrid::ScalingStrategy::Base10); } // scaling strategy
    )
);

/// check that the grid produces expected grid spacing if no zoom or
/// spacing could be cached
TEST_P(GridScaledSpacingTest, cached_zoom_level)
{
    const auto& functor = GetParam();
    functor(grid);

    GtGridSpacing spacing = grid.scaledGridSpacing();

    EXPECT_DOUBLE_EQ(spacing.hSpacing, grid.hSpacing());
    EXPECT_DOUBLE_EQ(spacing.vSpacing, grid.vSpacing());

    // on next grid point
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing}).y(), spacing.vSpacing);

    // x or y closer to next grid point
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.6, spacing.vSpacing}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.6, spacing.vSpacing}).y(), spacing.vSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing * 0.6}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing * 0.6}).y(), spacing.vSpacing);

    // x or y closer to origin
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing}).x(), 0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing}).y(), spacing.vSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing * 0.4}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing, spacing.vSpacing * 0.4}).y(), 0);

    // x and y closer to origin
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing * 0.4}).x(), 0);
    EXPECT_DOUBLE_EQ(grid.computeNearestGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing * 0.4}).y(), 0);

    spacing = grid.scaledMinorGridSpacing();

    // on next grid point
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing}).y(), spacing.vSpacing);

    // x or y closer to next grid point
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.6, spacing.vSpacing}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.6, spacing.vSpacing}).y(), spacing.vSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing * 0.6}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing * 0.6}).y(), spacing.vSpacing);

    // x or y closer to origin
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing}).x(), 0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing}).y(), spacing.vSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing * 0.4}).x(), spacing.hSpacing);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing, spacing.vSpacing * 0.4}).y(), 0);

    // x and y closer to origin
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing * 0.4}).x(), 0);
    EXPECT_DOUBLE_EQ(grid.computeNearestMinorGridPoint({spacing.hSpacing * 0.4, spacing.vSpacing * 0.4}).y(), 0);
}

TEST(Grid, hidden_grid_keeps_cached_zoom_and_spacing_consistent)
{
    GtGrid grid;
    grid.setScalingStrategy(GtGrid::ScalingStrategy::Base2);
    grid.setSpacing(10);
    grid.setCurrentZoom(1.0);
    grid.enableGrid(false);

    EXPECT_EQ(grid.scaledGridSpacing().hSpacing, grid.scaledGridSpacing(1.0).hSpacing);
    EXPECT_EQ(grid.scaledGridSpacing().vSpacing, grid.scaledGridSpacing(1.0).vSpacing);

    QImage image{100, 100, QImage::Format_ARGB32};
    QPainter painter{&image};
    painter.scale(2.0, 2.0);
    grid.paint(painter, QRectF{0, 0, 50, 50});

    grid.setSpacing(20);

    EXPECT_EQ(grid.scaledGridSpacing().hSpacing, grid.scaledGridSpacing(2.0).hSpacing);
    EXPECT_EQ(grid.scaledGridSpacing().vSpacing, grid.scaledGridSpacing(2.0).vSpacing);
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
    size_t ref = 0;

    auto onUpdated = [&emissionCount](){
        emissionCount++;
    };

    QObject::connect(&grid, &GtGrid::updated, &grid, onUpdated);

    EXPECT_EQ(emissionCount, ref);
    
    grid.enableGrid(false); ref++;
    grid.enableMinorGrid(false); ref++;
    grid.show(); ref++;
    grid.hide(); ref++;

    grid.setMajorLineColor({}); ref++;
    grid.setMinorLineColor({}); ref++;
    grid.setAxisColor({}); ref++;

    grid.setMajorPen({}); ref++;
    grid.setMinorPen({}); ref++;
    grid.setAxisPen({}); ref++;
    
    grid.setActiveAxis({}); ref++;
    grid.setMinorGridCutoffDensity(1.0); ref++;
    grid.setScalingStrategy(GtGrid::ScalingStrategy::Fixed); ref++;

    grid.setHSpacing(1u); ref++;
    grid.setVSpacing(1u); ref++;
    grid.setHSubdivisions(1u); ref++;
    grid.setVSubdivisions(1u); ref++;

    EXPECT_EQ(emissionCount, ref);
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

/// check if painter's state is restored  after painting the grid
TEST(Grid, painter_state_restored)
{
    GtGrid grid;

//    char args[] = "";
//    char* argv  = args;
//    int argc    = 0;
//    QApplication app{argc, &argv};

    QImage image{400, 400, QImage::Format_ARGB32};
    QPainter painter{&image};

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::TextAntialiasing, true);

    QPen pen{Qt::red};
    pen.setWidth(5);
    pen.setStyle(Qt::PenStyle::DashDotLine);
    pen.setCosmetic(false);
    pen.setBrush(Qt::black);

    QBrush brush{Qt::black};
    brush.setStyle(Qt::BrushStyle::CrossPattern);

    painter.setPen(pen);
    painter.setBrush(brush);

    grid.paint(painter, image.rect(), GtGrid::PaintOption::PaintAll);

    EXPECT_TRUE(painter.renderHints().testFlag(QPainter::Antialiasing));
    EXPECT_TRUE(painter.renderHints().testFlag(QPainter::TextAntialiasing));

    EXPECT_EQ(painter.pen().color(), pen.color());
    EXPECT_EQ(painter.pen().style(), pen.style());
    EXPECT_EQ(painter.pen().isCosmetic(), pen.isCosmetic());
    EXPECT_EQ(painter.pen().brush().color(), pen.brush().color());

    EXPECT_EQ(painter.brush().color(), brush.color());
    EXPECT_EQ(painter.brush().style(), brush.style());
}
