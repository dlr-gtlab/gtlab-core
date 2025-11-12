/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GTPROPERTYSTRUCTCONTAINER_H
#define GTPROPERTYSTRUCTCONTAINER_H


#include "gt_datamodel_exports.h"

#include "gt_polyvector.h"
#include "gt_abstractproperty.h"

#include <QString>
#include <QObject>

#include <memory>

class GtPropertyStructInstance;
class GtPropertyStructDefinition;

/**
 * @brief A container containing of a dynamic number of PropertyStructs
 * of specified types.
 */
class GT_DATAMODEL_EXPORT GtPropertyStructContainer : public QObject
{
    Q_OBJECT
public:

    enum ContainerType
    {
        Sequential,   /* Like a vector, ids are uuids */
        Associative  /* Like a map with string key, ids are named keys */
    };

    using iterator =
        gt::PolyVector<GtPropertyStructInstance>::iterator;
    using const_iterator =
        gt::PolyVector<GtPropertyStructInstance>::const_iterator;

    // Creates a sequential container
    GtPropertyStructContainer(const QString& ident, const QString& name);
    explicit GtPropertyStructContainer(const QString& ident);

    GtPropertyStructContainer(const QString& ident, const QString& name, ContainerType);
    explicit GtPropertyStructContainer(const QString& ident, ContainerType);

    ~GtPropertyStructContainer() override;

    ContainerType type() const;

    enum Flags
    {
        Hidden   = 1,
        ReadOnly = 2,

    };

    /**
     * @brief Gets the flags of the container, possibly as a superposition
     *        of GtPropertyStructContainer::Flags
     * @return The flags values
     */
    int getFlags() const;

    /**
     * @brief Sets the flags of the container
     *
     * Example setFlags(Hidden | ReadOnly)
     */
    void setFlags(int);

    /**
     * @brief registerAllowedType
     * @param typeID
     * @param f
     */
    void registerAllowedType(const GtPropertyStructDefinition& f);

    /**
     * @brief Returns list of all allowed types.
     * @return List of allowed type identification strings
     */
    QStringList allowedTypes() const;

    /**
     * @brief Performs an in-place creation of a new struct instance
     * given the type.
     *
     * Note, since we need unique names to reference entries of the container,
     * the name of the entry will be a UUID
     *
     * @param typeID The typename / classname of the struct to create
     * @param id Optional id of the entry. If not set, a uuid
     *           will be set as the id
     * @return
     */
    GtPropertyStructInstance& newEntry(QString typeID, QString id = {});

    /**
     * @brief Performs an in-place insertion of a new struct instance
     * given the type.
     *
     * Note, since we need unique names to reference entries of the container,
     * the name of the entry will be a UUID
     *
     * @param typeID The typename / classname of the struct to create
     * @param id Optional id of the entry. If not set, a uuid
     *           will be set as the id
     * @param position Iterator to the position of the new entry
     * @return
     */
    GtPropertyStructInstance& newEntry(QString typeID,
                                       const_iterator position, 
                                       QString id = {});

    /**
     * @brief Removes an Entry from the container
     * @param position Iterator to the element to be removed
     * @return Iterator following the removed element.
     */
    iterator removeEntry(iterator position);

    const_iterator findEntry(const QString& id) const;
    iterator findEntry(const QString& id);

    /**
     * @brief Returns the number of entries in the vector
     */
    size_t size() const;

    QString ident() const;

    QString name() const;

    /**
     * @brief Returns the entry prefix, which is used to give
     * entries a name for display
     *
     * If e.g. the entries of the container should be displayed
     *   stage[0]
     *   stage[1]
     *   stage[2],
     * the prefix is "stage".
     */
    QString entryPrefix() const;

    QString entryDisplayName(const_iterator position) const;
    QString entryDisplayName(size_t index) const;

    /**
     * @brief Sets the entry prefix, which is used to give
     * entries a name for display
     *
     * If e.g. the entries of the container should be displayed
     *   stage[0]
     *   stage[1]
     *   stage[2],
     * the prefix is "stage".
     */
    GtPropertyStructContainer& setEntryPrefix(QString prefix);

    /**
     * @brief access specified element
     *
     * Returns a reference to the element at specified location pos.
     * No bounds checking is performed.
     *
     * @param idx position of the element to return
     * @return Reference to the requested element.
     */
    GtPropertyStructInstance& operator[](size_t idx);

    /**
     * @brief access specified element
     *
     * Returns a reference to the element at specified location pos.
     * No bounds checking is performed.
     *
     * @param idx position of the element to return
     * @return Reference to the requested element.
     */
    const GtPropertyStructInstance& operator[](size_t idx) const;

    /**
     * @brief access specified element with bounds checking
     *
     * Returns a reference to the element at specified location pos,
     * with bounds checking. If pos is not within the range of the container,
     * an exception of type std::out_of_range is thrown.
     *
     * @param idx position of the element to return
     * @return Reference to the requested element.
     */
    GtPropertyStructInstance& at(size_t idx);

    /**
     * @brief access specified element with bounds checking
     *
     * Returns a reference to the element at specified location pos,
     * with bounds checking. If pos is not within the range of the container,
     * an exception of type std::out_of_range is thrown.
     *
     * @param idx position of the element to return
     * @return Reference to the requested element.
     */
    const GtPropertyStructInstance& at(size_t idx) const;

    void clear();

    /**
     * @brief Begin Iterator
     */
    iterator begin();

    /**
     * @brief Begin Iterator
     */
    const_iterator begin() const;

    /**
     * @brief End Iterator
     */
    iterator end();

    /**
     * @brief End Iterator
     */
    const_iterator end() const;

signals:
    /**
     *  An entry was removed at the given index
     *
     *  The previous element container[idx] does not exist anymore.
     */
    void entryRemoved(int idx);

    /**
     * @brief An antry was added at the given index.
     *
     * The new element is container[idx]
     */
    void entryAdded(int idx);

    /**
     * @brief A property at the given index was changed
     * @param idx The index of the changed entry
     * @param property A pointer to the changed property
     */
    void entryChanged(int idx, GtAbstractProperty* property);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTPROPERTYSTRUCTCONTAINER_H
