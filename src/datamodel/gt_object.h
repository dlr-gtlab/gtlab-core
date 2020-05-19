/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECT_H
#define GTOBJECT_H

#include "gt_datamodel_exports.h"

#include <QObject>

class QSignalMapper;
class GtObjectMemento;
class GtAbstractObjectFactory;
class GtAbstractProperty;
class GtObjectIO;
class GtResult;
class GtDataZoneTable;
class GtObjectMementoDiff;

#define GT_CLASSNAME(A) A::staticMetaObject.className()
#define GT_METADATA(A) A::staticMetaObject

/**
 * @brief The GtObject class
 */
class GT_DATAMODEL_EXPORT GtObject : public QObject
{
    Q_OBJECT

    friend class GtObjectIO;

public:
    enum ObjectFlag
    {
        HasOwnChanges = 1,
        HasChildChanges = 2,
        NewlyCreated = 4,
        UserDeletable = 8,
        UserRenamable = 16,
        DefaultComponent = 32,
        UserHidden = 64
    };
    Q_DECLARE_FLAGS(ObjectFlags, ObjectFlag)

    /**
     * @brief GtObject
     * @param parent
     */
    GtObject(GtObject* parent = NULL);

    /**
     * @brief objectFlags
     * @return
     */
    GtObject::ObjectFlags objectFlags();

    /**
     * @brief Returns true if object is a dummy. Otherwise false is returned.
     * @return True if object is a dummy.
     */
    bool isDummy();

    /**
     * @brief Returns true if object has children of type dummy.
     * Otherwise false is returned.
     * @return True if object has children of type dummy.
     */
    bool hasDummyChildren();

    /**
     * @brief Creates a memento of the internal object state.
     * @param clone
     * @return
     */
    GtObjectMemento toMemento(bool clone = true);

    /**
     * @brief fromMemento
     * @param memento
     */
    void fromMemento(const GtObjectMemento& memento);

    /**
     * @brief applyDiff
     * @param diff
     * @return
     */
    bool applyDiff(GtObjectMementoDiff& diff);

    /**
     * @brief revertDiff
     * @param diff
     * @return
     */
    bool revertDiff(GtObjectMementoDiff& diff);

    /**
     * @brief copy
     * @return
     */
    GtObject* copy();

    //    template <class T = GtObject*>
    //    T copy()
    //    {
    //        // check for factory
    //        if (m_factory == NULL)
    //        {
    //            return NULL;
    //        }

    //        // generate memento
    //        GtObjectMemento memento = toMemento(false);

    //        if (memento.isNull())
    //        {
    //            return NULL;
    //        }

    //        return memento.restore<T>(m_factory);
    //    }

    /**
     * @brief clone
     * @return
     */
    GtObject* clone();

    //    template <class T = GtObject*>
    //    T clone()
    //    {
    //        // check for factory
    //        if (m_factory == NULL)
    //        {
    //            return NULL;
    //        }

    //        // generate memento
    //        GtObjectMemento memento = toMemento(true);

    //        if (memento.isNull())
    //        {
    //            return NULL;
    //        }

    //        return memento.restore<T>(m_factory);
    //    }


    /**
     * @brief appendChild - appends the object as child. old relationship is
     * destroyed
     * @param c child object
     * @return whether object has been appended or not
     */
    bool appendChild(GtObject* c);

    /**
     * @brief appendChildren
     * @param list
     * @return
     */
    bool appendChildren(const QList<GtObject*>& list);

    /**
     * @brief Disconnects all data changed signals/slots and set parent to
     * nullptr.
     */
    void disconnectFromParent();

    /** Returns uuid of component.
        @return UUID */
    QString uuid() const;

    /** Sets new uuid for component.
        @param val New UUID */
    void setUuid(const QString& val);

    /**
     * @brief Generates new component uuid.
     */
    void newUuid(bool renewChildUUIDs = false);

    /**
     * @brief Calculates object specific hash.
     * @return Object specific hash.
     */
    QString calcHash();

    /**
     * @brief isDefault
     * @return
     */
    bool isDefault();

    /**
     * @brief setDefault
     * @param val
     */
    void setDefault(bool val);

    /**
     * @brief Returns true if user hidden flag is active. Otherwise false is
     * returned.
     * @return User hidden flag state.
     */
    bool isUserHidden();

