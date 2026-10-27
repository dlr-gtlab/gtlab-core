/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_COLLECTIONREGISTRY_H
#define GT_COLLECTIONREGISTRY_H

#include <gt_gui_exports.h>
#include <gt_platform.h>

#include <QMap>
#include <QMetaType>

#include <memory>

class GtCollectionInterface;
class QIcon;

/**
 * @brief The GtCollectionRegistry class. Interace to access registered
 * collections
 */
class GT_GUI_EXPORT GtCollectionRegistry final
{
public:
    ~GtCollectionRegistry() noexcept;

    /**
     * @brief Returns the current instance of this factory
     * @return Self
     */
    GT_NO_DISCARD
    static GtCollectionRegistry& instance() noexcept;

    /**
     * @brief Registers the collection
     * @param collectionId Id of the collection
     * @param icollection Collection interface
     * @return success
     */
    bool registerCollection(const QString& collectionId,
                            GtCollectionInterface* icollection);

    /**
     * @brief Returns the registered collection ids
     * @return Collection ids
     */
    GT_NO_DISCARD
    QStringList collectionIds() const;

    /**
     * @brief Returns the registered collection icon for the given collection id
     * @param collectionId Id of the collection
     * @return Icon
     */
    GT_NO_DISCARD
    QIcon collectionIcon(const QString& collectionId) const;

    /**
     * @brief Collecion structure
     * @param collectionId Id of the collection
     * @return Collection structure
     */
    GT_NO_DISCARD
    QMap<QString, QMetaType::Type> collectionStructure(
        const QString& collectionId) const;

    /**
     * @brief Returns the collection for the given id
     * @param collectionId Id of the collection
     * @return Collection
     */
    GT_NO_DISCARD
    GtCollectionInterface* collection(const QString& collectionId) const;

private:
    GtCollectionRegistry();

    struct Impl;
    std::unique_ptr<Impl> impl;

    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl* pimpl() noexcept;
    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    const Impl* pimpl() const noexcept;
};

#endif // GT_COLLECTIONREGISTRY_H
