/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_operationexecutioncontext.h"

#include "gt_executioneventstream.h"

#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <utility>

#include <QUuid>

struct GtCancellationToken::State
{
    struct Subscriber
    {
        explicit Subscriber(std::function<void()> callback) :
            callback(std::move(callback))
        {
        }

        void invoke() noexcept
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (!active)
                {
                    return;
                }
                ++inFlight;
            }

            try
            {
                callback();
            }
            catch (...)
            {
                // Cancellation requests must remain noexcept.
            }

            {
                std::lock_guard<std::mutex> lock(mutex);
                --inFlight;
                condition.notify_all();
            }
        }

        void deactivate()
        {
            std::unique_lock<std::mutex> lock(mutex);
            active = false;
            condition.wait(lock, [this] { return inFlight == 0; });
        }

        std::function<void()> callback;
        std::mutex mutex;
        std::condition_variable condition;
        bool active{true};
        std::size_t inFlight{0};
    };

    std::mutex mutex;
    std::unordered_map<std::size_t, std::shared_ptr<Subscriber>> subscribers;
    std::size_t nextSubscriberId{0};
    bool requested{false};
};

struct GtOperationExecutionContext::Impl
{
    Impl(GtObject* data, GtExecutionEventStream& events,
         GtCancellationToken cancellation) :
        data(data),
        events(&events), cancellation(std::move(cancellation))
    {
    }

    GtObject* data;
    GtExecutionEventStream* events;
    GtCancellationToken cancellation;
};

GtExecutionId::GtExecutionId() :
    m_value(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

bool GtExecutionId::operator==(GtExecutionId const& other) const
{
    return m_value == other.m_value;
}

QString const&
GtExecutionId::toString() const noexcept
{
    return m_value;
}

GtCancellationToken::GtCancellationToken() : m_state(std::make_shared<State>())
{
}

GtCancellationToken::Subscription::Subscription(
    std::function<void()> unsubscribe) : m_unsubscribe(std::move(unsubscribe))
{
}

GtCancellationToken::Subscription::~Subscription()
{
    if (m_unsubscribe)
    {
        m_unsubscribe();
    }
}

GtCancellationToken::Subscription::Subscription(Subscription&& other) noexcept :
    m_unsubscribe(std::move(other.m_unsubscribe))
{
}

GtCancellationToken::Subscription&
GtCancellationToken::Subscription::operator=(Subscription&& other) noexcept
{
    if (this != &other)
    {
        if (m_unsubscribe)
        {
            m_unsubscribe();
        }
        m_unsubscribe = std::move(other.m_unsubscribe);
    }
    return *this;
}

void
GtCancellationToken::requestCancellation() noexcept
{
    std::unordered_map<std::size_t, std::shared_ptr<State::Subscriber>>
        subscribers;
    {
        std::lock_guard<std::mutex> lock(m_state->mutex);
        if (m_state->requested)
        {
            return;
        }
        m_state->requested = true;
        subscribers.swap(m_state->subscribers);
    }

    for (const auto& entry : subscribers)
    {
        entry.second->invoke();
    }
}

GtCancellationToken::Subscription
GtCancellationToken::subscribe(std::function<void()> callback) const
{
    if (!callback)
    {
        return {};
    }

    auto subscriber = std::make_shared<State::Subscriber>(std::move(callback));
    std::size_t subscriberId{};
    bool requested{};
    {
        std::lock_guard<std::mutex> lock(m_state->mutex);
        requested = m_state->requested;
        if (!requested)
        {
            subscriberId = m_state->nextSubscriberId++;
            m_state->subscribers.emplace(subscriberId, subscriber);
        }
    }

    if (requested)
    {
        subscriber->invoke();
        subscriber->deactivate();
        return {};
    }

    std::weak_ptr<State> weakState = m_state;
    return Subscription([weakState, subscriberId, subscriber] {
        subscriber->deactivate();
        if (auto state = weakState.lock())
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->subscribers.erase(subscriberId);
        }
    });
}

bool
GtCancellationToken::isCancellationRequested() const noexcept
{
    std::lock_guard<std::mutex> lock(m_state->mutex);
    return m_state->requested;
}

GtOperationExecutionContext::GtOperationExecutionContext(
    GtObject* data, GtExecutionEventStream& events,
    GtCancellationToken cancellation) :
    m_impl(std::make_unique<Impl>(data, events, std::move(cancellation)))
{
}

GtOperationExecutionContext::~GtOperationExecutionContext() = default;

GtOperationExecutionContext::GtOperationExecutionContext(
    GtOperationExecutionContext const& other) :
    m_impl(std::make_unique<Impl>(*other.m_impl))
{
}

GtOperationExecutionContext&
GtOperationExecutionContext::operator=(GtOperationExecutionContext const& other)
{
    if (this != &other)
    {
        m_impl = std::make_unique<Impl>(*other.m_impl);
    }

    return *this;
}

GtOperationExecutionContext::GtOperationExecutionContext(
    GtOperationExecutionContext&& other) noexcept = default;

GtOperationExecutionContext& GtOperationExecutionContext::operator=(
    GtOperationExecutionContext&& other) noexcept = default;

GtObject*
GtOperationExecutionContext::data() noexcept
{
    return m_impl->data;
}

GtObject const*
GtOperationExecutionContext::data() const noexcept
{
    return m_impl->data;
}

GtExecutionId const&
GtOperationExecutionContext::executionId() const noexcept
{
    return m_impl->events->executionId();
}

GtExecutionEventStream&
GtOperationExecutionContext::events() noexcept
{
    return *m_impl->events;
}

GtCancellationToken&
GtOperationExecutionContext::cancellation() noexcept
{
    return m_impl->cancellation;
}

GtCancellationToken const&
GtOperationExecutionContext::cancellation() const noexcept
{
    return m_impl->cancellation;
}