    /**
     * @brief Sets user hidden flag by given value.
     * @param val New user hidden flag state.
     */
    void setUserHidden(bool val);

    /**
     * @brief isRenamable
     * @return
     */
    bool isRenamable();

    /**
     * @brief Returns true if object is user deletable and not default,
     * otherwise returns false.
     * @return Deletable flag
     */
    bool isDeletable();

    /**
     * @brief setFactory
     * @param factory
     */
    void setFactory(GtAbstractObjectFactory* factory);

    /**
     * @brief Returns the position index from the parent child list
     * @return position index
     */
    int childNumber();

    /**
     * @brief Returns the parent automatically casted to GtObject. Returns
     * NULL if no parent exists or parent is not a GtObject.
     * @return parent object
     */
    GtObject* parentObject();

    /**
     * @brief results
     * @return
     */
    GtResult* results();

    /**
     * @brief Returns all label identification strings stored in results of
     * object and all child objects.
     * @return List of label identification strings.
     */
    QStringList labelIds();

    /**
    * @brief collectDzt
    * @return
    */
    virtual GtDataZoneTable* createDzt(bool* ok = Q_NULLPTR)
    {
        Q_UNUSED(ok);
        return Q_NULLPTR;
    }

    /**
     * @brief hasChanges
     * @return
     */
    bool hasChanges();

    /**
     * @brief hasChildChanged
     * @return
     */
    bool hasChildChanged();

    /**
     * @brief newlyCreated
     * @return
     */
    bool newlyCreated();

    /**
     * @brief acceptChanges
     */
    void acceptChanges();

    /**
     * @brief acceptChangesRecursively
     */
    void acceptChangesRecursively();

    /**
     * @brief debugObjectTree
     * @param indent
     */
    void debugObjectTree(int indent = 0);

    /**
     * @brief Returns list of root properties.
     * @return List of root properties.
     */
    const QList<GtAbstractProperty*>& properties() const;

    /**
     * @brief Returns list of all property and sub property items. List
     * does not contain duplicates.
     * @return List of all properties.
     */
    QList<GtAbstractProperty*> fullPropertyList() const;

    /**
     * @brief findProperty
     * @param id
     * @return
     */
    Q_INVOKABLE GtAbstractProperty* findProperty(const QString& id);

    /**
     * @brief findProperty
     * @param name
     * @return
     */
    GtAbstractProperty* findPropertyByName(const QString& name);

    /**
     * @brief Returns all properties of type T
     * @param Type
     * @return
     */
    template <class T>
    QList<T> propertiesByType()
    {
        QList<T> retVal;

        foreach (GtAbstractProperty* absProp, properties())
        {
            //qDebug() << absProp;

            if (T prop = qobject_cast<T>(absProp))
            {
                retVal.append(prop);
            }
        }

        return retVal;
    }

    /**
     * @brief Returns child count
     * @param name
     * @return
     */
    template <class T>
    int childCount(const QString& name = QString()) const
    {
        QList<T> list = findDirectChildren<T>(name);

        return list.size();
    }

    /**
     * @brief findParent
     * @param name
     * @return
     */
    template <class T>
    T findParent(const QString& name = QString())
    {
        if (parent() != NULL)
        {
            T temp = qobject_cast<T>(parent());

            if (temp && (name.isNull() || name == temp->objectName()))
            {
                return temp;
            }

            GtObject* o = qobject_cast<GtObject*>(parent());

            if (o)
            {
                return o->findParent<T>(name);
            }
        }

        return NULL;
    }

    /**
     * @brief findRoot
     * @param name
     * @return
     */
    template <class T>
    T findRoot(T last = Q_NULLPTR)
    {
        GtObject* pObj = parentObject();

        if (pObj != NULL)
        {
            last = qobject_cast<T>(pObj);

            T temp = pObj->findRoot<T>(last);

            if (temp != Q_NULLPTR)
            {
                return temp;
            }
        }

        return last;
    }

    /**
     * @brief findDirectChildren
     * @param name
     * @param sorted
     * @return
     */
    template <class T>
    QList<T> findDirectChildren(const QString& name = QString()) const
    {
        return findChildren<T>(name, Qt::FindDirectChildrenOnly);
    }

