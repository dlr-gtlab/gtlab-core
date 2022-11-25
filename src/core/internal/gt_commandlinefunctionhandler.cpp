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

GtCommandLineFunctionHandler&
GtCommandLineFunctionHandler::instance()
{
    static GtCommandLineFunctionHandler handler;
    return handler;
}

bool
GtCommandLineFunctionHandler::addFunction(GtCommandLineFunction func)
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

GtCommandLineFunction
GtCommandLineFunctionHandler::getFunction(const QString& functionId) const
{
    if (!m_interfaces.contains(functionId))
    {
        return {};
    }

    return m_interfaces.value(functionId);
}

QStringList
GtCommandLineFunctionHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool
gt::commandline::registerFunction(GtCommandLineFunction func)
{
    return GtCommandLineFunctionHandler::instance()
        .addFunction(std::move(func));
}
