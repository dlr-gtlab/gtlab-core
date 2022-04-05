/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROPERTY_H
#define GTABSTRACTPROPERTY_H

#include "gt_datamodel_exports.h"
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
    virtual bool setValueFromVariant(const QVariant& val, const QString& unit,
                                     bool* success = 0) = 0;

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
    virtual const QList<GtAbstractProperty*>& properties();

    /**
     * @brief fullProperties
     * @return
     */
    const QList<GtAbstractProperty*>& fullProperties();

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
     * @brief isActive
     * @return
     */
    bool isActive() const;

    /**
     * @brief setActive
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

protected:
    /**
     * @brief GtAbstractProperty
     */
    GtAbstractProperty() :
        m_connection(nullptr)
    {
        m_readOnly = false;
        m_optional = false;
        m_active = true;
        m_hidden = false;
        m_storeMemento = true;
        m_category = GtAbstractProperty::Main;
        m_unitCategory = GtUnit::None;
    }

    /// Identification string
    QString m_id;

    /// Brief description of parameter
    QString m_brief;

    /// Custom category string
    QString m_customCategoryString;

    /// Unit Category of the parameter
    GtUnit::Category m_unitCategory;

    /// List of all sub properties
    QList<GtAbstractProperty*> m_subProperties;

    /// Read only indicator
    bool m_readOnly;

    /// Optional indicator
    bool m_optional;

    /// Active indicator
    bool m_active;

    /// Hide indicator
    bool m_hidden;

    /// Whether property is stored in memento information or not
    bool m_storeMemento;

    /// Property category
    GtAbstractProperty::PropertyCategory m_category;

    /// Pointer to property connection
    QPointer<GtPropertyConnection> m_connection;

    /**
     * @brief setValFromConnection
     */
    virtual void setValFromConnection();

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

};

Q_DECLARE_METATYPE(GtUnit::Category)



#endif // GTABSTRACTPROPERTY_H