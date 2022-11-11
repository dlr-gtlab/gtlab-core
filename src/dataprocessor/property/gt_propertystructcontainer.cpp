/* GTlab - Gas Turbine laboratory
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_propertystructcontainer.h"

#include "gt_polyvector.h"
#include "gt_structproperty.h"
#include "gt_exceptions.h"

#include <QUuid>

#include <map>
#include <utility>


struct GtPropertyStructContainer::Impl
{
    Impl(const QString& ident, const QString& name) :
        id(ident),
        name(name)
    {
    }

    QString id, name, entryPrefix;

    using TypeIdStr = QString;
    std::map<TypeIdStr, GtPropertyStructDefinition> allowedTypes;
    gt::PolyVector<GtPropertyStructInstance> entries;
};

GtPropertyStructContainer::GtPropertyStructContainer(const QString& ident,
                                                     const QString& name) :
    pimpl(std::make_unique<GtPropertyStructContainer::Impl>(ident, name))
{
}

GtPropertyStructContainer::GtPropertyStructContainer(const QString& ident) :
    GtPropertyStructContainer(ident, ident)
{
}

GtPropertyStructContainer::~GtPropertyStructContainer() = default;

void
GtPropertyStructContainer::registerAllowedType(
    const GtPropertyStructDefinition& f)
{
    pimpl->allowedTypes.insert(std::make_pair(f.typeName(), f));
}

GtPropertyStructInstance&
GtPropertyStructContainer::newEntry(QString typeID, QString id)
{
    return newEntry(std::move(typeID), end(), std::move(id));
}

GtPropertyStructInstance&
GtPropertyStructContainer::newEntry(QString typeID, const_iterator position,
                                    QString id)
{
    const auto iter = pimpl->allowedTypes.find(typeID);
    if (iter == pimpl->allowedTypes.end())
    {
        throw GTlabException(
            "GtPropertyStructInstance::newEntry",
            QString("Cannot create a structure of type '%1'.").arg(typeID));
    }

    const auto& structureDefinition = iter->second;

    if (id.isEmpty())
    {
        id = QUuid::createUuid().toString();QUuid::createUuid().toString();
    }

    pimpl->entries.insert(position,
                          structureDefinition.newInstance(std::move(id)));

    return pimpl->entries[pimpl->entries.size() - 1];
}

GtPropertyStructContainer::iterator
GtPropertyStructContainer::removeEntry(iterator position)
{
    return pimpl->entries.erase(position);
}

GtPropertyStructContainer::const_iterator
GtPropertyStructContainer::findEntry(const QString &id) const
{
    auto iter = std::find_if(begin(), end(),
                             [&id](const GtPropertyStructInstance& entry) {
        return id == entry.ident();

    });

    return iter;
}

GtPropertyStructContainer::iterator
GtPropertyStructContainer::findEntry(const QString &id)
{
    auto iter = std::find_if(begin(), end(),
                            [&id](const GtPropertyStructInstance& entry) {
        return id == entry.ident();
    });

    return iter;
}

size_t
GtPropertyStructContainer::size() const
{
    return pimpl->entries.size();
}

QString
GtPropertyStructContainer::ident() const
{
    return pimpl->id;
}

QString
GtPropertyStructContainer::name() const
{
    return pimpl->name;
}

QString
GtPropertyStructContainer::entryPrefix() const
{
    return pimpl->entryPrefix;
}

GtPropertyStructContainer&
GtPropertyStructContainer::setEntryPrefix(QString prefix)
{
    pimpl->entryPrefix = std::move(prefix);
    return *this;
}

const GtPropertyStructInstance&
GtPropertyStructContainer::operator[](size_t idx) const
{
    return pimpl->entries[idx];
}

GtPropertyStructInstance&
GtPropertyStructContainer::at(size_t idx)
{
    return pimpl->entries.at(idx);
}

const GtPropertyStructInstance&
GtPropertyStructContainer::at(size_t idx) const
{
    return pimpl->entries.at(idx);
}

void GtPropertyStructContainer::clear()
{
    pimpl->entries.clear();
}

GtPropertyStructContainer::iterator
GtPropertyStructContainer::begin()
{
    return pimpl->entries.begin();
}

GtPropertyStructContainer::const_iterator
GtPropertyStructContainer::begin() const
{
    return pimpl->entries.begin();
}

GtPropertyStructContainer::iterator
GtPropertyStructContainer::end()
{
    return pimpl->entries.end();
}

GtPropertyStructContainer::const_iterator
GtPropertyStructContainer::end() const
{
    return pimpl->entries.end();
}

GtPropertyStructInstance&
GtPropertyStructContainer::operator[](size_t idx)
{
    return pimpl->entries[idx];
}
