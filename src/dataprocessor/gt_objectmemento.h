/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTMEMENTO_H
#define GTOBJECTMEMENTO_H

#include "gt_datamodel_exports.h"
#include <gt_version.h>

#include <QDomDocument>
#include <QByteArray>
#include <QVariant>

#include <memory>
#include <type_traits>

#include "gt_object.h"
#include "gt_logging.h"
#include "gt_logging/qt_bindings.h"
#include "gt_qtutilities.h"

class GtAbstractObjectFactory;
class GtObjecIO;
class QCryptographicHash;
class VariantHasher;
class GtPropertyStructInstance;

/**
 * @brief The GtObjectMemento class
 */
class GT_DATAMODEL_EXPORT GtObjectMemento
{
public:
    /**
     * @brief Creates a memento of a GtObject
     * @param obj Object to serialize, or nullptr for a null memento
     * @param clone Whetherthe preserve UUIDs (true) or generate new ones
     * (false)
     */
    explicit GtObjectMemento(const GtObject* obj = nullptr, bool clone = true);

    /**
     * @brief GtObjectMemento
     * @param element
     */
    explicit GtObjectMemento(const QDomElement& element);

    /**
     * @brief GtObjectMemento
     * @param byteArray
     */
    explicit GtObjectMemento(const QByteArray& byteArray);

    /**
     * @brief interface from QDomElement
     */
    bool isNull() const;

    /**
     * @brief interface from QDomElement
     */
    QDomElement documentElement() const;

    /**
     * @brief interface from QDomElement
     */
    QByteArray toByteArray() const;

    /**
     * TODO: move to object io
     *
     * @brief isRestorable
     * @param factory
     * @return
     */
    bool isRestorable(GtAbstractObjectFactory* factory) const;

    /**
     * @brief Restores a new object from the memento data
     * @param factory Object factory used to create the object instance
     * @param newUuid Whether to generate new UUIDs for the restored objects
     * @return Raw pointer to the restored object or nullptr, if the object
     * could not be restored. Ownership of the object is transferred to the
     * caller, who has to delete it. Use @ref restore_unique instead, which
     * returns a std::unique_ptr and cannot leak.
     */
    GT_REMOVAL_GUARD(2, 2, "Use `restore_unique()` instead.");
    template <class T = GtObject*>
    GT_DEPRECATED_ATTR(2, 2,
                       "Use `restore_unique()`, which returns a "
                       "std::unique_ptr instead of a raw pointer, "
                       "to prevent memory leaks.")
    T restore(GtAbstractObjectFactory* factory, bool newUuid = false)
    {
        return restore_unique<std::remove_pointer_t<T>>(factory, newUuid)
            .release();
    }

    /**
     * @brief Restores a new object from the memento data
     * @param factory Object factory used to create the object instance
     * @param newUuid Whether to generate new UUIDs for the restored objects
     * @return Unique pointer to the restored object or nullptr, if the object
     * could not be restored
     */
    template <class T = GtObject>
    std::unique_ptr<T> restore_unique(GtAbstractObjectFactory* factory,
                                      bool newUuid = false)
    {
        std::unique_ptr<T> retval{};

        if (!factory)
        {
            gtFatal() << QObject::tr("no factory set!")
                      << QStringLiteral("(") << className()
                      << QStringLiteral(")");
            return retval;
        }

        std::unique_ptr<GtObject> tmp = toObject(*factory);

        if (!tmp)
        {
            gtWarning() << QObject::tr("object not properly restored!")
                        << QStringLiteral("(") << ident()
                        << QStringLiteral(")");
            return retval;
        }

        if (newUuid)
        {
            tmp->newUuid(true);
        }

        retval = gt::unique_qobject_cast<T>(std::move(tmp));

        if (!retval)
        {
            gtWarning() << QObject::tr("wrong object type!")
                        << QStringLiteral("(") << ident()
                        << QStringLiteral(")");
        }

        return retval;
    }

    /**
     * @brief Creates a gtobject from the memento
     * @param factory An object factory to create object instances
     *
     * @return A pointer to an object or nullptr, if it could not be created.
     */
    std::unique_ptr<GtObject> toObject(GtAbstractObjectFactory& factory) const;

