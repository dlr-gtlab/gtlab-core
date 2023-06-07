/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectlinkproperty.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTOBJECTLINKPROPERTY_H
#define GTOBJECTLINKPROPERTY_H

#include "gt_datamodel_exports.h"

#include <QPointer>
#include <QString>

#include "gt_property.h"

class GtObject;

/**
 * @brief The GtObjectLinkProperty class
 */
class GT_DATAMODEL_EXPORT GtObjectLinkProperty : public GtProperty<QString>
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param ident Memento identification string.
     * @param name Visible property identification string.
     * @param brief Property description.
     * @param uuid Initial uuid.
     * @param obj Source object.
     * @param allowedClasses List of allowed classes.
     * @param linkFromSuperClassesEnabled mode if objects of classes
     *  which inherit allowed classes should be allowed classes too
     */
    GtObjectLinkProperty(const QString& ident,
                         const QString& name,
                         const QString& brief,
                         const QString& uuid,
                         GtObject* obj,
                         QStringList allowedClasses,
                         bool linkFromSuperClassesEnabled = false);

    /**
     * @brief Constructor.
     * @param ident Memento identification string.
     * @param name Visible property identification string.
     * @param brief Property description.
     * @param obj Source object.
     * @param allowedClasses List of allowed classes.
     * @param linkFromSuperClassesEnabled mode if objects of classes
     * which inherit allowed classes should be allowed classes too
     *
     * The initial value is an empty string
     */
    GtObjectLinkProperty(const QString& ident,
                         const QString& name,
                         const QString& brief,
                         GtObject* obj,
                         const QStringList& allowedClasses,
                         bool linkFromSuperClassesEnabled = false);

    /**
     * @brief Overloaded function to convert internal property value to
     * QVariant. Value is converted based on given unit.
     * @param unit Unit into which the value is to be converted.
     * @param success Whether conversion was successfull or not.
     * @return Value as QVariant
     */
    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override;

    /**
     * @brief Overloaded function to set internal property value from given
     * QVariant. Value of QVariant is converted based on given unit.
     * @param val New value in form of QVariant.
     * @param unit Unit of the input value.
     * @param success Whether the value could be set or not
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(const QVariant& val,
                             const QString& unit) override;

    /**
     * @brief linkedObject
     * @return
     */
    GtObject* linkedObject(GtObject* root = nullptr);

    /**
     * @brief object
     * @return
     */
    GtObject* object() const;

    /**
     * @brief linkedObjectUUID
     * @return
     */
    QString linkedObjectUUID() const;

    /**
     * @brief allowedClasses
     * @return
     */
    const QStringList& allowedClasses() const;

    /**
     * @brief Sets the list of allowed classes
     * @param allowedClasses Allowed classes
     */
    void setAllowedClasses(QStringList allowedClasses);

    /**
     * @brief linkFromSuperClass
     * @return true if classes which inherit
     * allowed classes should be allowed, too
     */
    bool linkFromSuperClass() const;

protected:
    /// Pointer to parent object.
    QPointer<GtObject> m_obj;

    /// List of all allowed class names.
    QStringList m_allowedClasses;

    bool m_linkFromSuperClassesEnabled;

};

#endif // GTOBJECTLINKPROPERTY_H
