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
#include "gt_typetraits.h"

#include <QObject>

#include <algorithm>
#include <memory>

class GtObjectMemento;
class GtAbstractObjectFactory;
class GtAbstractProperty;
class GtObjectIO;
class GtObjectMementoDiff;
class GtPropertyStructContainer;

#define GT_CLASSNAME(A) A::staticMetaObject.className()
#define GT_METADATA(A) A::staticMetaObject

class GtObject;
namespace gt
{

/**
 * @brief Searchs for the parent of type T with the name specified
 * @param object Object to search parent in
 * @param name Name of the parent
 * @return Parent object pointer
 */
template <typename T = GtObject*,
          typename Object, // may be const
          trait::enable_if_ptr_derived_of_qobject<T> = true,
          trait::enable_if_base_of<QObject, std::decay_t<Object>> = true>
T findParent(Object& object, const QString& name = {});

/**
 * @brief Searchs for the root parent of type T
 * @param object Object to search root in
 * @param last Last root
 * @return Root object pointer
 */
template <typename T = GtObject*,
          typename Object, // may be const
          trait::enable_if_ptr_derived_of_qobject<T> = true,
          trait::enable_if_base_of<QObject, std::decay_t<Object>> = true>
T findRoot(Object& object, T last = nullptr);

/**
 * @brief Function to correctly move an object of class GtObject into a thread.
 * Unlike normal QObjects, the Property and PropertyContainer structures are
 * also moved into the thread.
 * @param object Object which schould be moved to given thread.
 * @param thread
 */
void GT_DATAMODEL_EXPORT moveToThread(GtObject& object, QThread* thread);

}

/**
 * @brief The GtObject class
 * The GtObject is the base datamodel class for all datamodel objects in GTlab.
 *
 * For correct functionality of the datamodel structure all objects should
 * be inherit from GtObject.
 */
class GT_DATAMODEL_EXPORT GtObject : public QObject
{
    Q_OBJECT

    friend class GtObjectIO;
    friend class GtObjectMemento;

public:
    /// Flags to describe objects state and otions
    enum ObjectFlag
    {
        /// the object has changes
        HasOwnChanges = 1,
        /// at least one child of the object has changes
        HasChildChanges = 2,
        /// the object is newly created
        NewlyCreated = 4,
        /// the object can be deleted in the GUI via the context menu
        UserDeletable = 8,
        /// the object can be renamed in the GUI via the context menu
        UserRenamable = 16,
        /// the component is a default component.
        /// This means it is checked while recreation if the object can be
        /// restored or otherwise is initialized again.
        DefaultComponent = 32,
        /// the object is not shown in the explorer widget in the GUI
        UserHidden = 64
    };
    Q_DECLARE_FLAGS(ObjectFlags, ObjectFlag)

    /**
     * @brief GtObject
     * Constructor sets basic flag for newly creation, sets a new uuid
     * for the object and sets the parent.
     * @param parent is the object in the hierarchy of the datamodel which is
     * the level over the object and its owner. If the parent is deleted the
     * object is deleted aswell.
     */
    explicit GtObject(GtObject* parent = nullptr);

    ~GtObject() override;

    /**
     * @brief objectFlags
     * @return the flags which are set for the object. These can be used to
     * describe its state (e.g. HasOwnChanges)
     * or its options in the user interface (e.g. UserRenamable)
     */
    GtObject::ObjectFlags objectFlags() const;

    /**
     * @brief Returns true if object is a dummy. Otherwise false is returned.
     * A dummy object is an object which cannot be recreated while starting GTlab
     * A typical situation would be if the object was created as a specific
     * datamodel class of a module which is not active in the current running
     * GTlab application.
     * To be safe not to loose information the given information are stored
     * in a dummy object.
     *
     * @return True if object is a dummy.
     */
    bool isDummy() const;

