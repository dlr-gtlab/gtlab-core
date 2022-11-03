/* GTlab - Gas Turbine laboratory
 * Source File: dynamicinterface.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 08.06.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#ifndef DYNAMICINTERFACE_H
#define DYNAMICINTERFACE_H

#include "gt_core_exports.h"

#include <QString>
#include <QVariantList>

#include <functional>

namespace gt
{

/**
 * @brief This is the functional interface
 *
 * It provides a function with the signature
 *
 *   std::function<QVariantList(const QVariantList&)>
 *
 * a help() and name() method.
 *
 * To convert arbitrary functions into InterfaceFunction,
 * use `auto func = gtlab::interface::make_interface_function(the_function)`
 */
class GT_CORE_EXPORT InterfaceFunction
{
public:
    using FunctionType = std::function<QVariantList(const QVariantList&)>;

    InterfaceFunction(QString funcName, FunctionType func, QString help) :
        m_f(std::move(func)),
        m_name(std::move(funcName)),
        m_help(std::move(help))
    {}

    InterfaceFunction() = default;

    // We want to have implicit conversion from nullptr as it should behave
    // as a function pointer
    // cppcheck-suppress noExplicitConstructor // NOLINTNEXTLINE
    InterfaceFunction(nullptr_t) {}

    /**
     * @brief Calls the function
     */
    QVariantList operator()(const QVariantList& l) const
    {
        return m_f(l);
    }

    /**
     * @brief The name of the function
     */
    const QString& name() const
    {
        return m_name;
    }

    /**
     * @brief Returns the docstring of the interface function
     */
    const QString& help() const
    {
        return m_help;
    }

    /**
     * @brief checks, whether the function is a nullptr
     */
    bool isNull() const
    {
        return !m_f;
    }

    // We want to have implicit bool checks aka if (myFunction)
    // NOLINTNEXTLINE
    operator bool() const
    {
        return !isNull();
    }

    bool operator==(nullptr_t) const
    {
        return isNull();
    }

    bool operator!=(nullptr_t) const
    {
        return !isNull();
    }

private:
    FunctionType m_f;
    QString m_name, m_help;
};

namespace interface
{
/**
 * @brief Retrieves an interface function from Interface Handler
 *
 * Usage:
 *
 * auto itf_mypow = interface::get:function("mymodule", "mypow");
 *
 * auto result = itf_mypow({3.0, 2}); // returns QVariantList({9.0})
 */
GT_CORE_EXPORT InterfaceFunction
getFunction(const QString& moduleId, const QString& functionId);

}  // namespace interface

} // namespace gt

#endif // DYNAMICINTERFACE_H
