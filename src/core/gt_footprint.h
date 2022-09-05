/* GTlab - Gas Turbine laboratory
 * Source File: gt_footprint.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 28.04.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFOOTPRINT_H
#define GTFOOTPRINT_H

#include "gt_core_exports.h"

#include <QString>
#include <QMap>

#include <memory>

class GtFootprintImpl;
class GtVersionNumber;

/**
 * @brief The GtFootprint class provides information about the footprint of a
 * GTlab environment. Furthermore, an existing footprint can be compared with
 * the footprint corresponding to the current environment.
 */
class GtFootprint
{
public:
    /**
     * @brief Constructor.
     * Represents the footprint of the current GTlab environment.
     */
    GT_CORE_EXPORT GtFootprint();

    GT_CORE_EXPORT GtFootprint(const GtFootprint&);

    /**
     * @brief Constructor.
     * Initializes the footprint based on a serialized record (XML-format).
     */
    GT_CORE_EXPORT explicit GtFootprint(const QString& data);

    /**
     * @brief Destructor.
     */
    GT_CORE_EXPORT ~GtFootprint();

    /**
     * @brief Returns true if footprint is valid. Otherwise false is returned.
     * @return footprint validity
     */
    GT_CORE_EXPORT bool isValid() const;

    /**
     * @brief Generates a footprint hash. Considers information about GTlab
     * major, minor and patch version and the loaded modules (including
     * module version).
     * @return footprint hash
     */
    GT_CORE_EXPORT QByteArray generateHash() const;

    /**
     * @brief Exports footprint information to a XML formatted data record.
     * @return XML serialization of footprint
     */
    GT_CORE_EXPORT QString exportToXML() const;

    /**
     * @brief Returns true if footprint is compatible to the current GTlab
     * environment. Otherwise false is returned.
     * @return footprint compatibility
     */
    GT_CORE_EXPORT bool isCompatible() const;

    /**
     * @brief Returns true if footprint is generated by an older GTlab
     * environment comparend to the current environment. Otherwise false is
     * returned.
     * @return true if footprint is based on older GTlab environment
     */
    GT_CORE_EXPORT bool isOlderRelease() const;

    /**
     * @brief Returns true if footprint is generated by an newer GTlab
     * environment comparend to the current environment. Otherwise false is
     * returned.
     * @return true if footprint is based on newer GTlab environment
     */
    GT_CORE_EXPORT bool isNewerRelease() const;

    /**
     * @brief Returns footprint framework version.
     * @return Footprint version
     */
    GT_CORE_EXPORT GtVersionNumber frameworkVersion() const;

    /**
     * @brief Returns a list of all known modules and the corresponding version
     * numbers. Also core framework version is included.
     * @return list of modules and the corresponding version
     * numbers. Also core framework version is included.
     */
    GT_CORE_EXPORT QMap<QString, GtVersionNumber> fullVersionInfo() const;

    /**
     * @brief Returns a list of all modules and the corresponding version
     * numbers that are unknown in the current GTlab environment.
     * @return list of unknown modules and the corresponding version numbers
     */
    GT_CORE_EXPORT QMap<QString, GtVersionNumber> unknownModules() const;

    /**
     * @brief Returns a list of all modules and the corresponding version
     * numbers that are incompatible (different version) with the current GTlab
     * environment.
     * @return list of incompatible modules and the corresponding version
     * numbers
     */
    GT_CORE_EXPORT QMap<QString, GtVersionNumber> incompatibleModules() const;

    /**
     * @brief Returns a list of all modules and the corresponding version
     * numbers that exists in a higher version within the GTlab framework.
     * @return list of updated modules and the corresponding version
     * numbers
     */
    GT_CORE_EXPORT QMap<QString, GtVersionNumber> updatedModules() const;

    /**
     * @brief Returns a list of all known modules and the corresponding version
     * numbers
     * @return list of modules and the corresponding version
     * numbers
     */
    GT_CORE_EXPORT QMap<QString, GtVersionNumber> modules() const;

    /**
     * @brief Returns identification to store and find version information of
     * core framework in footprints.
     * @return Identification string of core framework.
     */
    GT_CORE_EXPORT static QString frameworkIdentificationString();

private:
    /// Private implementation
    std::unique_ptr<GtFootprintImpl> m_pimpl;

};

#endif // GTFOOTPRINT_H