    /**
     * @brief findDirectChild
     * @param name
     * @return
     */
    template <class T>
    T findDirectChild(const QString& name = QString()) const
    {
        return findChild<T>(name, Qt::FindDirectChildrenOnly);
    }

    /**
     * @brief insertChild
     * @param pos
     * @param obj
     * @return
     */
    template <class T>
    bool insertChild(int pos, T obj)
    {
        if (pos < 0)
        {
            return false;
        }

        QList<T> list = findDirectChildren<T>();

        if (pos > list.size())
        {
            return false;
        }

        if (pos == list.size())
        {
            return appendChild(obj);
        }

        QList<T> elementsBehindPos;

        for (int i = pos; i < list.size(); ++i)
        {
            elementsBehindPos.append(list.at(i));
            list.at(i)->setParent(Q_NULLPTR);
        }

        appendChild(obj);

        foreach (T element, elementsBehindPos)
        {
            element->setParent(this);
        }

        return true;
    }

    /**
     * @brief getObjectByUuid
     * @param uuid
     * @return
     */
    GtObject* getObjectByUuid(const QString& objectUUID);

    /**
     * @brief getDirectChildByUuid
     * @param objectUUID
     * @return
     */
    GtObject* getDirectChildByUuid(const QString& objectUUID);

    /**
     * @brief getObjectByPath
     * @param objectPath
     * @return
     */
    GtObject* getObjectByPath(const QString& objectPath);

    /**
     * @brief getObjectByPath
     * @param objectPath
     * @return
     */
    GtObject* getObjectByPath(QStringList& objectPath);

    /**
     * @brief objectPath
     * @return
     */
    QString objectPath();

    /**
     * @brief setFlag
     * @param enable
     */
    void setFlag(GtObject::ObjectFlag flag, bool enable = true);

    /**
     * @brief factory
     * @return
     */
    GtAbstractObjectFactory* factory();

    //public slots:
    //    void changed();

protected:
    ///
    //    bool m_renamable;

    /**
     * @brief registerProperty
     * @param property
     */
    bool registerProperty(GtAbstractProperty& property);

    /**
     * @brief registerProperty
     * @param property
     * @param cat
     */
    bool registerProperty(GtAbstractProperty& property, const QString& cat);

    /**
     * @brief Called after successfully restoring from memento
     */
    virtual void onObjectDataMerged();

    /**
     * @brief childAccepted
     * @param child
     * @return
     */
    virtual bool childAccepted(GtObject* child);

protected slots:
    /**
     * @brief changed
     */
    void changed();

private:
    /// Object specific uuid
    QString m_uuid;

    /// Object flags
    GtObject::ObjectFlags m_objectFlags;

    ///
    //    bool m_default;

    /// factory
    GtAbstractObjectFactory* m_factory;

    /// object properties
    QList<GtAbstractProperty*> m_properties;

    /// mapper for property signals
    QSignalMapper* m_propertyMapper;

    /**
     * @brief objectPath
     * @return
     */
    QString objectPath(QString& str);

    /**
     * @brief connectProperty
     * @param property
     */
    void connectProperty(GtAbstractProperty& property);

    /**
     * @brief fullPropertyListHelper
     * @param p
     * @return
     */
    void fullPropertyListHelper(GtAbstractProperty* p,
                                QList<GtAbstractProperty*>& list) const;

    /**
     * @brief renews child uuids recursively for all child objects
     * @param parent parent object
     */
    void newChildUUIDs(GtObject* parent);

private slots:
    /**
     * @brief emit a signal if a property is changed
     * @param object
     */
    void propertyChanged(QObject* obj);

    /**
     * @brief onChildDataChanged
     */
    void onChildDataChanged();


signals:
    /**
     * @brief dataChanged
     */
    void dataChanged(GtObject*);

    /**
     * @brief dataChanged
     */
    void dataChanged(GtObject*, GtAbstractProperty*);

    /**
     * @brief childAppended
     * @param child
     * @param parent
     */
    void childAppended(GtObject* child, GtObject* parent);

};

typedef QList<GtObject*> GtObjectList;

Q_DECLARE_METATYPE(GtObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(GtObject::ObjectFlags)

#endif // GTOBJECT_H
