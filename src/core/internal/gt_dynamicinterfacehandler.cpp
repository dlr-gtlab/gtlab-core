/* GTlab - Gas Turbine laboratory
 * Source File: dynamicinterfacehandler.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QDebug>

#include "gt_dynamicinterfacehandler.h"

using namespace gtlab::internal;

DynamicInterfaceHandler&
DynamicInterfaceHandler::instance()
{
    static DynamicInterfaceHandler handler;
    return handler;
}

bool
DynamicInterfaceHandler::addInterface(gtlab::InterfaceFunction func_ptr)
{
    const auto& ident = func_ptr.name();
    if (m_interfaces.contains(ident))
    {
        qDebug() << "could not add interface. dynamic function with id='"
                 << ident << "' already defined!";
        return false;
    }

    m_interfaces.insert(ident, std::move(func_ptr));

    return true;
}

gtlab::InterfaceFunction
DynamicInterfaceHandler::getInterfaceFunc(const QString& ident)
{
    if (!m_interfaces.contains(ident))
    {
        return nullptr;
    }

    return m_interfaces.value(ident);
}

QStringList DynamicInterfaceHandler::getRegisteredFunctionIDs() const
{
    return m_interfaces.keys();
}

bool gtlab::interface::internal::register_function(InterfaceFunction func)
{
    return gtlab::internal::DynamicInterfaceHandler::instance()
        .addInterface(std::move(func));
}
