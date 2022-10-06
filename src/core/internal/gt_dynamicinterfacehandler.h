/* GTlab - Gas Turbine laboratory
 * Source File: dynamicinterfacehandler.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef DYNAMICINTERFACEHANDLER_H
#define DYNAMICINTERFACEHANDLER_H

#include "gt_core_exports.h"

#include "gt_dynamicinterface.h"

#include <QVariantList>
#include <QMap>

namespace gtlab
{
namespace internal
{

class GT_CORE_EXPORT DynamicInterfaceHandler
{

public:
    static DynamicInterfaceHandler& instance();

    /**
     * @brief Adds a function to the handler
     *
     * Returns false, if a function with the same ID already exists
     */
    bool addInterface(const QString& moduleId, InterfaceFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    InterfaceFunction getInterfaceFunc(const QString& moduleId,
                                       const QString& functionId);

    /**
     * @brief Returns the IDs of the registered functions
     */
    QStringList getRegisteredFunctionIDs() const;

private:
    QMap<QString, InterfaceFunction> m_interfaces;

    DynamicInterfaceHandler() = default;

};

} // namespace internal


namespace interface
{
namespace internal
{

/**
 * @brief Registers an InterfaceFunction to the handler
 */
GT_CORE_EXPORT bool register_function(const QString& moduleId,
                                      InterfaceFunction func);

} // namespace internal
} // namespace interface
} // namespace gtlab

#endif // DYNAMICINTERFACEHANDLER_H