    /**
     * @brief mergeTo
     * @param obj
     * @param factory
     * @return
     */
    bool mergeTo(GtObject& obj, GtAbstractObjectFactory& factory) const;

    /**
     * @brief className
     * @return
     */
    const QString& className() const;
    GtObjectMemento& setClassName(const QString& className);

    /**
     * @brief uuid
     * @return
     */
    const QString& uuid() const;
    GtObjectMemento& setUuid(const QString& uuid);

    /**
     * @brief ident
     * @return
     */
    const QString& ident() const;
    GtObjectMemento& setIdent(const QString& ident);

    /**
     * @brief canCastTo
     * @param className
     * @return
     */
    bool canCastTo(const QString& classname, GtAbstractObjectFactory* factory);

    const GtObjectMemento* findChildByUuid(const QString& uuid) const;


    struct PropertyData
    {
        enum PropertyType
        {
            DATA_T,
            STRUCT_T,
            ENUM_T // only used by meta properties
        };

        QString name;
        bool isActive = true;

        const QVariant& data() const
        {
            return _data;
        }

        GT_DATAMODEL_EXPORT
        PropertyData& setData(const QVariant& val);

        const QString& dataType() const
        {
            return _dataType;
        }

        const PropertyType& type() const
        {
            return _type;
        }

        PropertyData& toStruct(const QString& structTypeName);

        PropertyData& fromQMetaProperty(const QMetaProperty& prop,
                                        const QVariant& val);


        QVector<PropertyData> childProperties; /// sub properties
        mutable QByteArray hash;

    private:
        QVariant _data;    /// The data as a variant
        QString _dataType; /// The type of the data
        PropertyType _type  {DATA_T};

    };

    /**
     * @brief Searches a property by its name
     * @param list The list to search for
     * @param name The property name
     * @return Pointer to the property or nullptr
     */
    static PropertyData const *
    findPropertyByName(const QVector<PropertyData>& list, const QString& name);

    /**
     * @brief get hash of this object's properties
     */
    const QByteArray& propertyHash() const {return m_propertyHash;}

    /**
     * @brief get hash of this object (including all child objects)
     */
    const QByteArray& fullHash() const {return m_fullHash;}

    /**
     * @brief update fullHash and propertyHash, needs to be called before accessing these
     */
    void calculateHashes() const;

    struct ExternalizationInfo
    {
        bool isFetched = true;
        QByteArray hash{};
        const QMetaObject* metaObject{};

        bool isValid() const { return metaObject != nullptr; }
    };

    ExternalizationInfo externalizationInfo(const GtAbstractObjectFactory& factory) const;

    enum Flag
    {
        SaveAsOwnFile = 1, /// The memento should be serialized into an own file
        IsUnresolved  = 2  /// Is enabled, if the memento data could not be fully
                           /// restored from disk, e.g. the linked file was not found
    };

    /**
     * @brief Checks whether the flag is enabled
     */
    bool isFlagEnabled(Flag) const;

    /**
     * @brief Sets or unsets the enabled state of a flag
     */
    void setFlagEnabled(Flag, bool enabled);

    QVector<PropertyData> properties;
    QVector<PropertyData> propertyContainers;
    QVector<GtObjectMemento> childObjects;

private:
    /**
     * @brief Creates a new object and adds it to the given parent
     *
     * @param factory An object factory to create object instances
     * @param parent Pointer to the parent object
     * @return A pointer to an object or nullptr, if it could not be created.
     */
    GtObject* toObject(GtAbstractObjectFactory& factory, GtObject* parent) const;

    QString m_className, m_uuid, m_ident;

    /**
     * @brief cached hashes of a GtObject (properties only) and the full GtObject (including all its children)
     */
    mutable QByteArray m_propertyHash, m_fullHash;

    /**
     * if true, the memento likes to be serialized into a separate file
     */
    int m_flags{0};

};

namespace gt
{

GT_DATAMODEL_EXPORT
void importStructEntryFromMemento(const GtObjectMemento::PropertyData& propStruct,
                                  GtPropertyStructInstance& structEntry);

} // namespace gt

#endif // GTOBJECTMEMENTO_H
