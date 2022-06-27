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

using namespace gtlab::internal;

namespace
{
    QString makeKey(const QString& moduleId, const QString& functionId)
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
                                      gtlab::InterfaceFunction func_ptr)
{
    const auto& ident = func_ptr.name();

    auto key = makeKey(moduleId, ident);

    gtDebug() << "Registering shared module function: '" << key << "'.";

    if (m_interfaces.contains(key))
    {
        gtWarning() << "Overwriting shared module function '" << key << "'.";
    }

    m_interfaces[key] = std::move(func_ptr);

    return true;
}

gtlab::InterfaceFunction
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

QStringList DynamicInterfaceHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool gtlab::interface::internal::register_function(const QString& moduleId,
                                              InterfaceFunction func)
{
    return gtlab::internal::DynamicInterfaceHandler::instance()
        .addInterface(moduleId, std::move(func));
}
