/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOR_H
#define GTCALCULATOR_H

#include "gt_calculators_exports.h"

#include "gt_processcomponent.h"

#include <QPointer>

#include "gt_boolproperty.h"
#include "gt_objectpath.h"

class QDir;
class GtModeProperty;
class GtLabelProperty;
class GtAbstractRunnable;
class GtObjectLinkProperty;
class GtObjectPathProperty;

/**
 * @brief The GtCalculator class
 */
class GT_CALCULATORS_EXPORT GtCalculator : public GtProcessComponent
{
    Q_OBJECT

public:
    /**
     * @brief Destructor
     */
    virtual ~GtCalculator();

    /**
     * @brief Execution process of the calculator.
     * @return Whether execution process was successful or not.
     */
    bool exec();

    /**
     * @brief Main run method of the calculator.
     * @return Whether run process was successful or not.
     */
    virtual bool run() = 0;

    /**
     * @brief Returns selected execution mode of the calculator.
     * @return Execution mode identification string.
     */
    const QString& execMode();

    /**
     * @brief Sets the execution mode of the calculator.
     * @param Identification string of the new execution mode.
     */
    void setExecMode(const QString& execMode);

    /**
     * @brief Sets the execution mode to local exectution (default)
     */
    void setExecModeLocal();

    /**
     * @brief Returns list of all linked datamodel objects.
     * @return List of linked datamodel objects.
     */
    const QList<QPointer<GtObject> >& linkedObjects();

    /**
     * @brief Returns process and project specific temporary directory.
     * @return Temporary directory object.
     */
    QDir tempDir();

    /**
     * @brief Returns current execution identification label.
     * @return Identification label.
     */
    const QString& executionLabel();

    /**
     * @brief Sets current execution identification label.
     * @param New Identification label.
     */
    void setExecutionLabel(const QString& label);

    /**
     * @brief Returns true if calculator is set to skip mode.
     * @return Skip indicator.
     */
    bool isSkipped();

    /**
     * @brief Sets calculator skip mode.
     * @param Whether calculator should be skipped or not.
     */
    void setSkipped(bool val);

    /**
     * @brief Returns true if run should be marked as failed if warning flag
     * is activated. Otherwise false is returnes.
     * @return Whether run should be marked as failed or not.
     */
    bool runFailsOnWarning();

    /**
     * @brief Sets current state of process component.
     * @param New state.
     */
    void setState(GtProcessComponent::STATE state) Q_DECL_OVERRIDE;

    /**
     * @brief Returns datamodel object based on given object uuid. If no
     * object is found nullpointer is returned.
     * @tparam T Object type
     * @param uuid Object uuid string
     * @return Object corresponding to given uuid
     */
    template <class T>
    T data(QString uuid)
    {
        foreach (QPointer<GtObject> p, m_linkedObjects)
        {
            if (p == Q_NULLPTR)
            {
                return Q_NULLPTR;
            }

            GtObject* obj = p.data();

            if (obj == Q_NULLPTR)
            {
                continue;
            }

            if (obj->uuid() == uuid)
            {
                return qobject_cast<T>(obj);
            }
        }

        gtInfo() << "obj not found - uuid =" << uuid;

        return Q_NULLPTR;
    }

    /**
     * @brief Returns datamodel object based on given object path. If no
     * object is found nullpointer is returned.
     * @tparam T Object type
     * @param path Object path
     * @return Object corresponding to given object path
     */
    template <class T>
    T data(const GtObjectPath& path)
    {
        foreach (QPointer<GtObject> p, m_linkedObjects)
        {
            GtObject* obj = p.data();

            if (obj == Q_NULLPTR)
            {
                continue;
            }

            if (obj->objectPath() == path.toString())
            {
                return qobject_cast<T>(obj);
            }
        }

        gtInfo() << "obj not found - path =" << path.toString();

        return Q_NULLPTR;
    }

    /**
     * @brief Returns datamodel object based on given object link property.
     * If no object is found nullpointer is returned.
     * @tparam T Object type
     * @param prop Object link property
     * @return Object corresponding to given object link property
     */
    template <class T>
    T data(GtObjectLinkProperty& prop)
    {
        const QString uuid = dataHelper(prop);
        return data<T>(uuid);
    }

    /**
     * @brief Returns datamodel object based on given object path property.
     * If no object is found nullpointer is returned.
     * @tparam T Object type
     * @param prop Object path property
     * @return Object corresponding to given object path property
     */
    template <class T>
    T data(GtObjectPathProperty& prop)
    {
        const GtObjectPath path = pathHelper(prop);

        return data<T>(path);
    }

protected:
    /**
     * @brief Constructor
     */
    GtCalculator();

    /// List of linked datamodel objects.
    QList<QPointer<GtObject>> m_linkedObjects;

    /// Delete temporary path indicator
    bool m_deleteTempPath;

    /**
     * @brief Returns string value of environment variable corresponding to
     * given identification string.
     * @param var Environment variable identification string
     * @return String value of environment variable
     */
    QString environmentVariable(const QString& var);

    /**
     * @brief Hides or Shows the label property.
     * @param val If true, the label property will be hidden, otherwise it
     * will be displayed.
     */
    void hideLabelProperty(bool val = true);

    /**
     * @brief Returns path of current project.
     * Note: only possible when calculator is already running.
     * @return Pasth of current project.
     */
    QString projectPath();

private:
    /// Execution mode indicator.
    GtModeProperty* m_execMode;

    /// Execution label property
    QPointer<GtLabelProperty> m_labelProperty;

    /// Path to process/project specific temporary path.
    QString m_tempPath;

    /// Pointer to runnable associated to the calculator.
    GtAbstractRunnable* m_runnable;

    /// Skip indicator.
    GtBoolProperty m_skipped;

    /// Fail run on warning indicator.
    GtBoolProperty m_failRunOnWarning;

    /**
     * @brief Returns uuid string of given object link property.
     * @param prop Object link property.
     * @return Uuid string.
     */
    QString dataHelper(GtObjectLinkProperty& prop);

    /**
     * @brief Returns object path of given object path property.
     * @param prop Object path property.
     * @return Object path.
     */
    GtObjectPath pathHelper(GtObjectPathProperty& prop);

};

Q_DECLARE_METATYPE(GtCalculator*)

#endif // GTCALCULATOR_H
