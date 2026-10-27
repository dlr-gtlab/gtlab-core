/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_collectionregistry.h"

#include "gt_collectioninterface.h"

#include <QHash>
#include <QIcon>

struct GtCollectionRegistry::Impl
{
    QHash<QString, GtCollectionInterface*> collections;

}; // struct Impl

GtCollectionRegistry::GtCollectionRegistry() :
    impl{std::make_unique<Impl>()}
{ }

GtCollectionRegistry::~GtCollectionRegistry() noexcept = default;

GtCollectionRegistry&
GtCollectionRegistry::instance() noexcept
{
    static GtCollectionRegistry self;
    return self;
}

GtCollectionRegistry::Impl*
GtCollectionRegistry::pimpl() noexcept
{
    return impl.get();
}

const GtCollectionRegistry::Impl*
GtCollectionRegistry::pimpl() const noexcept
{
    return impl.get();
}

bool
GtCollectionRegistry::registerCollection(const QString& collectionId,
                                         GtCollectionInterface* icollection)
{
    if (collection(collectionId)) return false;

    pimpl()->collections.insert(collectionId, icollection);
    return true;
}

QStringList
GtCollectionRegistry::collectionIds() const
{
    return pimpl()->collections.keys();
}

QIcon
GtCollectionRegistry::collectionIcon(const QString& collectionId) const
{
    GtCollectionInterface* coll = collection(collectionId);
    if (!coll) return {};

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtCollectionRegistry::collectionStructure(const QString& collectionId) const
{
    GtCollectionInterface* coll = collection(collectionId);
    if (!coll) return {};

    return coll->collectionStructure();
}

GtCollectionInterface*
GtCollectionRegistry::collection(const QString& collectionId) const
{
    return pimpl()->collections.value(collectionId, nullptr);
}
