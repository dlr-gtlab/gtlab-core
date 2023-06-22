/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 7.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTEVENTLOOP_H
#define GTEVENTLOOP_H

#include "gt_datamodel_exports.h"

#include <QEventLoop>

/**
 * @brief The GtStatusFuture class
 */
class GT_DATAMODEL_EXPORT GtEventLoop : public QObject
{
    Q_OBJECT

public:

    enum State
    {
        Success = 0,
        Failed = 1,
        Aborted = 2
    };
    Q_ENUM(State)

    /**
     * @brief Constructor
     * @param mstimeout Timeout in ms. Use -1 for no timeout
     */
    explicit GtEventLoop(int mstimeout);
    explicit GtEventLoop(std::chrono::milliseconds timeout)
        : GtEventLoop(timeout.count())
    {}

    /**
     * @brief Setter for timeout, after which exec will return Failed
     * @param mstimeout Timeout in ms. Use -1 for no timeout
     * @return reference
     */
    GtEventLoop& setTimeout(int mstimeout);
    GtEventLoop& setTimeout(std::chrono::milliseconds timeout)
    {
        return setTimeout(timeout.count());
    }

    /**
     * @brief Clears all internal connections. Should be called before
     * setting up singals.
     * @return reference
     */
    GtEventLoop& clear();

    /**
     * @brief Blocking call. Executes an event loop until either the abort,
     * succes or failed signal has been emitted.
     *
     * Note: All connections must be set prior!
     * @return State or Reason, why event loop exited
     */
    State exec();

    /**
     * @brief Helper function to connect the abort signal
     * @param t Object to connect
     * @param f Signal to trigger abort
     * @return reference
     */
    template <typename T, typename F>
    GtEventLoop& connectAbort(T* t, F&& f)
    {
        QObject::connect(t, std::forward<F>(f), this, &GtEventLoop::abort,
                         Qt::QueuedConnection);
        return *this;
    }
    /// overload that accepts SLOT/SIGNAL macro
    template <typename T>
    GtEventLoop& connectAbort(T* t, const char* f)
    {
        QObject::connect(t, f, this, SIGNAL(abort()), Qt::QueuedConnection);
        return *this;
    }

    /**
     * @brief Helper function to connect the success signal
     * @param t Object to connect
     * @param f Signal to trigger success
     * @return reference
     */
    template <typename T, typename F>
    GtEventLoop& connectSuccess(T* t, F&& f)
    {
        QObject::connect(t, std::forward<F>(f), this, &GtEventLoop::success,
                         Qt::QueuedConnection);
        return *this;
    }
    /// overload that accepts SLOT/SIGNAL macro
    template <typename T>
    GtEventLoop& connectSuccess(T* t, const char* f)
    {
        QObject::connect(t, f, this, SIGNAL(success()), Qt::QueuedConnection);
        return *this;
    }

    /**
     * @brief Helper function to connect the failed signal
     * @param t Object to connect
     * @param f Signal to trigger failed
     * @return reference
     */
    template <typename T, typename F>
    GtEventLoop& connectFailed(T* t, F&& f)
    {
        QObject::connect(t, std::forward<F>(f), this, &GtEventLoop::failed,
                         Qt::QueuedConnection);
        return *this;
    }
    /// overload that accepts SLOT/SIGNAL macro
    template <typename T>
    GtEventLoop& connectFailed(T* t, const char* f)
    {
        QObject::connect(t, f, this, SIGNAL(failed()), Qt::QueuedConnection);
        return *this;
    }

signals:

    /// May be emitted to abort the event loop.
    /// Will also be emitted once the event loop has aborted.
    void abort();

    /// May be emitted to quit the event loop with success.
    /// Will also be emitted once the event loop has successfully finished.
    void success();

    /// May be emitted to exit the event loop with failed.
    /// Will also be emitted once the event loop has failed.
    void failed();

    /// Will be emitted once the event loop has finished.
    void finished();

private:

    QEventLoop m_eventLoop;

    int m_mstimeout;
};

#endif // GTEVENTLOOP_H
