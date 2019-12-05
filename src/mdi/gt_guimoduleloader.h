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
     * @brief objectUI
     * @param obj
     * @return
     */
    GtObjectUI* objectUI(GtObject* obj);

    /**
     * @brief objectUI
     * @param className
     * @return
     */
    GtObjectUI* objectUI(const QString& classname);

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
    QHash<QString, GtObjectUI*> m_uiObjs;

    // temporary user interface objects
    QMap<QString, GtObjectUI*> m_uiObjsTmp;

    /**
     * @brief registerObjectUI
     * @param metaObj
     */
    void registerObjectUI(const char* classId, const QMetaObject& metaObj);

};

#endif // GTGUIMODULELOADER_H
