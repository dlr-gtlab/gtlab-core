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

namespace gt
{

class GT_CORE_EXPORT CommandLineFunctionHandler
{

public:
    static CommandLineFunctionHandler& instance();

    /**
     * @brief Adds a function to the handler
     *
     * Returns false, if a function with the same ID already exists
     * and will not add this function
     */
    bool addFunction(gt::CommandLineFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    gt::CommandLineFunction getFunction(const QString& functionId) const;

    /**
     * @brief Returns the IDs of the registered functions
     */
    QStringList getRegisteredFunctionIDs() const;

private:
    QMap<QString, gt::CommandLineFunction> m_interfaces;

    CommandLineFunctionHandler() = default;
};

namespace commandline
{

/**
 * @brief Registers an InterfaceFunction to the handler
 */
GT_CORE_EXPORT bool registerFunction(gt::CommandLineFunction func);

} // namespace commandline

} // namespace gt

#endif // GTCOMMANDLINEFUNCTIONHANDLER_H
