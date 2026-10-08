/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */
#ifndef GTCONSOLEPROPERTYOVERRIDES_H
#define GTCONSOLEPROPERTYOVERRIDES_H

#include <QString>
#include <QStringList>

class GtObject;

namespace gt
{
    namespace console
    {

        /**
         * @brief A single parsed "--set <path>=<value>" command line override
         *
         * The path is the user facing property path, the value is the raw
         * (still unconverted) string representation of the new property value.
         */
        struct PropertyOverride
        {
            QString path;
            QString value;
        };

        /**
         * @brief Parses the raw values of repeated "--set" options
         *
         * Each entry must have the shape "path=value". The argument is split at
         * the first "=", the remainder is used as raw value.
         *
         * @param rawArgs raw option values as given on the command line
         * @param errors [out] list of human readable errors for invalid entries
         * @return list of valid overrides (empty if at least one entry is invalid)
         */
        QList<PropertyOverride> parsePropertyOverrides(
            const QStringList& rawArgs, QStringList* errors);

        /**
         * @brief Applies a single property path override to the given root object
         *
         * The root object is the implicit root of the path, i.e. the path never
         * leaves the subtree of @root.
         *
         * Path syntax (intentionally small, XPath inspired):
         *   - "/" navigates through the GtObject hierarchy (direct children)
         *   - "." switches from object navigation to property access
         *   - "ObjectName" matches objectName() and must resolve unambiguously
         *   - "ObjectName[n]" selects the one based nth direct child with that
         *     exact objectName()
         *   - "{uuid}" selects the direct child by its object UUID
         *   - "container[i].prop" selects the one based ith entry of a sequential
         *     property container
         *   - "container[{id}].prop" selects the entry with the given id of an
         *     associative property container
         *   - A leading "." forces property access on the current object and avoids
         *     child-object navigation when a path is ambiguous
         *
         * The raw value is passed to the GTlab property conversion and validation
         * mechanism (no expressions, no unit conversion). Only writable properties
         * can be changed, read only and monitoring properties are rejected.
         *
         * @return empty string on success, otherwise a human readable error message
         */
        QString applyPropertyOverride(GtObject& root, const QString& path,
                                      const QString& value);

    } // namespace console
} // namespace gt

#endif // GTCONSOLEPROPERTYOVERRIDES_H
