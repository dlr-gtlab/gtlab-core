/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROPERTY_H
#define GTABSTRACTPROPERTY_H

#include "gt_datamodel_exports.h"
#include "gt_platform.h"
#include "gt_unit.h"

#include <QObject>
#include <QPointer>
#include <QVariant>

#include "gt_propertyconnection.h"

/**
 * @brief The GtAbstractProperty class
 */
class GT_DATAMODEL_EXPORT GtAbstractProperty : public QObject
{
    Q_OBJECT

public:
    enum PropertyCategory
    {
        Main,
        Custom
    };

    ~GtAbstractProperty();

    /**
     * @brief valueAsVariant
     * @return
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const = 0;

    /**
     * @brief valueToVariant
     * @return
     */
    Q_INVOKABLE QVariant valueToVariant() const;

    /**
     * @brief Gets and cast the value into type T
     * @return The value of type T
     */
    template <typename T>
    T value() const
    {
        return valueToVariant().value<T>();
    }

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    Q_INVOKABLE bool setValueFromVariant(const QVariant& val);

    /**
     * @brief setValueFromVariant
     * @param val
     * @param unit
     * @param success
     * @return
     */
    GT_NO_DISCARD
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit) = 0;

    /**
     * @brief Retrieves the brief description of the parameter
     * @return
     */
    virtual QString brief() const;

    /**
     * @brief Retrieves the SI unit of the parameter
     * @return
     */
    virtual QString siUnit() const;

    /**
     * @brief Retrieves the unit category of the parameter as string
     * @return
     */
    virtual QString unitCategoryToString() const;

    /**
     * @brief Retrieves the unit category of the parameter
     * @return
     */
    GtUnit::Category unitCategory() const;

    /**
     * @brief category
     * @return
     */
    GtAbstractProperty::PropertyCategory category() const;

    /**
     * @brief setCategory
     * @param cat
     */
    void setCategory(GtAbstractProperty::PropertyCategory cat);

    /**
     * @brief setCategory
     * @param cat
     */
    void setCategory(const QString& cat);

    /**
     * @brief categoryToString
     * @param cat
     * @return
     */
    QString categoryToString();

    /**
     * @brief properties
     * @return
     */
    virtual const QList<GtAbstractProperty*>& properties() const;

    /**
     * @brief fullProperties
     * @return
     */
    const QList<GtAbstractProperty*>& fullProperties() const;

    /**
     * @brief registerSubProperty
     * @param property
     */
    virtual void registerSubProperty(GtAbstractProperty& property);

    /**
     * @brief insertSubProperty
     * @param property
     */
    void insertSubProperty(GtAbstractProperty& property, int index);

    /**
     * @brief isReadOnly
     * @return
     */
    bool isReadOnly();

    /**
     * @brief setReadOnly
     * @param val
     */
    void setReadOnly(bool val);

    /**
     * @brief Returns the is active flag. This flag is only relevant for
     * properties that are optional.
     * @return Returns whether the property is active or not
     */
    bool isActive() const;

    /**
     * @brief Setter for the is active flag. This flag is only relevant for
     * properties that are optional.
     * @param val
     */
    void setActive(bool val);

    /**
     * @brief Returns true if property is marked as hidden.
     * @return Returns whether property is hidden or not.
     */
    bool isHidden() const;

    /**
     * @brief Marks the property as hidden.
     * @param val New hiden indicator.
     */
    void hide(bool val = true);

    /**
     * @brief isOptional
     * @return
     */
    bool isOptional() const;

    /**
     * @brief Sets optional flag of the property. If val = true property
     * is set inactive.
     * @param val
     */
    void setOptional(bool val);

    /**
     * @brief Returns number of sub properties
     * @return
     */
    int propertyCount() const;

    /**
     * @brief findProperty
     * @param id
     * @return
     */
    GtAbstractProperty* findProperty(const QString& id);
    GtAbstractProperty const * findProperty(const QString& id) const;

    GtAbstractProperty* findPropertyByName(const QString& name);

    /**
     * @brief Returns false if identification string is empty, otherwise
     * returns true.
     * @return Property validation.
     */
    bool isValid() const;

    /**
     * @brief Returns identification string of property.
     * @return Identification string.
     */
    const QString& ident() const;

    /**
     * @brief storeToMemento
     * @return
     */
    bool storeToMemento() const;

    /**
     * @brief revert
     */
    virtual void revert();

    /**
     * @brief Connects property by given connection.
     * @param Property connection.
     */
    void propertyConnect(GtPropertyConnection* connection);

    /**
     * @brief Clears connection.
     */
    void clearConnection();

    /**
     * @brief Returns true if property is connected, otherwise returns false.
     * @return Whether property is connected or not.
     */
    bool isConnected();
    bool isConnected() const;

    /**
     * @brief isMonitoring
     * @return true if property is initialized as monitoring
     */
    bool isMonitoring() const;

    void setMonitoring(bool monitoring);
protected:
    /**
     * @brief GtAbstractProperty
     */
    GtAbstractProperty();

    /// Identification string
    QString m_id;

    /// Brief description of parameter
    QString m_brief;

    /// Custom category string
    QString m_customCategoryString;

    /// Unit Category of the parameter
    GtUnit::Category m_unitCategory{GtUnit::None};

    /// List of all sub properties
    QList<GtAbstractProperty*> m_subProperties;

    /// Read only indicator
    bool m_readOnly{false};

    /// Optional indicator
    bool m_optional{false};

    /// Active indicator
    bool m_active{true};

    /// Hide indicator
    bool m_hidden{false};

    /// Whether property is stored in memento information or not
    bool m_storeMemento{true};

    /// Property category
    GtAbstractProperty::PropertyCategory m_category{GtAbstractProperty::Main};

    /// Pointer to property connection
    QPointer<GtPropertyConnection> m_connection{nullptr};

    /**
     * @brief setValFromConnection
     */
    virtual void setValFromConnection();

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

private slots:
    /**
     * @brief triggerForceSetVal
     */
//    void triggerForceSetVal();

    void onTriggerValueTransfer();

signals:
    /**
     * @brief changed
     */
    void changed();

    /**
     * @brief This is triggered if any of the sub properties is changed
     */
    void subPropChanged(GtAbstractProperty* p);

};

Q_DECLARE_METATYPE(GtUnit::Category)

namespace gt
{

GT_DATAMODEL_EXPORT
QVariant getConnectedValue(const GtPropertyConnection& connection);

/**
     * Function definition to create a property with the
     * identifier 'id'
     *
     * This is mainly used to recreate types in property container.
     *
     * Note: we currently omit the "name" argument, as name is used
     * to store the dynamic type of the struct
     */
using PropertyFactoryFunction =
    std::function<GtAbstractProperty*(const QString& id)>;

} // namespace gt

#endif // GTABSTRACTPROPERTY_H
