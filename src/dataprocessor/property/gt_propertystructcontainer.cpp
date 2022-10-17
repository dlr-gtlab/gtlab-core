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


namespace gt
{

template<class T> struct add_const { typedef const T type; };

template< class T >
using add_const_t    = typename add_const<T>::type;

template <class T>
constexpr add_const_t<T>& as_const(T& t) noexcept
{
    return t;
}

}

struct GtPropertyStructContainer::Impl
{
    Impl(const QString& ident, const QString& name) :
        id(ident),
        name(name)
    {
    }

    QString id, name;

    using TypeIdStr = QString;
    std::map<TypeIdStr, GtPropertyStructDefinition> allowedTypes;
    gt::poly_vector<GtPropertyStructInstance> entries;
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
    return newEntry(typeID, end(), id);
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

const GtPropertyStructInstance *
GtPropertyStructContainer::findEntry(const QString &id) const
{
    auto iter = std::find_if(std::begin(pimpl->entries), std::end(pimpl->entries),
                             [&id](const GtPropertyStructInstance& entry) {
        return id == entry.ident();

    });

    if (iter == pimpl->entries.end())
    {
        return nullptr;
    }

    return &*iter;
}

GtPropertyStructInstance *
GtPropertyStructContainer::findEntry(const QString &id)
{
    auto ret = static_cast<const GtPropertyStructContainer*>(this)
                   ->findEntry(id);
    return const_cast<GtPropertyStructInstance*>(ret);
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
    return gt::as_const(pimpl->entries).begin();
}

GtPropertyStructContainer::iterator
GtPropertyStructContainer::end()
{
    return pimpl->entries.end();
}

GtPropertyStructContainer::const_iterator
GtPropertyStructContainer::end() const
{
    return gt::as_const(pimpl->entries).end();
}

GtPropertyStructInstance&
GtPropertyStructContainer::operator[](size_t idx)
{
    return pimpl->entries[idx];
}
