/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_commandlinefunctionhandler.h
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
    bool addFunction(GtCommandLineFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    GtCommandLineFunction getFunction(const QString& functionId) const;

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
