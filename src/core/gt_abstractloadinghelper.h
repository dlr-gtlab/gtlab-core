/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractloadinghelper.h
 *
 *  Created on: 29.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GT_ABSTRACTLOADINGHELPER_H
#define GT_ABSTRACTLOADINGHELPER_H

#include "gt_core_exports.h"

#include <QObject>
#include <memory>

/**
 * @brief The GtAbstractLoadingHelper class
 */
class GT_CORE_EXPORT GtAbstractLoadingHelper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractLoadingHelper
     */
    GtAbstractLoadingHelper();

    /**
     * @brief ~GtAbstractLoadingHelper
     */
    ~GtAbstractLoadingHelper() override;

    /**
     * @brief run
     */
    virtual void run() = 0;

public slots:
    /**
     * @brief exec
     */
    void exec();

signals:
    /**
     * @brief finished
     */
    void finished();

};

namespace gt
{

namespace detail
{

    template <typename Func>
    class GtFunctionalLoadingHelper : public GtAbstractLoadingHelper
    {
    public:
        explicit GtFunctionalLoadingHelper(Func f) : f(std::move(f))
        {}

        void run() override
        {
            f();
        }

    private:
        Func f;
    };

} // namespace detail

 /**
 * @brief Creates a loading helper from the user defined function f
 * @param f
 * @return
 */
template <typename Func>
std::unique_ptr<detail::GtFunctionalLoadingHelper<Func>>
makeLoadingHelper(Func&& f)
{
    using FuncLoadingHelper = detail::GtFunctionalLoadingHelper<Func>;
    return std::make_unique<FuncLoadingHelper>(std::forward<Func>(f));
}

} // namespace gt

#endif // GT_ABSTRACTLOADINGHELPER_H
