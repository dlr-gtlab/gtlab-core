/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 12.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTGUIMODULELOADER_H
#define GTGUIMODULELOADER_H

#include <QHash>

#include "gt_coremoduleloader.h"

class GtObject;
class GtObjectUI;

typedef QList<GtObjectUI*> GtObjectUIList;

/**
 * @brief The GtGuiModuleLoader class
 */
class GtGuiModuleLoader : public GtCoreModuleLoader
{
public:
    /**
     * @brief GtGuiModuleLoader
     */
    GtGuiModuleLoader();

    /**
     * @brief ~GtGuiModuleLoader
     */
    ~GtGuiModuleLoader() override;

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class.
     * @param obj Object for retrieving UI classes of the associated class.
     * @return List of all registered user interfaces objects.
     */
    GtObjectUIList objectUI(GtObject* obj) const;

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class name.
     * @param classname Classname for retrieving UI classes.
     * @return List of all registered user interfaces objects.
     */
    GtObjectUIList objectUI(const QString& classname) const;

    /**
     * @brief knownUIObjects - returns a list of all stored object ui classes
     * @return class name list
     */
    QStringList knownUIObjects() const;

protected:
    /**
     * @brief check
     * @param plugin
     * @return
     */
    bool check(GtModuleInterface* plugin) const override;

    /**
     * @brief insert
     * @param plugin
     */
    void insert(GtModuleInterface* plugin) override;

private:
    // object specific user interfaces
    QHash<QString, GtObjectUIList> m_uiObjs;

    /**
     * @brief registerObjectUI
     * @param metaObj
     */
    void registerObjectUI(const char* classId, const QMetaObject& metaObj);

};

#endif // GTGUIMODULELOADER_H
