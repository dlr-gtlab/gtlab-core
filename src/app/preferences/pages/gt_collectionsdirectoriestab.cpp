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

GtCollectionsDirectoriesTab::GtCollectionsDirectoriesTab(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GtCollectionsDirectoriesTab)
{
    ui->setupUi(this);

    connect(ui->btnShowDir, &QPushButton::clicked, this,
            &GtCollectionsDirectoriesTab::onShowInExplorer);
}

GtCollectionsDirectoriesTab::~GtCollectionsDirectoriesTab()
{
    delete ui;
}

void
GtCollectionsDirectoriesTab::onShowInExplorer()
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

QString
GtCollectionsDirectoriesTab::collectionsPath()
{
    return ui->edtGtlabEnvValue->text();
}

void
GtCollectionsDirectoriesTab::setCollectionsPath(const QString& dir)
{
    ui->edtGtlabEnvValue->setText(dir);
}
