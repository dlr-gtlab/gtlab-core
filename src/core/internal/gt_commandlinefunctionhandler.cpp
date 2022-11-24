/* GTlab - Gas Turbine laboratory
 * Source File: gt_commandlinefunctionhandler.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 14.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_commandlinefunctionhandler.h"
#include "gt_logging.h"

gt::CommandLineFunctionHandler&
gt::CommandLineFunctionHandler::instance()
{
    static gt::CommandLineFunctionHandler handler;
    return handler;
}

bool
gt::CommandLineFunctionHandler::addFunction(gt::CommandLineFunction func)
{
    const auto& ident = func.id();

    if (m_interfaces.contains(ident))
    {
        gtWarning() << "Cannot overwriting existing command line function '"
                    << ident << "'.";
        return false;
    }

    m_interfaces[ident] = std::move(func);

    return true;
}

gt::CommandLineFunction
gt::CommandLineFunctionHandler::getFunction(const QString& functionId) const
{
    if (!m_interfaces.contains(functionId))
    {
        return {};
    }

    return m_interfaces.value(functionId);
}

QStringList
gt::CommandLineFunctionHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool
gt::commandline::registerFunction(gt::CommandLineFunction func)
{
    return gt::CommandLineFunctionHandler::instance()
        .addFunction(std::move(func));
}
