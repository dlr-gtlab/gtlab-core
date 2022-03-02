/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectpathproperty.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.12.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_OBJECTPATHPROPERTY_H
#define GT_OBJECTPATHPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

#include <QPointer>
#include <QString>

class GtObject;
class GtObjectPath;

/**
 * @brief The GtObjectPathProperty class
 */
class GT_DATAMODEL_EXPORT GtObjectPathProperty : public GtProperty<QString>
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param ident Property memento identification string.
     * @param name Property display identification string.
     * @param brief Property description.
     * @param uuid Path to object.
     * @param obj Parent object.
     * @param allowedClasses List of allowed class names.
     */
    GtObjectPathProperty(const QString& ident,
                         const QString& name,
                         const QString& brief,
                         const QString& path,
                         GtObject* obj,
                         const QStringList& allowedClasses);

    /**
     * @brief valueToVariant
     * @return
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = nullptr) const override;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = nullptr) override;

    /**
     * @brief Returns object based on stored object path. Returns null pointer
     * if path is empty or invalid.
     * @param root Root objectt for search.
     * @return linked object.
     */
    GtObject* linkedObject(GtObject* root = nullptr);

    /**
     * @brief Returns stored root object.
     * @return Root object.
     */
    GtObject* object();

    /**
     * @brief Returns allowed object classes.
     * @return List of allowed object classes.
     */
    const QStringList& allowedClasses();

    /**
     * @brief Returns object path.
     * @return Object path.
     */
    GtObjectPath path() const;

private:
    /// Pointer to parent object.
    QPointer<GtObject> m_obj;

    /// List of all allowed class names.
    QStringList m_allowedClasses;

};

#endif // GT_OBJECTPATHPROPERTY_H
