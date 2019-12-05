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
     */
    GtObjectLinkProperty(const QString& ident,
                         const QString& name,
                         const QString& brief,
                         const QString& uuid,
                         GtObject* obj,
                         const QStringList& allowedClasses);

    /**
     * @brief Overloaded function to convert internal property value to
     * QVariant. Value is converted based on given unit.
     * @param unit Unit into which the value is to be converted.
     * @param success Whether conversion was successfull or not.
     * @return Value as QVariant
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief Overloaded function to set internal property value from given
     * QVariant. Value of QVariant is converted based on given unit.
     * @param val New value in form of QVariant.
     * @param unit Unit of the input value.
     * @param success Whether the value could be set or not
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = 0) Q_DECL_OVERRIDE;

    /**
     * @brief linkedObject
     * @return
     */
    GtObject* linkedObject(GtObject* root = Q_NULLPTR);

    /**
     * @brief object
     * @return
     */
    GtObject* object();

    /**
     * @brief linkedObjectUUID
     * @return
     */
    QString linkedObjectUUID() const;

    /**
     * @brief allowedClasses
     * @return
     */
    const QStringList& allowedClasses();

protected:
    /// Pointer to parent object.
    QPointer<GtObject> m_obj;

    /// List of all allowed class names.
    QStringList m_allowedClasses;

};

#endif // GTOBJECTLINKPROPERTY_H