    /**
     * @brief Returns true if object has children of type dummy.
     * Otherwise false is returned.
     * For more information about dummy objects see the description of "isDummy"
     * @return True if object has children of type dummy.
     */
    bool hasDummyChildren() const;

    /**
     * @brief Returns true if any parent object is of type dummy.
     * Otherwise false is returned.
     * For more information about dummy objects see the description of "isDummy"
     * @return True if any parent object is of type dummy.
     */
    bool hasDummyParents() const;

    /**
     * @brief Creates a memento of the internal object state.
     * @param clone
     * @return
     */
    GtObjectMemento toMemento(bool clone = true) const;

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
     * A second object is creaated which is nearly identical to the object.
     * Only the uuid is a new one.
     * @return a copy of the object.
     */
    GtObject* copy() const;

    /**
     * @brief clone
     * A second object is creaated which is identical to the object.
     * Even the uuid is the same.
     * @return a clone of the object.
     */
    GtObject* clone() const;

    /**
     * @brief appendChild - appends the object as child. Relationship to the
     * old parent of the child object is destroyed
     * @param c child object
     * @return whether object has been appended or not
     */
    bool appendChild(GtObject* c);

    /**
     * @brief appendChildren - appends multiple objects as children.
     * Relationship to the old parents of the children objects are destroyed
     * @param list of the GtObjects.
     * @return true in case of success.
     */
    bool appendChildren(const QList<GtObject*>& list);

    /**
     * @brief Disconnects all data changed signals/slots and set parent to
     * nullptr.
     */
    void disconnectFromParent();

    /**
     * @brief Returns uuid of object.
     * Uuid means universal unique identifier.
     * Each object has a uuid to identfy it even if the names of objects
     * are identical.
     * @return UUID
     */
    QString uuid() const;

    /**
     * @brief Sets new uuid for object.
     * @param val - new uuid to use
     */
    void setUuid(const QString& val);

    /**
     * @brief Generates new object uuid uses this for the object
     * @param renewChildUUIDs - if this option is set (default is false)
     * the uuids of all child elemts are renewed, too.
     */
    void newUuid(bool renewChildUUIDs = false);

    /**
     * @brief Calculates object specific hash.
     * @return Object specific hash.
     */
    QString calcHash() const;

    /**
     * @brief Returns true if is default flag is active. Otherwise false is
     * returned.
     * @return Is default flag state.
     */
    bool isDefault() const;

    /**
     * @brief setDefault
     * Sets the isDefault flag
     * @param val New is default flag state.
     */
    void setDefault(bool val);

    /**
     * @brief Returns true if user hidden flag is active. Otherwise false is
     * returned.
     * @return User hidden flag state.
     */
    bool isUserHidden() const;

    /**
     * @brief Sets user hidden flag by given value.
     * @param val New user hidden flag state.
     */
    void setUserHidden(bool val);

    /**
     * @brief Returns true if is renamable flag is active. Otherwise false is
     * returned.
     * @return Is renamable flag state.
     */
    bool isRenamable() const;

    /**
     * @brief Returns true if object is user deletable and not default,
     * otherwise returns false.
     * @return Deletable flag
     */
    bool isDeletable() const;

    /**
     * @brief setFactory
     * @param factory
     */
    void setFactory(GtAbstractObjectFactory* factory);

    /**
     * @brief Returns the position index from the parent child list
     * @return position index. Returns -1 if the obejct does not have a parent
     */
    int childNumber() const;

    /**
     * @brief Reimplemented from QObject. Returns the parent with const
     * correctness
     * @return parent object
     */
    QObject* parent();
    QObject const* parent() const;

    /**
     * @brief Returns the parent automatically casted to GtObject. Returns
     * Nullptr if no parent exists or parent is not a GtObject.
     * @return parent object
     */
    GtObject* parentObject();
    GtObject const* parentObject() const;

    /**
     * @brief Returns all label identification strings stored in results of
     * object and all child objects.
     * @return List of label identification strings.
     */
    QStringList labelIds() const;

