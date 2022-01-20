/* GTlab - Gas Turbine laboratory
 * Source File: gt_downloaddialog.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DOWNLOADDIALOG_H
#define GT_DOWNLOADDIALOG_H

#include <QDialog>

class QTextEdit;
class QProgressBar;
class GtDownloader;

/**
 * @brief The GtDownloadDialog class
 */
class GtDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtDownloadDialog
     * @param parent
     */
    GtDownloadDialog(QWidget* parent = Q_NULLPTR);

    /**
     * @brief addFileDownload
     * @param url
     * @param downloadPath
     * @param targetFileName
     */
    void addFileDownload(const QString& url, const QString& downloadPath,
                         QString targetFileName = QString());

    /**
     * @brief GtDownloadItem
     * @param url
     * @param downloadPath
     * @param targetFileName
     */
    void addFileDownload(const QUrl& url, const QString& downloadPath,
                         QString targetFileName = QString());

protected:
    /**
     * @brief showEvent
     * @param event
     */
    void showEvent(QShowEvent * event) Q_DECL_OVERRIDE;

private:
    /// Downloader
    GtDownloader* m_downloader;

    /// Text edit.
    QTextEdit* m_textEdit;

    /// Progress bar.
    QProgressBar* m_progressBar;

    /// Ok button.
    QPushButton* m_okButton;

    /// Cancel button.
    QPushButton* m_cancelButton;

private slots:
    /**
     * @brief cancelDownloadProcess
     */
    void cancelDownloadProcess();

    /**
     * @brief onDownloaderMessage
     */
    void onDownloaderMessage(QString str);

    /**
     * @brief onDownloaderFinished
     */
    void onDownloaderFinished();

    /**
     * @brief onQueueChanged
     * @param finished
     * @param total
     */
    void onQueueChanged(int finished, int total);

};

#endif // GT_DOWNLOADDIALOG_H
