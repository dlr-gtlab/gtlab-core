/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_collectionsdirectoriestab.h"
#include "ui_gt_collectionsdirectoriestab.h"

#include "gt_icons.h"
#include "gt_logging.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QSignalBlocker>
#include <QUrl>

GtCollectionsDirectoriesTab::GtCollectionsDirectoriesTab(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GtCollectionsDirectoriesTab)
{
    ui->setupUi(this);

    ui->btnCollectionsDirShowInExplorer->setIcon(gt::gui::icon::folderEye());
    ui->btnCollectionsDirPathReset->setIcon(gt::gui::icon::revert());

    ui->btnCollectionsDirPathSelect->setToolTip(
        QObject::tr("Select directory"));
    ui->btnCollectionsDirShowInExplorer->setToolTip(
        QObject::tr("Open directory in explorer"));
    ui->btnCollectionsDirPathReset->setToolTip(QObject::tr("Reset to default"));

    connect(ui->btnCollectionsDirShowInExplorer, &QToolButton::clicked, this,
            &GtCollectionsDirectoriesTab::onCollectionsDirShowInExplorer);

    connect(ui->btnCollectionsDirPathReset, &QToolButton::clicked, this,
            &GtCollectionsDirectoriesTab::onCollectionsDirResetDefault);

    connect(ui->btnCollectionsDirPathSelect, &QToolButton::clicked, this,
            &GtCollectionsDirectoriesTab::onBtnCollectionsDirSelectPressed);

    connect(ui->editCollectionsDirPath, &QLineEdit::textChanged, this,
            &GtCollectionsDirectoriesTab::onEditCollectionsDirPathEdited);
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
    setCollectionsPath(m_defaultPath, m_defaultPath);
}

void
GtCollectionsDirectoriesTab::onBtnCollectionsDirSelectPressed()
{
    const QString selectedDir = QFileDialog::getExistingDirectory(
        this, tr("Select Collections Install Directory"), collectionsPath(),
        QFileDialog::ShowDirsOnly);

    if (!selectedDir.isEmpty())
    {
        ui->editCollectionsDirPath->setText(
            QDir::toNativeSeparators(selectedDir));
    }
}

void
GtCollectionsDirectoriesTab::onEditCollectionsDirPathEdited()
{
    const auto path = collectionsPath();
    const QDir directory{path};

    if (!directory.exists())
    {
        const bool missingDefault = !m_defaultPath.isEmpty() &&
                                    directory == QDir{m_defaultPath} &&
                                    !QFileInfo::exists(path);
        if (!missingDefault)
        {
            setCollectionPathsError(QObject::tr("Directory does not exist"));
            return;
        }
    }

    setCollectionPathsError({});
}

QString
GtCollectionsDirectoriesTab::collectionsPath() const
{
    return QDir::fromNativeSeparators(ui->editCollectionsDirPath->text());
}

void
GtCollectionsDirectoriesTab::setCollectionsPath(const QString& dir,
                                                const QString& defaultPath)
{
    m_defaultPath = defaultPath;
    const QSignalBlocker blocker{ui->editCollectionsDirPath};
    ui->editCollectionsDirPath->setText(QDir::toNativeSeparators(dir));
    onEditCollectionsDirPathEdited();
}

void
GtCollectionsDirectoriesTab::setCollectionPathsError(const QString& msg)
{
    m_pathError = msg;
    ui->editCollectionsDirPath->setPlaceholderText(msg);
    ui->editCollectionsDirPath->setStyleSheet(
        msg.isEmpty() ? QString{} : QStringLiteral("color: red;"));
    updatePathToolTip();
}

void
GtCollectionsDirectoriesTab::updatePathToolTip()
{
    auto* textbox = ui->editCollectionsDirPath;
    const QString fullPath = textbox->text();
    QString toolTip = fullPath.isEmpty() ? m_pathError : fullPath;
    if (!fullPath.isEmpty() && !m_pathError.isEmpty())
    {
        toolTip += QLatin1Char('\n') + m_pathError;
    }
    textbox->setToolTip(toolTip);
    textbox->setAccessibleDescription(toolTip);
}