    /**
     * @brief hasChanges
     * @return
     */
    bool hasChanges() const;

    /**
     * @brief hasChildChanged
     * @return
     */
    bool hasChildChanged() const;

    /**
     * @brief newlyCreated
     * @return
     */
    bool newlyCreated() const;

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
    const QList<GtAbstractProperty*>& properties();
    const QList<GtAbstractProperty const*>& properties() const;

    /**
     * @brief Returns list of all property and sub property items. List
     * does not contain duplicates.
     * @return List of all properties.
     */
    QList<GtAbstractProperty*> fullPropertyList();
    QList<GtAbstractProperty const*> fullPropertyList() const;

    /**
     * @brief findProperty
     * @param id
     * @return
     */
    Q_INVOKABLE GtAbstractProperty* findProperty(const QString& id);
    const GtAbstractProperty* findProperty(const QString& id) const;

    /**
     * @brief findPropertyByName
     * @param name to use to search property
     * @return pointer to abstract property or nullptr if the property
     * could not be found
     */
    GtAbstractProperty* findPropertyByName(const QString& name);
    GtAbstractProperty const* findPropertyByName(const QString& name) const;

    /**
     * @brief Returns a dynamic sie property given its "id"
     *
     * @param id The id
     * @return A pointer to the property or nullptr, if it cannot be found
     */
    GtPropertyStructContainer* findPropertyContainer(const QString& id);
    GtPropertyStructContainer const*
    findPropertyContainer(const QString& id) const;

    std::vector<std::reference_wrapper<GtPropertyStructContainer>>&
    propertyContainers();
    std::vector<std::reference_wrapper<const GtPropertyStructContainer>>
    propertyContainers() const;

    /**
     * @brief propertiesByType
     * @return all properties of type T
     */
    template <typename T = GtAbstractProperty*,
              gt::trait::enable_if_ptr_base_of<GtAbstractProperty, T> = true>
    QList<T> propertiesByType()
    {
        return propertiesByTypeHelper<QList<T>>(this);
    }
    template <typename T = GtAbstractProperty*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_base_of<GtAbstractProperty, T> = true>
    QList<T_const_ptr> propertiesByType() const
    {
        return propertiesByTypeHelper<QList<T_const_ptr>>(this);
    }

    /**
     * @brief Returns child count
     * @param name - optional string parameter to caount only children with
     * the given name.
     * @return number of direct children of the given template class
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    int childCount(const QString& name = {}) const
    {
        return findDirectChildren<gt::trait::const_ptr<T>>(name).size();
    }

    /**
     * @brief findParent
     * @param name - optional string to define object by name
     * @return first parent obejct of the given template class
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T findParent(const QString& name = {})
    {
        return gt::findParent<T>(*this, name);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T_const_ptr findParent(const QString& name = {}) const
    {
        return gt::findParent<T_const_ptr>(*this, name);
    }

    /**
     * @brief findRoot
     * @param last
     * @return
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T findRoot(T last = nullptr)
    {
        return gt::findRoot<T>(*this, last);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T_const_ptr findRoot(T last = nullptr) const
    {
        return gt::findRoot<T_const_ptr>(*this, last);
    }

    /**
     * @brief findDirectChildren
     * @param name - optional string to search child with given name
     * @return returns list of pointers to children of the template class
     */
    template <class T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    QList<T> findDirectChildren(const QString& name = {})
    {
        return QObject::findChildren<T>(name, Qt::FindDirectChildrenOnly);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    QList<T_const_ptr> findDirectChildren(const QString& name = {}) const
    {
        return QObject::findChildren<T_const_ptr>(name, Qt::FindDirectChildrenOnly);
    }

    /**
     * @brief findChildren
     * @param name - optional string to search child with given name
     * @return returns list of pointers to children of the template class
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    QList<T> findChildren(const QString& name = {})
    {
        return QObject::findChildren<T>(name, Qt::FindChildrenRecursively);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    QList<T_const_ptr> findChildren(const QString& name = {}) const
    {
        return QObject::findChildren<T_const_ptr>(name, Qt::FindChildrenRecursively);
    }

    /**
     * @brief findDirectChild
     * @param name - optional string to search child with given name
     * @return return pointer to first child of the template class
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T findDirectChild(const QString& name = {})
    {
        return QObject::findChild<T>(name, Qt::FindDirectChildrenOnly);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T_const_ptr findDirectChild(const QString& name = {}) const
    {
        return QObject::findChild<T_const_ptr>(name, Qt::FindDirectChildrenOnly);
    }

    /**
     * @brief findChild
     * @param name - optional string to search child with given name
     * @return return pointer to first child of the template class
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T findChild(const QString& name = {})
    {
        return QObject::findChild<T>(name, Qt::FindChildrenRecursively);
    }
    template <typename T = GtObject*,
              typename T_const_ptr = gt::trait::const_ptr<T>,
              gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
    T_const_ptr findChild(const QString& name = {}) const
    {
        return QObject::findChild<T_const_ptr>(name, Qt::FindChildrenRecursively);
    }

    /**
     * @brief insertChild
     * @param pos - position to insert child to. Is not allowed to be negative
     * @param obj - object to insert as child
     * @return bool value to show success
     */
    template <typename T = GtObject*,
              gt::trait::enable_if_ptr_derived_of_qobject<T> =true,
              gt::trait::enable_if_ptr_not_const<T> = true>
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
            list.at(i)->setParent(nullptr);
        }

