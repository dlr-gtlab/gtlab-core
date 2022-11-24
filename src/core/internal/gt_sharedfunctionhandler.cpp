/* GTlab - Gas Turbine laboratory
 * Source File: gt_sharedfunctionhandler.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include <QCoreApplication>

#include "gt_sharedfunctionhandler.h"

using namespace gt::detail;

namespace
{
    QString
    makeKey(const QString& moduleId, const QString& functionId)
    {
        return moduleId + "::" + functionId;
    }
}

SharedFunctionHandler&
SharedFunctionHandler::instance()
{
    static SharedFunctionHandler handler;
    return handler;
}

bool
SharedFunctionHandler::addSharedFunction(const QString& moduleId,
                                         gt::SharedFunction func)
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

gt::SharedFunction
SharedFunctionHandler::getSharedFunction(const QString& moduleId,
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
SharedFunctionHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool
gt::interface::detail::registerFunction(const QString& moduleId,
                                        SharedFunction func)
{
    return gt::detail::SharedFunctionHandler::instance()
        .addSharedFunction(moduleId, std::move(func));
}
