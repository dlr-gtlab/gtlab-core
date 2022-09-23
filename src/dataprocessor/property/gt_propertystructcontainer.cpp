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

#include <map>
#include <utility>

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
GtPropertyStructContainer::newEntry(QString name, QString typeID)
{
    const auto iter = pimpl->allowedTypes.find(typeID);
    if (iter == pimpl->allowedTypes.end())
    {
        throw GTlabException(
            "GtPropertyStructInstance::newEntry",
            QString("Cannot create a structure of type '%1'.").arg(typeID));
    }

    const auto& structureDefinition = iter->second;

    pimpl->entries.push_back(structureDefinition.newInstance(name));

    return pimpl->entries[pimpl->entries.size() - 1];
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

GtPropertyStructInstance&
GtPropertyStructContainer::operator[](size_t idx)
{
    return pimpl->entries[idx];
}
