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
#include <QTableWidget>
#include <QHeaderView>
#include <QFont>

#include "gt_application.h"
#include "gt_updatechecker.h"
#include "gt_logging.h"
#include "gt_icons.h"

#include "gt_checkforupdatesdialog.h"

GtCheckForUpdatesDialog::GtCheckForUpdatesDialog(QWidget* parent) :
    GtDialog(parent)
{
    setWindowTitle(tr("Check for updates"));
    setWindowIcon(gt::gui::icon::update16());

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel* titleLabel = new QLabel;

    titleLabel->setPixmap(gt::gui::pixmap::updateLogo());

    layout->addWidget(titleLabel);

    m_progressLabel = new QLabel;
    auto movie = new QMovie(QStringLiteral(":/pixmaps/progressbar.gif"),
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

    m_tableWid = new QTableWidget;

    layout->addWidget(m_tableWid);

    layout->addSpacerItem(new QSpacerItem(10, 10,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding));

    QHBoxLayout* hLay = new QHBoxLayout;

    m_checkButton = new QPushButton;
    m_checkButton->setText(tr("Check Again"));
    m_checkButton->setIcon(gt::gui::icon::loop16());
    connect(m_checkButton, SIGNAL(clicked(bool)), SLOT(checkForUpdate()));

    hLay->addWidget(m_checkButton);

    hLay->addSpacerItem(new QSpacerItem(10, 10,
                                        QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));

    m_updateButton = new QPushButton;
    m_updateButton->setText(tr("Update"));
    m_updateButton->setIcon(gt::gui::icon::update16());
    m_updateButton->setEnabled(false);
    connect(m_updateButton, SIGNAL(clicked(bool)), SLOT(accept()));

    hLay->addWidget(m_updateButton);

    QPushButton* cancelButton = new QPushButton;
    cancelButton->setText(tr("Cancel"));
    cancelButton->setIcon(gt::gui::icon::delete16());
    connect(cancelButton, SIGNAL(clicked(bool)), SLOT(reject()));

    hLay->addWidget(cancelButton);

    hLay->setContentsMargins(9, 9, 9, 9);

    layout->addLayout(hLay);

    setLayout(layout);

    setMaximumWidth(350);
    setMinimumHeight(310);
    setMaximumHeight(310);

    checkForUpdate();
}

void
GtCheckForUpdatesDialog::checkForUpdate()
{
    gtInfo() << tr("Checking for updates...");

    m_tableWid->hide();

    m_txtLabel->setText(tr("searching..."));
    m_progressLabel->setVisible(true);
    m_checkButton->setEnabled(false);
    m_updateButton->setEnabled(false);

    QThread* thread = new QThread;
    GtUpdateChecker* check = new GtUpdateChecker;
    check->enableExtendedInfo(true);
    check->moveToThread(thread);

    connect(thread, SIGNAL(started()), check, SLOT(checkForUpdate()));
    connect(check, SIGNAL(updateAvailable()), thread, SLOT(quit()));
    connect(check, SIGNAL(updateAvailable()), this, SLOT(updateAvailable()));
    connect(check, SIGNAL(error(int,QString)), thread, SLOT(quit()));
    connect(check, SIGNAL(error(int,QString)),
            SLOT(noUpdateAvailable(int,QString)));
    connect(check, SIGNAL(error(int,QString)), check, SLOT(deleteLater()));
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

    QString fontString = "darkgreen";
    if (gtApp->inDarkMode())
    {
        fontString = "green";
    }

    m_txtLabel->setText("<b><font color='" + fontString + "'>" +
                        tr("New updates available!") +
                        "</font></b>");

    GtUpdateChecker* check = qobject_cast<GtUpdateChecker*>(sender());

    if (!check)
    {
        return;
    }

    QList<GtUpdateChecker::PackageInfo> pkgList = check->pkgList();

    m_tableWid->setRowCount(pkgList.count());
    m_tableWid->setColumnCount(3);

    foreach (const GtUpdateChecker::PackageInfo& pkg_info, pkgList)
    {
        m_tableWid->setItem(0, 0, new QTableWidgetItem(pkg_info.m_name));
        m_tableWid->setItem(0, 1, new QTableWidgetItem(pkg_info.m_currentVers));
        m_tableWid->setItem(0, 2, new QTableWidgetItem(pkg_info.m_newVers));
    }

    m_tableWid->setHorizontalHeaderLabels(QStringList() << "Package" <<
                                        "Installed Version" <<
                                        "New Version");
    m_tableWid->verticalHeader()->hide();
    QFont font = m_tableWid->font();
    font.setPointSize(7);
    m_tableWid->setFont(font);

    m_tableWid->setShowGrid(false);

    m_tableWid->setColumnWidth(0, 150);
    m_tableWid->setColumnWidth(1, 95);
    m_tableWid->setColumnWidth(2, 80);
    m_tableWid->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tableWid->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tableWid->verticalHeader()->setDefaultSectionSize(20);

    m_tableWid->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_tableWid->show();

    check->deleteLater();
}

void
GtCheckForUpdatesDialog::noUpdateAvailable(int errorCode, const QString& str)
{
    m_progressLabel->setVisible(false);
    m_updateButton->setEnabled(false);
    m_checkButton->setEnabled(true);

    gtDebug().nospace()
            << "Update check: " << str << " (Error code: " << errorCode << ')';

    QString fontString = "darkred";
    if (gtApp->inDarkMode())
    {
        fontString = "red";
    }

    switch (errorCode)
    {
    case 1:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Maintenance Tool not found!") + "</font>");
        break;
    case 2:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Timed out!") + "</font>");
        break;
    case 3:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Update file not found!") + "</font>");
        break;
    case 4:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Could not open update file!") + "</font>");
        break;
    case 5:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
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
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Failure!") + "<br>" + tr("Please try again"
                            " later.") + "</font>");
        break;
    case 8:
        m_txtLabel->setText("<font color='" + fontString + "'>" +
                            tr("Error!") + "<br>" + tr("Please try again"
                            " later.") + "</font>");
        break;
    default:
        break;
    }
}
