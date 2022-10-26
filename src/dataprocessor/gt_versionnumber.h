/* GTlab - Gas Turbine laboratory
 * Source File: gt_versionnumber.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 05.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTVERSIONNUMBER_H
#define GTVERSIONNUMBER_H

#include "gt_datamodel_exports.h"

#include <vector>
#include <memory>
#include <QString>

/**
 * @brief The GtVersionNumber class is used for universal versioning of
 * different software parts and modules of GTlab. It implements the semantic
 * versioning in version 2.0.0 (https://semver.org/#spec-item-11) with the
 * exception that minor and patch version are optional.
 * Furthermore, a prioritization of the keyword "dp" (developer preview) takes
 * place when evaluating the pre-release identifier.
 * Credits for code snippets to:
 * https://github.com/DLR-SC/tigl/blob/master/src/Version.h
 */
class GtVersionNumber
{
public:
    /**
     * @brief Constructor
     * @param major Major version number
     * @param minor Minor version number
     * @param patch Patch version number
     * @param additional Additional pre-release information (e.g. alpha, rc, etc.)
     * @param additional Additional build information
     */
    GT_DATAMODEL_EXPORT GtVersionNumber(int major, int minor, int patch,
                                   const QString& pre_release = QString(),
                                   const QString& build = QString());

    /**
     * @brief Constructor
     * @param major Major version number
     * @param minor Minor version number
     * @param additional Additional pre-release information (e.g. alpha, rc, etc.)
     * @param additional Additional build information
     */
    GT_DATAMODEL_EXPORT GtVersionNumber(int major, int minor,
                                        const QString& pre_release = QString(),
                                        const QString& build = QString());

    /**
     * @brief Constructor
     * @param major Major version number
     * @param additional Additional pre-release information (e.g. alpha, rc, etc.)
     * @param additional Additional build information
     */
    GT_DATAMODEL_EXPORT explicit GtVersionNumber(
            int major,
            const QString& pre_release = QString(),
            const QString& build = QString());

    /**
     * @brief Produces a null version.
     */
    GT_DATAMODEL_EXPORT GtVersionNumber();

    /**
     * @brief Produces a version from string. If version could not be
     * reproduced, null version is produced.
     * @param versStr Version string
     */
    GT_DATAMODEL_EXPORT explicit GtVersionNumber(const QString& versStr);

    /**
     * @brief Copy constructor
     * @param v2 Version number.
     */
    GT_DATAMODEL_EXPORT GtVersionNumber(const GtVersionNumber& v2);

    /**
     * @brief Copy assignment operator
     * @param v2 Version number.
     */
    GT_DATAMODEL_EXPORT GtVersionNumber& operator = (const GtVersionNumber& v2);

    /**
     * @brief Destructor.
     */
    GT_DATAMODEL_EXPORT ~GtVersionNumber();

    /**
     * @brief Returns a string with all of the segments (e.g. 1.0.0-alpha)
     * @return Version numer string.
     */
    GT_DATAMODEL_EXPORT QString toString() const;

    /**
     * @brief Returns major version number
     * @return major version number
     */
    GT_DATAMODEL_EXPORT int major() const;

    /**
     * @brief Returns minor version number
     * @return minor version number
     */
    GT_DATAMODEL_EXPORT int minor() const;

    /**
     * @brief Returns patch level
     * @return patch level
     */
    GT_DATAMODEL_EXPORT int patch() const;

    /**
     * @brief Returns pre-release identifier
     * @return pre-release identifier
     */
    GT_DATAMODEL_EXPORT QString preRelease() const;

    /**
     * @brief Returns build version identifier
     * @return build version identifier
     */
    GT_DATAMODEL_EXPORT QString build() const;

    /**
     * @brief Returns true if there are zero numerical segments,
     * otherwise false is returned.
     * @return Whether version contains only zero numerical segments or not.
     */
    GT_DATAMODEL_EXPORT bool isNull() const;

    /**
     * @brief Compares v1 with v2 and returns an integer less than, equal to,
     * or greater than zero, depending on whether v1 is less than, equal to,
     * or greater than v2, respectively.
     * @param v1 Version number for comparison
     * @param v2 Version number for comparison
     * @return Comparison result
     */
    GT_DATAMODEL_EXPORT static int compare(const GtVersionNumber& v1,
                                           const GtVersionNumber& v2);

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

};

/**
 * @brief Returns true if v1 is equal to v2. otherwise false is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is equal to v2.
 */
GT_DATAMODEL_EXPORT bool operator==(const GtVersionNumber& v1,
                                    const GtVersionNumber& v2);

/**
 * @brief Returns true if v1 is not equal to v2. otherwise false is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is not equal to v2.
 */
GT_DATAMODEL_EXPORT bool operator!=(const GtVersionNumber& v1,
                                    const GtVersionNumber& v2);

/**
 * @brief Returns true if v1 is greater than v2. otherwise false is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is greater than v2.
 */
GT_DATAMODEL_EXPORT bool operator>(const GtVersionNumber& v1,
                                   const GtVersionNumber& v2);

/**
 * @brief Returns true if v1 is less than v2. otherwise false is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is less  than v2.
 */
GT_DATAMODEL_EXPORT bool operator<(const GtVersionNumber& v1,
                                   const GtVersionNumber& v2);

/**
 * @brief Returns true if v1 is greater than or equal to v2. otherwise false
 * is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is greater than or equal to v2.
 */
GT_DATAMODEL_EXPORT bool operator>=(const GtVersionNumber& v1,
                                    const GtVersionNumber& v2);

/**
 * @brief Returns true if v1 is less than or equal to v2. otherwise false
 * is returned.
 * @param v1 Version number.
 * @param v2 Version number.
 * @return Whether v1 is less than or equal to v2.
 */
GT_DATAMODEL_EXPORT bool operator<=(const GtVersionNumber& v1,
                                    const GtVersionNumber& v2);

#endif // GTVERSIONNUMBER_H