        appendChild(obj);

        for (T element : elementsBehindPos)
        {
            element->setParent(this);
        }

        return true;
    }

    /**
     * @brief getObjectByUuid
     * Searches for an object with the uuid.
     * The functions checks the object itself and all children if one of them
     * has an identical uuid to the given argument.
     * @param uuid
     * @return the pointer to the found object with the given uuid,
     * if no object was found return nullptr.
     */
    GtObject* getObjectByUuid(const QString& objectUUID);
    GtObject const* getObjectByUuid(const QString& objectUUID) const;

    /**
     * @brief getDirectChildByUuid
     * @param objectUUID
     * @return
     */
    GtObject* getDirectChildByUuid(const QString& objectUUID);
    GtObject const* getDirectChildByUuid(const QString& objectUUID) const;

    /**
     * @brief getObjectByPath
     * @param objectPath
     * @return
     */
    GtObject* getObjectByPath(const QString& objectPath);
    GtObject const* getObjectByPath(const QString& objectPath) const;

    /**
     * @brief getObjectByPath
     * @param objectPath
     * @return
     */
    GtObject* getObjectByPath(QStringList& objectPath);
    GtObject const* getObjectByPath(QStringList& objectPath) const;

    /**
     * @brief objectPath
     * @return
     */
    QString objectPath() const;

    /**
     * @brief setFlag
     * @param enable
     */
    void setFlag(GtObject::ObjectFlag flag, bool enable = true);

    /**
     * @brief factory
     * @return
     */
    GtAbstractObjectFactory* factory() const;

protected:
    /**
     * @brief registerProperty
     * @param property
     */
    bool registerProperty(GtAbstractProperty& property);

    bool registerPropertyStructContainer(GtPropertyStructContainer& c);

    /**
     * @brief registerProperty
     * @param property
     * @param cat
     */
    bool registerProperty(GtAbstractProperty& property, const QString& cat);
    /**
     * @brief registerProperty
     * @param property
     * @param cat
     * @param silent
     */
    bool registerSilentProperty(GtAbstractProperty& property);

    /**
     * @brief registerProperty
     * @param property
     * @param cat
     * @param silent
     */
    bool registerSilentProperty(GtAbstractProperty& property,
                                const QString& cat);

    /**
     * @brief Called after successfully restoring or merging data from memento
     */
    virtual void onObjectDataMerged();

    /**
     * @brief Called after successfully applying/reverting a memento diff
     */
    virtual void onObjectDiffMerged();

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

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief objectPath
     * @return
     */
    QString objectPath(QString& str) const;

    /**
     * @brief Connects the property changed singal to the data changed signal
     * of this object
     * @param property Property to connect
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
    void newChildUUIDs(GtObject* parent) const;

    /**
     * @brief Converts the object into a dummy object
     */
    void makeDummy();
    void importMementoIntoDummy(const GtObjectMemento&);
    void exportDummyIntoMemento(GtObjectMemento&) const;

    // ObjectPtr may be const
    template <typename R, typename ObjectPtr>
    static R propertiesByTypeHelper(ObjectPtr obj)
    {
        R retVal;
        for (auto* prop : obj->properties())
        {
            if (auto* p = qobject_cast<gt::trait::value_t<R>>(prop))
            {
                retVal.append(p);
            }
        }
        return retVal;
    }

