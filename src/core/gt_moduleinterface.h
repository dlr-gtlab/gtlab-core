/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_MODULEINTERFACE_H
#define GT_MODULEINTERFACE_H

#include "gt_core_exports.h"

#include <QtPlugin>

#include "gt_versionnumber.h"

class QDomElement;

/// Function definition for project data upgrades provided by a module
typedef bool (*ConverterFunction)(QDomElement&, const QString&);

/// Project data upgrade routine provided by a module
struct VersionUpgradeRoutine
{
    /// Target version for project data upgrade routine
    GtVersionNumber target;

    /// specific upgrade function
    ConverterFunction f;
};

/**
 * @brief Main interface that must be implemented by a module in order for it
 * to be recognized by the framework. All pure virtual functions must be
 * implemented according to the specifications. This concerns especially the
 * module identification string, the module version and the module description.
 * A main() method is not required.
 * Detailed information:
 * https://wiki.dlr.de/display/GTLAB/Structure+of+a+GTlab+module
 */
class GT_CORE_EXPORT GtModuleInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtModuleInterface() {}

    /**
     * @brief By implementing this function, the module passes its
     * identification string to the framework. The module can be uniquely
     * identified within the framework via the string.
     * NOTE: This id should usually be specified only once and should not be
     * modified afterwards. Subsequent changes to the id could lead to
     * inconsistent data and undefined behavior.
     * @return identification string of module
     */
    virtual QString ident() const = 0;

    /**
     * @brief By implementing this function, the module passes its current
     * version number to the framework. The version number is used within the
     * framework for identification. In this way, incompatible data sets or
     * module updates can be detected by the framework.
     * NOTE: Each time the module is adapted, the version number should be
     * incremented. Otherwise, unwanted behavior may occur, e.g. inconsistent
     * data, framework crashes.
     * @return current version number of module
     */
    virtual GtVersionNumber version() = 0;

    /**
     * @brief By implementing this function, the module passes a short
     * description to the framework. The description is for information to the
     * framework user only and has no effect on the functionality.
     * NOTE: A description can significantly help the user to
     * understand what functionalities the respective module provides to the
     * framework.
     * @return description of the module.
     */
    virtual QString description() const = 0;

    /**
     * @brief In some cases, it is necessary to upgrade the data stored in the
     * project when the module version is updated. This function can be used to
     * add your own update routines to the framework.
     * @return List of all upgrade routines of the module.
     */
    virtual QList<VersionUpgradeRoutine> upgradeRoutines() const {
        return {};
    };

    /**
     * @brief Initializes module. Called on application startup.
     */
    virtual void init() {};

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtModuleInterface,
                    "de.dlr.gtlab.GtModuleInterface/2.0")
QT_END_NAMESPACE

#endif // GT_MODULEINTERFACE_H
