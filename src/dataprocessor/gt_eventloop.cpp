/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 7.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_eventloop.h"
#include "gt_logging.h"

#include <QTimer>

GtEventLoop::GtEventLoop(int mstimeout) :
    m_mstimeout{mstimeout}
{
    setObjectName("GtEventLoop");
    clear();
}

GtEventLoop&
GtEventLoop::setTimeout(int mstimeout)
{
    m_mstimeout = mstimeout;
    return *this;
}

GtEventLoop&
GtEventLoop::clear()
{
    this->disconnect();
    m_eventLoop.disconnect();

    auto success = connect(this, &GtEventLoop::abort, &m_eventLoop, [&](){
        m_eventLoop.exit(Aborted);
    }, Qt::QueuedConnection);
    assert(success);

    success = connect(this, &GtEventLoop::success, &m_eventLoop, [&](){
        m_eventLoop.exit(Success);
    }, Qt::QueuedConnection);
    assert(success);

    success = connect(this, &GtEventLoop::failed, &m_eventLoop, [&](){
        m_eventLoop.exit(Failed);
    }, Qt::QueuedConnection);
    assert(success);

    return *this;
}

GtEventLoop::State
GtEventLoop::exec()
{
    // trigger eventloop and timeout
    QTimer timer;
    if (m_mstimeout > 0)
    {
        timer.setSingleShot(true);
        connectFailed(&timer, &QTimer::timeout);
        timer.start(m_mstimeout);
    }

    int state = m_eventLoop.exec();
    timer.stop();

    // sanity check
    if (state < Success || state > Aborted)
    {
        state = Failed;
    }
    emit finished();
    return static_cast<State>(state);
}
