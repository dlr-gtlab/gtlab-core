/* GTlab - Gas Turbine laboratory
 * Source File: gt_downloaddialog.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>

#include "gt_downloader.h"
#include "gt_application.h"

#include "gt_downloaddialog.h"

GtDownloadDialog::GtDownloadDialog(QWidget* parent) : QDialog(parent),
    m_downloader(new GtDownloader(this))
{
    setWindowTitle(tr("Downloader"));
    setWindowIcon(gtApp->icon(QStringLiteral("dropDownIcon.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);


    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel* titleLabel = new QLabel;

    titleLabel->setPixmap(QPixmap(
                              QStringLiteral(":/pixmaps/downloaderLogo.png")));

    layout->addWidget(titleLabel);

    m_progressBar = new QProgressBar;
    m_progressBar->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_progressBar);

    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);

    layout->addWidget(m_textEdit);

    QHBoxLayout* hLay = new QHBoxLayout;

    QLabel* errLabel = new QLabel;

    hLay->addWidget(errLabel);

    hLay->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));

    m_okButton = new QPushButton(
                gtApp->icon(QStringLiteral("checkIcon_16.png")), tr("Ok"));
    m_okButton->setVisible(false);

    hLay->addWidget(m_okButton);

    m_cancelButton =
            new QPushButton(gtApp->icon(QStringLiteral("closeIcon_16.png")),
                            tr("Cancel"));

    hLay->addWidget(m_cancelButton);

    hLay->setContentsMargins(4, 0, 4, 4);

    layout->addLayout(hLay);

    setLayout(layout);

    setMaximumWidth(350);

    connect(m_downloader, SIGNAL(message(QString)),
            SLOT(onDownloaderMessage(QString)));
    connect(m_downloader, SIGNAL(finished()), SLOT(onDownloaderFinished()));
    connect(m_downloader, SIGNAL(queueChanged(int,int)),
            SLOT(onQueueChanged(int,int)));
    connect(m_okButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked(bool)),
            SLOT(cancelDownloadProcess()));
}

void
GtDownloadDialog::addFileDownload(const QString& url,
                                  const QString& downloadPath,
                                  QString targetFileName)
{
    m_downloader->addFileDownload(url, downloadPath, targetFileName);
}

void
GtDownloadDialog::addFileDownload(const QUrl& url,
                                  const QString& downloadPath,
                                  QString targetFileName)
{
    m_downloader->addFileDownload(url, downloadPath, targetFileName);
}

void
GtDownloadDialog::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    m_downloader->exec();
}

void
GtDownloadDialog::cancelDownloadProcess()
{
    m_downloader->cancelDownload();
}

void
GtDownloadDialog::onDownloaderMessage(const QString& str)
{
    QTextCursor cur = m_textEdit->textCursor();
    cur.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(cur);
    m_textEdit->insertPlainText(str + QStringLiteral("\n"));
    cur = m_textEdit->textCursor();
    cur.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(cur);
}

void
GtDownloadDialog::onDownloaderFinished()
{
    m_progressBar->setValue(m_progressBar->maximum());
    m_okButton->setVisible(true);
    m_cancelButton->setVisible(false);
}

void
GtDownloadDialog::onQueueChanged(int finished, int total)
{
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(finished);
}
