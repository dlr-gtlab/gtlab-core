/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

#include "gt_object.h"
#include "gt_logging.h"

class GtAbstractObjectFactory;
class GtObjecIO;
class QCryptographicHash;
class VariantHasher;

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

    struct MementoData;
    explicit GtObjectMemento(const GtObjectMemento::MementoData&);

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
    [[deprecated ("Use GtObjectIO::toDomElement instead.")]]
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

    [[deprecated ("Use mergeTo using references instead of pointers")]]
    bool mergeTo(GtObject* obj, GtAbstractObjectFactory* factory) const;

    /**
     * @brief className
     * @return
     */
    const QString& className() const;

    /**
     * @brief uuid
     * @return
     */
    const QString& uuid() const;

    /**
     * @brief ident
     * @return
     */
    const QString& ident() const;

    /**
     * @brief canCastTo
     * @param className
     * @return
     */
    bool canCastTo(const QString& classname, GtAbstractObjectFactory* factory);

    /**
     * @brief internal data structure for storing GtObject data
     */
    struct MementoData
    {
        const GtObjectMemento* findChild(const QString& ident) const;


        QString className, uuid, ident;

        struct PropertyData
        {
            QString name;
            bool isOptional = false;
            bool isActive = true;
            QString dynamicObjectName;

            enum PropertyType
            {
                DATA_T,
                DYNCONT_T,
                ENUM_T // only used by meta properties
            };

            const QVariant& data() const
            {
                return _data;
            }

            GT_DATAMODEL_EXPORT
            PropertyData& setData(const QVariant& val);

            static PropertyData
            makeDynamicContainer(const QString& dynamicObjectName);

            static PropertyData
            makeDynamicChild(const QVariant& value,
                             const QString& dynamicObjectName,
                             const QString& dynamicTypeName);

            const QString& dataType() const
            {
                return _dataType;
            }

            const PropertyType& type() const
            {
                return _type;
            }

            PropertyData& fromQMetaProperty(const QMetaProperty& prop,
                                            const QVariant& val);


            QVector<PropertyData> childProperties; /// sub properties
            mutable QByteArray hash;

        private:
            QVariant _data;    /// The data as a variant
            QString _dataType; /// The type of the data
            PropertyType _type  {DATA_T};

        };
        QVector<PropertyData> properties;
        
        QVector<GtObjectMemento> childObjects;
    };
    /**
     * @brief directly access memento data
     */
    const MementoData& data() const {return m_data;}

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

private:
    /**
     * @brief internal data that represents a GtObject
     */
    MementoData m_data;

    /**
     * @brief cached hashes of a GtObject (properties only) and the full GtObject (including all its children)
     */
    mutable QByteArray m_propertyHash, m_fullHash;

    void propertyHashHelper(const MementoData::PropertyData& property, QCryptographicHash& hash, VariantHasher& variantHasher) const;


};

#endif // GTOBJECTMEMENTO_H
