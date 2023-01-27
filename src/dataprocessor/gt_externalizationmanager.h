/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizationmanager.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 18.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDOBJECTMANAGER_H
#define GTEXTERNALIZEDOBJECTMANAGER_H


#include "gt_datamodel_exports.h"
#include "gt_abstractexternalizationinterface.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QPointer>

#define gtExternalizationManager (GtExternalizationManager::instance())

class GtObject;
class GtExternalizedObject;
class GtAbstractExternalizationInterface;
/**
 * @brief The GtExternalizationManager class
 */
class GT_DATAMODEL_EXPORT GtExternalizationManager : public QObject
{
    Q_OBJECT

    using ExternalizationInterface = GtAbstractExternalizationInterface*;
    using ExternalizationInterfacePtr = QPointer<ExternalizationInterface*>;

public:

    /**
     * @brief instance
     * @return singleton instance
     */
    static GtExternalizationManager* instance();

    /**
     * @brief isExternalizationEnabled
     * @return whether to use the externalization feature
     */
    bool isExternalizationEnabled() const;

    /**
     * @brief returns the base path of the project
     * @return project dir
     */
    const QString& projectDir() const;

    /**
     * @brief updates the project dir. Should be called once a project was
     * loaded.
     * @param projectDir new project dir
     */
    void setProjectDir(QString projectDir);

    /**
     * @brief clears the project dir. Should be called once a project was
     * closed.
     */
    void clearProjectDir();

    /**
     * @brief initializes all externalized objects. Must be called after the
     * initial project structure was created
     * @param root Root object
     */
    void initExternalizedObjects(GtObject& root);

    /**
     * @brief registers an externalization interface. Will receive info
     * if project was loaded.
     * @param inter interface to register
     */
    void registerExernalizationInterface(ExternalizationInterface inter);

public slots:

    /**
     * @brief setter to enable/disable externalization globally.
     * @param value true if externalization should be used
     */
    void enableExternalization(bool value);

    /**
     * @brief varaint based setter to enable/disable externalization globally.
     * @param boolValue Must be convertible to bool. true if externalization
     * should be enabled
     */
    void enableExternalization(const QVariant& boolValue);

    void onProjectLoaded(QString projectDir);

private:

    /**
     * @brief GtExternalizationManager
     */
    GtExternalizationManager();

    /// path to the directory of the current project
    QString m_projectDir{};

    /// whether to use the externalization feature
    bool m_isExternalizationEnabled{false};

    /// externalization interfaces
    QVector<ExternalizationInterface> m_externalizationInterfaces{};
};

#endif // GTEXTERNALIZEDOBJECTMANAGER_H
