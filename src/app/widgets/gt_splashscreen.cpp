/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QMovie>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QElapsedTimer>
#include <QWindow>

#ifdef Q_OS_WIN
#  include <QtCore/qt_windows.h>
#else
#  include <time.h>
#endif

#include "gt_colors.h"
#include "gt_splashscreen.h"

class SplashLabel : public QLabel
{
public:
    QString m_txt;
    bool m_showMessage = {false};

    explicit SplashLabel(QWidget * obj) : QLabel(obj) {}

    void paintEvent(QPaintEvent* e) override
    {
        QLabel::paintEvent(e);

        if (m_showMessage)
        {
            QPainter painter(this);

            painter.save();

            QLinearGradient gradient(1, 1, 574, 0);
            gradient.setColorAt(0.0, QColor(255, 255, 255, 180));
            gradient.setColorAt(1.0, Qt::transparent);

            painter.fillRect(QRect(1, 1, 574, 16), gradient);
            painter.setPen(Qt::black);
            painter.drawText(5, 12, m_txt);

            painter.restore();
        }
    }
};

GtSplashScreen::GtSplashScreen(bool showBorder, bool showMessage)
    : QWidget(nullptr, Qt::SplashScreen | Qt::FramelessWindowHint)
{
    auto mainLay = new QVBoxLayout;
    mainLay->setContentsMargins({0, 0, 0, 0});

    m_processLabel = new SplashLabel(this);
    m_movie = new QMovie(":pixmaps/splash_anim.gif", QByteArray(), m_processLabel);
    m_processLabel->resize({576, 324});
    m_processLabel->m_showMessage = showMessage;
    mainLay->addWidget(m_processLabel);

    m_processLabel->setMovie(m_movie);
    m_movie->start();

    if (showBorder)
    {
        m_processLabel->setStyleSheet("border: 1px solid black;");
    }

    setLayout(mainLay);
}

void
GtSplashScreen::showMessage(const QString& msg)
{
    m_processLabel->m_txt = msg;
    QCoreApplication::processEvents();
}

inline static bool
waitForWindowExposed(QWindow* window, int timeout = 1000)
{
    enum { TimeOutMs = 10 };
    QElapsedTimer timer;
    timer.start();
    while (!window->isExposed()) {
        const int remaining = timeout - int(timer.elapsed());
        if (remaining <= 0)
            break;
        QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
#if defined(Q_OS_WINRT)
        WaitForSingleObjectEx(GetCurrentThread(), TimeOutMs, false);
#elif defined(Q_OS_WIN)
        Sleep(uint(TimeOutMs));
#else
        struct timespec ts = { TimeOutMs / 1000, (TimeOutMs % 1000) * 1000 * 1000 };
        nanosleep(&ts, nullptr);
#endif
    }
    return window->isExposed();
}

void
GtSplashScreen::finish(QWidget* wid)
{
    if (wid)
    {
        if (!wid->windowHandle())
        {
            wid->createWinId();
        }

        waitForWindowExposed(wid->windowHandle());
    }

    close();
}

void
GtSplashScreen::processEvents(int msecs)
{
    QTime dieTime= QTime::currentTime().addMSecs(msecs);

    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
