/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_collectionsdirectoriestab.h"
#include "ui_gt_collectionsdirectoriestab.h"
#include "gt_logging.h"

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QFileDialog>

GtCollectionsDirectoriesTab::GtCollectionsDirectoriesTab(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GtCollectionsDirectoriesTab)
{
    ui->setupUi(this);

    connect(ui->btnCollectionsDirShowInExplorer, &QPushButton::clicked, this,
            &GtCollectionsDirectoriesTab::onCollectionsDirShowInExplorer);

    connect(ui->btnCollectionsDirPathReset, &QPushButton::clicked, this,
            &GtCollectionsDirectoriesTab::onCollectionsDirResetDefault);

    connect(ui->btnCollectionsDirPathSelect, &QPushButton::clicked, this,
            &GtCollectionsDirectoriesTab::onBtnCollectionsDirSelectPressed);
}

GtCollectionsDirectoriesTab::~GtCollectionsDirectoriesTab()
{
    delete ui;
}

void
GtCollectionsDirectoriesTab::onCollectionsDirShowInExplorer()
{
    QFileInfo fi(collectionsPath());
    if (!fi.exists())
    {
        gtError() << "Path does not exist:" << collectionsPath();
        return;
    }

    // If a file is passed, open its parent folder; if a dir, open it directly.
    const QString target =
        fi.isDir() ? fi.canonicalFilePath() : fi.absolutePath();

    QDesktopServices::openUrl(QUrl::fromLocalFile(target));
}

void
GtCollectionsDirectoriesTab::onCollectionsDirResetDefault()
{
    ui->edtCollectionsDirPath->setText(m_defaultPath);
    ui->lblCollectionsDirMsg->setStyleSheet("");
    ui->edtCollectionsDirPath->setStyleSheet("");
    ui->lblCollectionsDirMsg->setText("");
}

void
GtCollectionsDirectoriesTab::onBtnCollectionsDirSelectPressed()
{
    auto currPath = ui->edtCollectionsDirPath->text();


    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        "Select Collections Install Directory",
        currPath,
        QFileDialog::ShowDirsOnly
        );

    if (!selectedDir.isEmpty()) {
        setCollectionsPath(selectedDir, m_defaultPath);
    }
}

const QString
GtCollectionsDirectoriesTab::collectionsPath()
{
    return ui->edtCollectionsDirPath->text();
}

void
GtCollectionsDirectoriesTab::setCollectionsPath(const QString& dir, const QString& defaultPath)
{
    ui->edtCollectionsDirPath->setText(dir);
    m_defaultPath = defaultPath;
}

void
GtCollectionsDirectoriesTab::setCollectionPathsError(const QString &msg)
{
    ui->lblCollectionsDirMsg->setText(msg);

    if (!msg.isEmpty())
    {
        ui->lblCollectionsDirMsg->setStyleSheet("color: red;");
        ui->edtCollectionsDirPath->setStyleSheet("color: red;");
    }
    else
    {
        ui->lblCollectionsDirMsg->setStyleSheet("");
        ui->edtCollectionsDirPath->setStyleSheet("");
    }
}
