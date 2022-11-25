/* GTlab - Gas Turbine laboratory
 * Source File: gt_sharedfunctionhandler.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSHAREDFUNCTIONHANDLER_H
#define GTSHAREDFUNCTIONHANDLER_H

#include "gt_core_exports.h"

#include "gt_sharedfunction.h"

#include <QVariantList>
#include <QMap>

namespace gt
{
namespace detail
{

class GT_CORE_EXPORT SharedFunctionHandler
{

public:
    static SharedFunctionHandler& instance();

    /**
     * @brief Adds a function to the handler
     *
     * Returns false, if a function with the same ID already exists
     */
    bool addSharedFunction(const QString& moduleId, SharedFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    SharedFunction getSharedFunction(const QString& moduleId,
                                     const QString& functionId);

    /**
     * @brief Returns the IDs of the registered functions
     */
    QStringList getRegisteredFunctionIDs() const;

private:
    QMap<QString, SharedFunction> m_interfaces;

    SharedFunctionHandler() = default;

};

} // namespace detail


namespace interface
{
namespace detail
{

/**
 * @brief Registers an InterfaceFunction to the handler
 */
GT_CORE_EXPORT bool registerFunction(const QString& moduleId,
                                     SharedFunction func);

} // namespace detail

} // namespace interface

} // namespace gt

#endif // GTSHAREDFUNCTIONHANDLER_H
