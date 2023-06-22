/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include "gt_functionalinterface.h"
#include "gt_commandlinefunction.h"
#include "gt_globals.h"

#define GT_IMPLEMENT_MODULE_ \
    Q_INTERFACES(GtModuleInterface) \
    QString ident() const final { return GT_MODULENAME(); }

#define GT_MODULE_NO_JSON_() \
    Q_PLUGIN_METADATA(IID GT_MODULE_ID)\
    GT_IMPLEMENT_MODULE_

#define GT_MODULE_WITH_JSON_(JSON_FILE) \
    Q_PLUGIN_METADATA(IID GT_MODULE_ID FILE JSON_FILE)\
    GT_IMPLEMENT_MODULE_

#define FUNC_CHOOSER(_f1, _f2, ...) _f2
#define FUNC_RECOMPOSER(argsWithParentheses) FUNC_CHOOSER argsWithParentheses
#define CHOOSE_FROM_ARG_COUNT(...) FUNC_RECOMPOSER((__VA_ARGS__, GT_MODULE_WITH_JSON_, ))
#define NO_ARG_EXPANDER() ,GT_MODULE_NO_JSON_
#define MACRO_CHOOSER(...) CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__ ())


/**
 * To create a gtlab module, use this function at the beginning of the class
 *
 * Usage
 *
 * class MyModule : public QObject, public GtModuleInterface
 {
    Q_OBJECT

    GT_MODULE()
    // or GT_MODULE("my_module.json") // if you like to pass metadata to the module

 */
#define GT_MODULE(...) MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

class QDomElement;

namespace gt
{

/// Function definition for project data upgrades provided by a module
using ConverterFunction = bool (*)(QDomElement& rootElement,
                                   QString const& filePath);

/// Project data upgrade routine provided by a module
struct VersionUpgradeRoutine
{
    /// Target version for project data upgrade routine
    GtVersionNumber target;

    /// specific upgrade function
    ConverterFunction f;
};

} // namespace gt

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

    /// struct to keep metainformation of the module
    struct MetaInformation
    {
        QString author;
        QString authorContact;
        QString licenseShort;
    };


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
     * @brief By implementing this function, the module passes
     * the name of the author to the framework.
     * The authors name is for information to the
     * framework user only and has no effect on the functionality.
     * NOTE: A reference to the author can significantly help the user to
     * know where a module is developed and who to contact.
     * @return name of the author of the module.
     */
    virtual MetaInformation metaInformation() const {
        return {};
    };

    /**
     * @brief In some cases, it is necessary to upgrade the data stored in the
     * project when the module version is updated. This function can be used to
     * add your own update routines to the framework.
     * @return List of all upgrade routines of the module.
     */
    virtual QList<gt::VersionUpgradeRoutine> upgradeRoutines() const {
        return {};
    }

    /**
     * @brief Returns the functions that the module likes to share.
     *
     * A shared function is a special kind of function that can be
     * used by other modules or the core framework without the need
     * to compile against this modules.
     * This is possible since a shared function of type InterfaceFunction
     * has a fixed function signature:
     * QVariantList sharedFunction(const QVariantList&).
     *
     * Reimplement `sharedFunctions`, if like to share generally useful
     * functions with others.
     *
     * @return
     */
    virtual QList<gt::SharedFunction> sharedFunctions() const {
        return {};
    }

    /**
     * @brief Returns the commandline functions that the module
     * offers for the GTlab batcj application.
     *
     * Reimplement `commandLineFunctions`, if like to add a command to
     * the console application
     *
     */
    virtual QList<GtCommandLineFunction> commandLineFunctions() const {
        return {};
    }

    /**
     * @brief Is called directly after loading the module. It can be used to
     * interact with the loading process of the modules.
     */
    virtual void onLoad() {}

    /**
     * @brief Initializes module. Called on application startup.
     */
    virtual void init() {}

};

/**
 * @brief Helper function to return the path to the settings of a module
 * settings value
 * @return
 */
[[deprecated("Use gt::makeModuleSettingsPath instead")]]
inline QString moduleSettingPath(const QString& modID,
                                 const QString& settingID)
{
    return QString("modules/%1/%2").arg(modID, settingID);
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtModuleInterface,
                    "de.dlr.gtlab.GtModuleInterface/2.0")
QT_END_NAMESPACE

#endif // GT_MODULEINTERFACE_H
