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
     * @brief isRestorable
     * @param factory
     * @return
     */
    bool isRestorable(GtAbstractObjectFactory* factory);

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
            GtObject* tmp = createObject(factory);

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
     * @brief mergeTo
     * @param obj
     * @param factory
     * @return
     */
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
        QString className, uuid, ident;

        struct PropertyData
        {
            QString name;
            bool isOptional = false;
            bool isActive = true;
            bool isDynamicContainer = false;
            QString dynamicClassName;
            QString dynamicObjectName;
            QString enumType;
            QVariant data;
            QVector<PropertyData> childProperties;
            mutable QByteArray hash;
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
     * @brief cached XML document
     */
    mutable QDomDocument m_domDocument;

    /**
     * @brief internal data that represents a GtObject
     */
    MementoData m_data;

    /**
     * @brief cached hashes of a GtObject (properties only) and the full GtObject (including all its children)
     */
    mutable QByteArray m_propertyHash, m_fullHash;

    void propertyHashHelper(const MementoData::PropertyData& property, QCryptographicHash& hash, VariantHasher& variantHasher) const;

    /**
     * @brief attribute
     * @param id
     * @return
     */
    QString attribute(const QString& id) const;

    /**
     * @brief createObject
     * @param factory
     * @return
     */
    GtObject* createObject(GtAbstractObjectFactory* factory);

    /**
     * @brief isRestorable
     * @param factory
     * @param element
     * @return
     */
    bool isRestorable(GtAbstractObjectFactory* factory,
                      const QDomElement& element);

};

#endif // GTOBJECTMEMENTO_H
