/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUILAUNCHER_H
#define GT_GUILAUNCHER_H

#include "gt_gui_exports.h"

#include <gt_platform.h>
#include <gt_globals.h>

#include <QObject>
#include <QPointer>

class GT_GUI_EXPORT GtGuiLauncher final : public QObject
{
public:

    ~GtGuiLauncher() noexcept override;

    /**
     * @brief Returns the current instance of this launcher
     * @return Self
     */
    GT_NO_DISCARD
    static GtGuiLauncher* instance() noexcept;

private:
    GtGuiLauncher();

    struct Impl;
    std::unique_ptr<Impl> impl;

    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl* pimpl() noexcept;
    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl const* pimpl() const noexcept;
};

#endif // GT_GUILAUNCHER_H
