/* GTlab - Gas Turbine laboratory
 * Source File: gt_commandlinefunctionhandler.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 14.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTCOMMANDLINEFUNCTIONHANDLER_H
#define GTCOMMANDLINEFUNCTIONHANDLER_H

#include "gt_core_exports.h"

#include "gt_commandlinefunction.h"

#include <QMap>

class GT_CORE_EXPORT GtCommandLineFunctionHandler
{

public:
    static GtCommandLineFunctionHandler& instance();

    /**
     * @brief Adds a function to the handler
     *
     * Returns false, if a function with the same ID already exists
     * and will not add this function
     */
    bool addInterface(GtCommandLineFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    GtCommandLineFunction getInterfaceFunc(const QString& functionId) const;

    /**
     * @brief Returns the IDs of the registered functions
     */
    QStringList getRegisteredFunctionIDs() const;

private:
    QMap<QString, GtCommandLineFunction> m_interfaces;

    GtCommandLineFunctionHandler() = default;
};

namespace gt
{
namespace commandline
{

/**
 * @brief Registers an InterfaceFunction to the handler
 */
GT_CORE_EXPORT bool registerFunction(GtCommandLineFunction func);

} // namespace commandline

} // namespace gt

#endif // GTCOMMANDLINEFUNCTIONHANDLER_H