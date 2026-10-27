/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guilauncher.h"

struct GtGuiLauncher::Impl
{


}; // struct Impl

GtGuiLauncher::GtGuiLauncher() :
    impl{std::make_unique<Impl>()}
{ }

GtGuiLauncher::~GtGuiLauncher() noexcept = default;

GtGuiLauncher&
GtGuiLauncher::instance() noexcept
{
    static GtGuiLauncher self;
    return self;
}

GtGuiLauncher::Impl*
GtGuiLauncher::pimpl() noexcept
{
    return impl.get();
}

const GtGuiLauncher::Impl*
GtGuiLauncher::pimpl() const noexcept
{
    return impl.get();
}