private slots:

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

namespace gt
{

template <typename T, typename Object,
          trait::enable_if_ptr_derived_of_qobject<T>,
          trait::enable_if_base_of<QObject, std::decay_t<Object>>>
inline T
findParent(Object& object, const QString& name)
{
    if (auto* parent = object.parent())
    {
        auto* t = qobject_cast<T>(parent);
        if (t && (name.isEmpty() || name == t->objectName()))
        {
            return t;
        }
        return findParent<T>(*parent, name);
    }
    return nullptr;
}

template <typename T, typename Object,
          trait::enable_if_ptr_derived_of_qobject<T>,
          trait::enable_if_base_of<QObject, std::decay_t<Object>>>
inline T
findRoot(Object& object, T last)
{
    if (auto* parent = object.parent())
    {
        last = qobject_cast<T>(parent);

        if (T temp = findRoot<T>(*parent, last))
        {
            return temp;
        }
    }

    return last;
}

/**
 * @brief Attempts to find the object with the uuid in the list specified
 * @param objectUUID Uuid to search for
 * @param list List of objects
 * @return Pointer to object with the desired uuid
 */
template <typename List,
          typename T = typename List::value_type,
          trait::enable_if_ptr_base_of<GtObject, T> = true>
inline GtObject*
findObject(const QString& objectUUID, const List& list)
{
    auto iter = std::find_if(std::begin(list), std::end(list),
                             [&objectUUID](const GtObject* obj) {
        return obj->uuid() == objectUUID;
    });

    return iter != std::end(list) ? *iter : nullptr;
}

/**
 * @brief Returns true if obj is derived of the class named superClassName
 * @param obj Object to check
 * @param superClassName Name of the super class to check
 * @return Returns true if the class of obj inherits the super class
 * given by the super class name.
 * *
 * @note: Returns false aswell if obj is a nullptr or the super class name is
 * empty. The super classes are analyzed up to the GtObject class.
 */
GT_DATAMODEL_EXPORT
bool isDerivedFromClass(GtObject* obj,
                        QString const& superClassName);

/**
 * @brief Overlaod that accepts a meta object pointer.
 * @param metaObject Meta object to check
 * @param superClassName Name of the super class to check
 * @return Returns true if the class inherits the class denoted by the super
 * class name
 */
GT_DATAMODEL_EXPORT
bool isDerivedFromClass(QMetaObject const* metaObject,
                        QString const& superClassName);

/// disables usage with nullptr
bool isDerivedFromClass(nullptr_t, QString const&) = delete;

} // namespace gt

template <typename ListOfObjectPtrs>
[[deprecated("Use gt::findObject() instead.")]]
inline GtObject*
findObject(const QString& objectUUID, const ListOfObjectPtrs& list)
{
    return gt::findObject(objectUUID, list);
}

[[deprecated("Use gt::isDerivedFromClass() instead.")]]
inline bool isDerivedFromClass(GtObject* obj, QString const& superClassName)
{
    return gt::isDerivedFromClass(obj, superClassName);
}

#endif // GTOBJECT_H
