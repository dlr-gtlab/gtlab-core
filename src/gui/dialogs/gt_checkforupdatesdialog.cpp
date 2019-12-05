/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkforupdatesdialog.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QFrame>
#include <QPushButton>
#include <QMovie>
#include <QThread>

#include "gt_application.h"
#include "gt_updatechecker.h"
#include "gt_logging.h"

#include "gt_checkforupdatesdialog.h"

GtCheckForUpdatesDialog::GtCheckForUpdatesDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Check for updates"));
    setWindowIcon(gtApp->icon(QStringLiteral("updateIcon_16.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel* titleLabel = new QLabel;

    titleLabel->setPixmap(QPixmap(
                              QStringLiteral(":/pixmaps/updateLogo.png")));

    layout->addWidget(titleLabel);

    m_progressLabel = new QLabel;
    QMovie* movie = new QMovie(QStringLiteral(":/pixmaps/progressbar.gif"),
                               QByteArray(), this);
    m_progressLabel->setMovie(movie);
    movie->start();
    m_progressLabel->setVisible(true);

    layout->addWidget(m_progressLabel);

    QFrame* line = new QFrame;
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout->addWidget(line);

    QVBoxLayout* textLay = new QVBoxLayout;
    textLay->setContentsMargins(9, 9, 9, 9);

    m_txtLabel = new QLabel;
    m_txtLabel->setWordWrap(true);

    m_txtLabel->setText(tr("You are already using the most recent "
                           "version of GTlab.\n"
                           "Please check again in the future."));

    textLay->addWidget(m_txtLabel);

    layout->addLayout(textLay);

    layout->addSpacerItem(new QSpacerItem(10, 10,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding));

    QHBoxLayout* hLay = new QHBoxLayout;

    m_checkButton = new QPushButton;
    m_checkButton->setText(tr("Check Again"));
    m_checkButton->setIcon(gtApp->icon(QStringLiteral("loopIcon_16.png")));
    connect(m_checkButton, SIGNAL(clicked(bool)), SLOT(checkForUpdate()));

    hLay->addWidget(m_checkButton);

    hLay->addSpacerItem(new QSpacerItem(10, 10,
                                        QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));

    m_updateButton = new QPushButton;
    m_updateButton->setText(tr("Update"));
    m_updateButton->setIcon(gtApp->icon(QStringLiteral("updateIcon_16.png")));
    m_updateButton->setEnabled(false);
    connect(m_updateButton, SIGNAL(clicked(bool)), SLOT(accept()));

    hLay->addWidget(m_updateButton);

    QPushButton* cancelButton = new QPushButton;
    cancelButton->setText(tr("Cancel"));
    cancelButton->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));
    connect(cancelButton, SIGNAL(clicked(bool)), SLOT(reject()));

    hLay->addWidget(cancelButton);

    hLay->setContentsMargins(9, 9, 9, 9);

    layout->addLayout(hLay);

    setLayout(layout);

    setMaximumWidth(350);
    setMinimumHeight(210);
    setMaximumHeight(210);

    checkForUpdate();
}

void
GtCheckForUpdatesDialog::checkForUpdate()
{
    gtInfo() << "checking for updates...";

    m_txtLabel->setText(tr("searching..."));
    m_progressLabel->setVisible(true);
    m_checkButton->setEnabled(false);
    m_updateButton->setEnabled(false);

    QThread* thread = new QThread;
    GtUpdateChecker* check = new GtUpdateChecker;
    check->moveToThread(thread);

    connect(thread, SIGNAL(started()), check, SLOT(checkForUpdate()));
    connect(check, SIGNAL(updateAvailable()), thread, SLOT(quit()));
    connect(check, SIGNAL(updateAvailable()), check, SLOT(deleteLater()));
    connect(check, SIGNAL(updateAvailable()), this, SLOT(updateAvailable()));
    connect(check, SIGNAL(error(int, QString)), thread, SLOT(quit()));
    connect(check, SIGNAL(error(int, QString)),
            SLOT(noUpdateAvailable(int, QString)));
    connect(check, SIGNAL(error(int, QString)), check, SLOT(deleteLater()));
    connect(this, SIGNAL(finished(int)), thread, SLOT(terminate()));

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void
GtCheckForUpdatesDialog::updateAvailable()
{
    m_progressLabel->setVisible(false);
    m_updateButton->setEnabled(true);
    m_checkButton->setEnabled(true);
    m_txtLabel->setText("<b><font color='darkgreen'>" +
                        tr("New updates available!") +
                        "</font></b>");
}

void
GtCheckForUpdatesDialog::noUpdateAvailable(int errorCode, const QString& str)
{
    m_progressLabel->setVisible(false);
    m_updateButton->setEnabled(false);
    m_checkButton->setEnabled(true);

    gtDebug() << "Error code = " << errorCode;
    gtDebug() << "Error message = " << str;

    switch (errorCode)
    {
    case 1:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Maintenance Tool not found!") + "</font>");
        break;
    case 2:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Timed out!") + "</font>");
        break;
    case 3:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Update file not found!") + "</font>");
        break;
    case 4:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Could not open update file!") + "</font>");
        break;
    case 5:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Connection refused!") + "<br>" +
                            tr("Please check yout internet"
                            " connection.") + "</font>");
        break;
    case 6:
        m_txtLabel->setText(tr("You are already using the most recent "
                               "version of GTlab.\n"
                               "Please check again in the future."));
        break;
    case 7:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Failure!") + "<br>" + tr("Please try again"
                            " later.") + "</font>");
        break;
    case 8:
        m_txtLabel->setText("<font color='darkred'>" +
                            tr("Error!") + "<br>" + tr("Please try again"
                            " later.") + "</font>");
        break;
    default:
        break;
    }
}
