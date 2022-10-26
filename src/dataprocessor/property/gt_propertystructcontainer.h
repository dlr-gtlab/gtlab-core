/* GTlab - Gas Turbine laboratory
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GTPROPERTYSTRUCTCONTAINER_H
#define GTPROPERTYSTRUCTCONTAINER_H


#include "gt_datamodel_exports.h"

#include "gt_polyvector.h"

#include <QString>

#include <memory>

class GtPropertyStructInstance;
class GtPropertyStructDefinition;

/**
 * @brief A container containing of a dynamic number of PropertyStructs
 * of specified types.
 */
class GT_DATAMODEL_EXPORT GtPropertyStructContainer
{
public:
    using iterator =
        gt::PolyVector<GtPropertyStructInstance>::iterator;
    using const_iterator =
        gt::PolyVector<GtPropertyStructInstance>::const_iterator;

    GtPropertyStructContainer(const QString& ident, const QString& name);


    explicit GtPropertyStructContainer(const QString& ident);

    ~GtPropertyStructContainer();

    /**
     * @brief registerAllowedType
     * @param typeID
     * @param f
     */
    void registerAllowedType(const GtPropertyStructDefinition& f);

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

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTPROPERTYSTRUCTCONTAINER_H
