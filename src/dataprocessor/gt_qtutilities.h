/* GTlab - Gas Turbine laboratory
 * Source File: gt_qtutilities.h
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include <QObject>

#include <memory>

namespace gt
{

/**
 * @brief Performs a qobject_cast for unique pointers
 *
 * This transfers the ownership of the source pointer to the target
 * pointer
 *
 * Usage:
 *
 * std::unique_ptr<Base> base = ...;
 *
 * auto derived_ptr = unique_qobject_cast<Derived>(std::move(base));
 *
 * @param src
 * @return
 */
template<typename Derived, typename Base>
std::unique_ptr<Derived>
unique_qobject_cast(std::unique_ptr<Base>&& basePtr) noexcept
{
    auto derivedPtr = std::unique_ptr<Derived>(
        qobject_cast<Derived*>(basePtr.get()));

    if (derivedPtr)
    {
        // transfer ownership
        basePtr.release();
        return derivedPtr;
    }

    return nullptr;
}

} // namespace gt
