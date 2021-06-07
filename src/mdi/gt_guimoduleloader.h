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
    virtual ~GtGuiModuleLoader();

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class.
     * @param obj Object for retrieving UI classes of the associated class.
     * @return List of all registered user interfaces objects.
     */
    GtObjectUIList objectUI(GtObject* obj);

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class name.
     * @param classname Classname for retrieving UI classes.
     * @return List of all registered user interfaces objects.
     */
    GtObjectUIList objectUI(const QString& classname);

    /**
     * @brief knownUIObjects - returns a list of all stored object ui classes
     * @return class name list
     */
    QStringList knownUIObjects();

protected:
    /**
     * @brief check
     * @param plugin
     * @return
     */
    virtual bool check(GtModuleInterface* plugin);

    /**
     * @brief insert
     * @param plugin
     */
    virtual void insert(GtModuleInterface* plugin);

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
