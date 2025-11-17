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

#include <QDomDocument>
#include <QByteArray>
#include <QVariant>

#include "gt_object.h"
#include "gt_logging.h"
#include "gt_logging/qt_bindings.h"

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
     * @brief GtObjectMemento
     * @param obj
     * @param clone
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
     * @brief restore
     * @param factory
     * @param newUuid
     * @return
     */
    template <class T = GtObject*>
    T restore(GtAbstractObjectFactory* factory, bool newUuid = false)
    {
        T retval = nullptr;

        if (factory)
        {
            auto tmp = toObject(*factory).release();

            if (tmp)
            {
                if (newUuid)
                {
                    tmp->newUuid(true);
                }

                retval = qobject_cast<T>(tmp);

                if (!retval)
                {
                    gtWarning() << QObject::tr("wrong object type!")
                                << QStringLiteral("(") << ident()
                                << QStringLiteral(")");
                    delete tmp;
                }
            }
            else
            {
                gtWarning() << QObject::tr("object not properly restored!")
                            << QStringLiteral("(") << ident()
                            << QStringLiteral(")");
            }
        }
        else
        {
            gtFatal() << QObject::tr("no factory set!")
                      << QStringLiteral("(") << className()
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
        SaveAsOwnFile = 1
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
