/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_dynamicpropertyfactory.h"

#include <QMutex>
#include <QMutexLocker>
#include <map>
#include <utility>
#include <vector>

namespace
{
    struct Entry
    {
        quint64 token;
        GtDynamicPropertyFactory::Matcher matcher;
        GtDynamicPropertyFactory::DefinitionWriter writer;
        GtDynamicPropertyFactory::Creator creator;
    };

    QMutex& registryMutex()
    {
        // Modules may be unloaded by Qt during process shutdown, after normal
        // function-local statics have already been destroyed. Keep the
        // registry infrastructure alive until the process exits so a module's
        // Registration handle can safely unregister from its destructor.
        static QMutex* mutex = new QMutex;
        return *mutex;
    }

    std::map<QString, Entry>& registry()
    {
        static auto* entries = new std::map<QString, Entry>;
        return *entries;
    }

    quint64& nextToken()
    {
        static quint64 token = 1;
        return token;
    }
} // namespace

GtDynamicPropertyFactory::Registration::Registration(
    Registration&& other) noexcept : m_token(other.m_token)
{
    other.m_token = 0;
}

GtDynamicPropertyFactory::Registration&
GtDynamicPropertyFactory::Registration::operator=(Registration&& other) noexcept
{
    if (this != &other)
    {
        GtDynamicPropertyFactory::unregister(m_token);
        m_token = other.m_token;
        other.m_token = 0;
    }
    return *this;
}

GtDynamicPropertyFactory::Registration::~Registration()
{
    GtDynamicPropertyFactory::unregister(m_token);
}

GtDynamicPropertyFactory::Registration
GtDynamicPropertyFactory::registerType(const QString& typeId, Matcher matcher,
                                       DefinitionWriter writer, Creator creator)
{
    if (typeId.trimmed().isEmpty() || !matcher || !writer || !creator)
    {
        return {};
    }
    QMutexLocker lock(&registryMutex());
    if (registry().count(typeId))
    {
        return {};
    }
    const quint64 token = nextToken()++;
    registry().emplace(typeId, Entry{token, std::move(matcher),
                                     std::move(writer), std::move(creator)});
    return Registration(token);
}

bool
GtDynamicPropertyFactory::definition(const GtAbstractProperty& property,
                                     QString& typeId, QVariantMap& result)
{
    struct MatcherEntry
    {
        QString typeId;
        quint64 token;
        Matcher matcher;
    };

    std::vector<MatcherEntry> entries;
    {
        QMutexLocker lock(&registryMutex());
        entries.reserve(registry().size());
        for (const auto& entry : registry())
        {
            entries.push_back(
                {entry.first, entry.second.token, entry.second.matcher});
        }
    }

    const MatcherEntry* match = nullptr;
    for (const auto& entry : entries)
    {
        if (!entry.matcher(property)) continue;
        if (match) return false;
        match = &entry;
    }
    if (!match) return false;

    DefinitionWriter writer;
    {
        QMutexLocker lock(&registryMutex());
        auto iter = registry().find(match->typeId);
        if (iter == registry().end() || iter->second.token != match->token)
        {
            return false;
        }
        writer = iter->second.writer;
    }

    typeId = match->typeId;
    result = writer(property);
    return true;
}

bool
GtDynamicPropertyFactory::definition(const QString& typeId,
                                     const GtAbstractProperty& property,
                                     QVariantMap& result)
{
    DefinitionWriter writer;
    {
        QMutexLocker lock(&registryMutex());
        auto iter = registry().find(typeId);
        if (iter == registry().end()) return false;
        writer = iter->second.writer;
    }

    result = writer(property);
    return true;
}

std::unique_ptr<GtAbstractProperty>
GtDynamicPropertyFactory::create(const QString& typeId, const QString& ident,
                                 const QString& name,
                                 const QVariantMap& definition)
{
    Creator creator;
    {
        QMutexLocker lock(&registryMutex());
        auto iter = registry().find(typeId);
        if (iter == registry().end()) return {};
        creator = iter->second.creator;
    }

    return creator(ident, name, definition);
}

void
GtDynamicPropertyFactory::unregister(quint64 token)
{
    if (!token) return;
    QMutexLocker lock(&registryMutex());
    for (auto iter = registry().begin(); iter != registry().end(); ++iter)
    {
        if (iter->second.token == token)
        {
            registry().erase(iter);
            return;
        }
    }
}
