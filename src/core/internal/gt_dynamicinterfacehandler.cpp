/* GTlab - Gas Turbine laboratory
 * Source File: dynamicinterfacehandler.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include <QCoreApplication>

#include "gt_dynamicinterfacehandler.h"

using namespace gt::detail;

namespace
{
    QString
    makeKey(const QString& moduleId, const QString& functionId)
    {
        return moduleId + "::" + functionId;
    }
}

DynamicInterfaceHandler&
DynamicInterfaceHandler::instance()
{
    static DynamicInterfaceHandler handler;
    return handler;
}

bool
DynamicInterfaceHandler::addInterface(const QString& moduleId,
                                      gt::InterfaceFunction func)
{
    const auto& ident = func.name();

    auto key = makeKey(moduleId, ident);

    if (m_interfaces.contains(key))
    {
        gtWarning() << "Overwriting shared module function '" << key << "'.";
    }

    m_interfaces[key] = std::move(func);

    return true;
}

gt::InterfaceFunction
DynamicInterfaceHandler::getInterfaceFunc(const QString& moduleId,
                                          const QString& functionId)
{
    auto key = makeKey(moduleId, functionId);
    if (!m_interfaces.contains(key))
    {
        return nullptr;
    }

    return m_interfaces.value(key);
}

QStringList
DynamicInterfaceHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool
gt::interface::detail::registerFunction(const QString& moduleId,
                                        InterfaceFunction func)
{
    return gt::detail::DynamicInterfaceHandler::instance()
        .addInterface(moduleId, std::move(func));
}
