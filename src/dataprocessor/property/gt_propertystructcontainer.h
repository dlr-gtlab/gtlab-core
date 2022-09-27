/* GTlab - Gas Turbine laboratory
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GTPROPERTYSTRUCTCONTAINER_H
#define GTPROPERTYSTRUCTCONTAINER_H


#include "gt_datamodel_exports.h"

#include <QString>

#include <memory>

class GtPropertyStructInstance;
class GtPropertyStructDefinition;

/**
 * @brief A container containing of a dynamic number of PropertyStructs
 * of specified types.
 */
class GtPropertyStructContainer
{
public:
    GT_DATAMODEL_EXPORT
    GtPropertyStructContainer(const QString& ident, const QString& name);

    GT_DATAMODEL_EXPORT
    explicit GtPropertyStructContainer(const QString& ident);

    GT_DATAMODEL_EXPORT
    ~GtPropertyStructContainer();

    /**
     * @brief registerAllowedType
     * @param typeID
     * @param f
     */
    GT_DATAMODEL_EXPORT
    void registerAllowedType(const GtPropertyStructDefinition& f);

    /**
     * @brief Performs an in-place creation of a new struct instance
     * given the type.
     *
     * Note, since we need unique names to reference entries of the container,
     * the name of the entry will be a UUID
     *
     * @param typeID The typename / classname of the struct to create
     * @return
     */
    GT_DATAMODEL_EXPORT
    GtPropertyStructInstance& newEntry(QString typeID);

    /**
     * @brief Returns the number of entries in the vector
     */
    GT_DATAMODEL_EXPORT
    size_t size() const;

    GT_DATAMODEL_EXPORT
    QString ident() const;

    GT_DATAMODEL_EXPORT
    QString name() const;

    /**
     * @brief access specified element
     *
     * Returns a reference to the element at specified location pos.
     * No bounds checking is performed.
     *
     * @param idx position of the element to return
     * @return Reference to the requested element.
     */
    GT_DATAMODEL_EXPORT
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
    GT_DATAMODEL_EXPORT
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
    GT_DATAMODEL_EXPORT
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
    GT_DATAMODEL_EXPORT
    const GtPropertyStructInstance& at(size_t idx) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTPROPERTYSTRUCTCONTAINER_H
