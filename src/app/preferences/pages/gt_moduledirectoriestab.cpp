/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 */

#include "gt_moduledirectoriestab.h"
#include "ui_gt_moduledirectoriestab.h"

#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

#include <algorithm>

#include <gt_icons.h>
#include <gt_logging.h>

GtModuleDirectoriesTab::GtModuleDirectoriesTab(QWidget* parent) :
    QWidget(parent), ui(new Ui::GtModuleDirectoriesTab)
{
    ui->setupUi(this);

    ui->btnAddDirectory->setIcon(gt::gui::icon::folderAdd());
    ui->btnRemoveDirectory->setIcon(gt::gui::icon::remove());
    ui->btnMoveUp->setIcon(gt::gui::icon::arrowUp());
    ui->btnMoveDown->setIcon(gt::gui::icon::arrowDown());

    ui->btnAddDirectory->setToolTip(tr("Add directory"));
    ui->btnRemoveDirectory->setToolTip(tr("Remove directory"));
    ui->btnMoveUp->setToolTip(tr("Move up"));
    ui->btnMoveDown->setToolTip(tr("Move down"));

    // Allow inline editing for user rows; F2, double-click, or selected-click
    ui->directoriesList->setEditTriggers(QAbstractItemView::EditKeyPressed |
                                         QAbstractItemView::SelectedClicked |
                                         QAbstractItemView::DoubleClicked);

    // Right-click context menu
    ui->directoriesList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->directoriesList->setUniformItemSizes(true);
    ui->directoriesList->setIconSize(QSize(20, 20));

    connect(ui->directoriesList, &QListWidget::customContextMenuRequested, this,
            &GtModuleDirectoriesTab::onListContextMenu);

    // Prepare two header items (greyed & disabled)
    ensureHeaderItems();

    // Wire buttons
    connect(ui->btnAddDirectory, &QToolButton::clicked, this,
            &GtModuleDirectoriesTab::onAddDirectory);
    connect(ui->btnRemoveDirectory, &QToolButton::clicked, this,
            &GtModuleDirectoriesTab::onRemoveDirectory);
    connect(ui->btnMoveUp, &QToolButton::clicked, this,
            &GtModuleDirectoriesTab::onMoveUp);
    connect(ui->btnMoveDown, &QToolButton::clicked, this,
            &GtModuleDirectoriesTab::onMoveDown);

    // Keep remove button state in sync
    connect(ui->directoriesList, &QListWidget::itemSelectionChanged, this,
            &GtModuleDirectoriesTab::onSelectionChanged);

    // Guard against accidental edits (only matters if edit triggers somehow)
    connect(ui->directoriesList, &QListWidget::itemChanged, this,
            &GtModuleDirectoriesTab::onItemChanged);

    updateRemoveButton();
}

GtModuleDirectoriesTab::~GtModuleDirectoriesTab()
{
    delete ui;
}

void
GtModuleDirectoriesTab::ensureHeaderItems()
{
    auto infoIcon = gt::gui::icon::info();

    // Remove any existing header rows (identified by no-item flags)
    for (int i = ui->directoriesList->count() - 1; i >= 0; --i)
    {
        auto* it = ui->directoriesList->item(i);
        if (it && it->flags() == Qt::NoItemFlags)
        {
            delete ui->directoriesList->takeItem(i);
        }
    }

    // Append two header items at the bottom
    for (int row = 0; row < 2; ++row)
    {
        auto* it = new QListWidgetItem;
        it->setFlags(Qt::NoItemFlags); // not selectable/editable
        it->setForeground(Qt::gray);
        it->setCheckState(Qt::Checked);
        it->setIcon(infoIcon);
        ui->directoriesList->addItem(it);
    }

    const int userRow = ui->directoriesList->count() - 2;
    const int installRow = ui->directoriesList->count() - 1;

    setHeaderText(userRow,
                  m_defaultUserPath.isEmpty()
                      ? tr("<default user modules path>")
                      : m_defaultUserPath,
                  tr("User module directory - always enabled, searched before the application directory"));
    setHeaderText(installRow,
                  m_defaultInstallPath.isEmpty() ? tr("<install modules path>")
                                                 : m_defaultInstallPath,
                  tr("Application module directory - always enabled, searched after the user directory"));
}

void
GtModuleDirectoriesTab::setHeaderText(int row, const QString& text,
                                      const QString& about)
{
    if (row < 0 || row >= 2)
        return;
    auto* it = ui->directoriesList->item(row);
    if (!it)
        return;
    it->setText(text);
    it->setToolTip(about);
}

bool
GtModuleDirectoriesTab::isHeaderRow(int row) const
{
    const int count = ui->directoriesList->count();
    if (count < 2)
        return false;
    return row >= count - 2;
}

void
GtModuleDirectoriesTab::setDefaultInstallPath(const QString& path)
{
    m_defaultInstallPath = QDir::toNativeSeparators(QDir(path).absolutePath());
    ensureHeaderItems();
}

void
GtModuleDirectoriesTab::setDefaultUserPath(const QString& path)
{
    m_defaultUserPath = QDir::toNativeSeparators(QDir(path).absolutePath());
    ensureHeaderItems();
}

QStringList
GtModuleDirectoriesTab::userPaths() const
{
    QStringList out;
    const int count = ui->directoriesList->count();
    const int lastUserRow = count - 3;
    for (int i = 0; i <= lastUserRow; ++i)
    {
        auto* item = ui->directoriesList->item(i);
        auto path = item->text();
        if (item->checkState() == Qt::Unchecked)
            path = '#' + path;
        out << path;
    }
    return out;
}

void
GtModuleDirectoriesTab::setUserPaths(const QStringList& paths)
{
    m_blockSignals = true;

    auto usericon = gt::gui::icon::folderEye();

    // Remove all existing items
    while (ui->directoriesList->count() > 0)
        delete ui->directoriesList->takeItem(0);

    // Add new ones (deduplicated, normalized)
    QSet<QString> seen;
    for (QString p : paths)
    {
        bool enabled = true;
        if (p.startsWith('#'))
        {
            enabled = false;
            p = p.mid(1);
        }

        if (p.isEmpty())
            continue;

        QString norm = QDir::toNativeSeparators(QDir(p).absolutePath());

        if (norm.isEmpty() || seen.contains(norm))
            continue;

        seen.insert(norm);

        auto* it = new QListWidgetItem(norm);
        it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                     Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        it->setIcon(usericon);
        it->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
        applyUserItemAppearance(it);

        ui->directoriesList->addItem(it);
    }

    ensureHeaderItems();

    m_blockSignals = false;
    updateRemoveButton();
    emit userPathsChanged(userPaths());
}

void
GtModuleDirectoriesTab::onAddDirectory()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select Module Directory"), QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    const QString norm = QDir::toNativeSeparators(QDir(dir).absolutePath());

    // Prevent duplicates (including headers)
    for (int i = 0; i < ui->directoriesList->count(); ++i)
    {
        if (ui->directoriesList->item(i)->text().compare(
                norm, Qt::CaseInsensitive) == 0)
        {
            ui->directoriesList->setCurrentRow(i);
            updateRemoveButton();
            return;
        }
    }

    auto usericon = gt::gui::icon::folderEye();

    auto* it = new QListWidgetItem(norm);
    it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable |
                 Qt::ItemIsUserCheckable);
    it->setCheckState(Qt::Checked); // enabled by default
    it->setIcon(usericon);
    const int insertRow = ui->directoriesList->count() - 2;
    ui->directoriesList->insertItem(std::max(0, insertRow), it);
    ui->directoriesList->setCurrentItem(it);

    emit userPathsChanged(userPaths());
    updateRemoveButton();
}

void
GtModuleDirectoriesTab::onRemoveDirectory()
{
    const auto selected = ui->directoriesList->selectedItems();
    if (selected.isEmpty())
        return;

    // Remove only user-defined rows (headers are locked)
    for (QListWidgetItem* it : selected)
    {
        const int row = ui->directoriesList->row(it);
        if (!isHeaderRow(row))
            delete ui->directoriesList->takeItem(row);
    }

    emit userPathsChanged(userPaths());
    updateRemoveButton();
}

void
GtModuleDirectoriesTab::onMoveUp()
{
    const auto selected = ui->directoriesList->selectedItems();
    if (selected.isEmpty())
        return;

    QVector<int> rows;
    QSet<QListWidgetItem*> selectedSet;
    for (auto* it : selected)
    {
        selectedSet.insert(it);
        const int row = ui->directoriesList->row(it);
        if (!isHeaderRow(row))
            rows.push_back(row);
    }

    if (rows.isEmpty())
        return;

    std::sort(rows.begin(), rows.end());

    for (int row : rows)
    {
        if (row <= 0)
            continue;
        if (isHeaderRow(row))
            continue;
        auto* item = ui->directoriesList->takeItem(row);
        ui->directoriesList->insertItem(row - 1, item);
    }

    ui->directoriesList->clearSelection();
    for (auto* it : selectedSet)
        it->setSelected(true);

    emit userPathsChanged(userPaths());
    updateRemoveButton();
}

void
GtModuleDirectoriesTab::onMoveDown()
{
    const auto selected = ui->directoriesList->selectedItems();
    if (selected.isEmpty())
        return;

    QVector<int> rows;
    QSet<QListWidgetItem*> selectedSet;
    for (auto* it : selected)
    {
        selectedSet.insert(it);
        const int row = ui->directoriesList->row(it);
        if (!isHeaderRow(row))
            rows.push_back(row);
    }

    if (rows.isEmpty())
        return;

    std::sort(rows.begin(), rows.end(), std::greater<int>());

    const int lastUserRow = ui->directoriesList->count() - 3;
    for (int row : rows)
    {
        if (row < 0 || row >= lastUserRow)
            continue;
        if (isHeaderRow(row))
            continue;
        auto* item = ui->directoriesList->takeItem(row);
        ui->directoriesList->insertItem(row + 1, item);
    }

    ui->directoriesList->clearSelection();
    for (auto* it : selectedSet)
        it->setSelected(true);

    emit userPathsChanged(userPaths());
    updateRemoveButton();
}

void
GtModuleDirectoriesTab::onSelectionChanged()
{
    updateRemoveButton();
}

void
GtModuleDirectoriesTab::onItemChanged(QListWidgetItem* item)
{
    if (!item)
        return;
    const int row = ui->directoriesList->row(item);

    // Protect headers
    const int count = ui->directoriesList->count();
    const int installRow = count - 2;
    const int userRow = count - 1;
    if (row == installRow)
    {
        item->setText(m_defaultInstallPath);
        return;
    }
    if (row == userRow)
    {
        item->setText(m_defaultUserPath);
        return;
    }

    // Always refresh appearance (covers checkbox toggles)
    applyUserItemAppearance(item);

    // If the text changed: normalize / dedupe / drop empties
    QString txt = item->text().trimmed();
    if (txt.isEmpty())
    {
        delete ui->directoriesList->takeItem(row);
        emit userPathsChanged(userPaths());
        return;
    }

    const QString norm = QDir::toNativeSeparators(QDir(txt).absolutePath());
    if (norm != item->text())
        item->setText(norm);

    // Prevent duplicates (including vs headers)
    for (int i = 0; i < ui->directoriesList->count(); ++i)
    {
        if (i == row)
            continue;
        if (ui->directoriesList->item(i)->text().compare(
                norm, Qt::CaseInsensitive) == 0)
        {
            ui->directoriesList->setCurrentRow(i);
            if (norm != item->text())
                item->setText(norm);
            return;
        }
    }

    emit userPathsChanged(userPaths());
}


void
GtModuleDirectoriesTab::updateRemoveButton()
{
    bool canRemove = false;
    const auto selectedItems = ui->directoriesList->selectedItems();
    for (const QListWidgetItem* it : selectedItems)
    {
        if (!isHeaderRow(ui->directoriesList->row(it)))
        {
            canRemove = true;
            break;
        }
    }
    ui->btnRemoveDirectory->setEnabled(canRemove);
    updateMoveButtons();
}

void
GtModuleDirectoriesTab::updateMoveButtons()
{
    bool canMoveUp = false;
    bool canMoveDown = false;

    const int count = ui->directoriesList->count();
    const int lastUserRow = count - 3;
    const auto selectedItems = ui->directoriesList->selectedItems();
    for (const QListWidgetItem* it : selectedItems)
    {
        const int row = ui->directoriesList->row(it);
        if (isHeaderRow(row))
            continue;
        if (row > 0)
            canMoveUp = true;
        if (row < lastUserRow)
            canMoveDown = true;
    }

    ui->btnMoveUp->setEnabled(canMoveUp);
    ui->btnMoveDown->setEnabled(canMoveDown);
}

void
GtModuleDirectoriesTab::onListContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->directoriesList->itemAt(pos);

    if (item)
    {
        ui->directoriesList->setCurrentItem(item);
    }

    QMenu menu(this);

    QAction* showAct = menu.addAction(tr("Show in Explorer"));
    connect(showAct, &QAction::triggered, this,
            &GtModuleDirectoriesTab::onShowInExplorer);

    // For user-defined rows only (exclude headers)
    const bool userRow = (item && !isHeaderRow(ui->directoriesList->row(item)));

    QAction* editInlineAct = menu.addAction(tr("Rename / Edit Path"));
    editInlineAct->setEnabled(userRow);
    connect(editInlineAct, &QAction::triggered, this,
            &GtModuleDirectoriesTab::onEditItemInline);

    QAction* changeToDirAct = menu.addAction(tr("Change to…"));
    changeToDirAct->setEnabled(userRow);
    connect(changeToDirAct, &QAction::triggered, this,
            &GtModuleDirectoriesTab::onChangeToDirectory);

    menu.exec(ui->directoriesList->viewport()->mapToGlobal(pos));
}

void
GtModuleDirectoriesTab::onShowInExplorer()
{
    auto* item = ui->directoriesList->currentItem();
    if (!item)
        return;

    const QString path = item->text().trimmed();
    if (path.isEmpty())
        return;

    openInFileManager(path);
}

void
GtModuleDirectoriesTab::onEditItemInline()
{
    if (auto* it = currentUserItem())
    {
        ui->directoriesList->editItem(it);
    }
}

void
GtModuleDirectoriesTab::onChangeToDirectory()
{
    auto* it = currentUserItem();
    if (!it)
        return;

    const QString startDir = it->text();
    const QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select Module Directory"),
        startDir.isEmpty() ? QString() : startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    const QString norm = QDir::toNativeSeparators(QDir(dir).absolutePath());

    // prevent duplicates vs all items (including headers)
    for (int i = 0; i < ui->directoriesList->count(); ++i)
    {
        if (ui->directoriesList->item(i)->text().compare(
                norm, Qt::CaseInsensitive) == 0)
        {
            ui->directoriesList->setCurrentRow(i);
            return;
        }
    }

    it->setText(norm);
    emit userPathsChanged(userPaths());
}

bool
GtModuleDirectoriesTab::openInFileManager(const QString& path)
{
    QFileInfo fi(path);
    if (!fi.exists())
        return false;

    // If a file is passed, open its parent folder; if a dir, open it directly.
    const QString target =
        fi.isDir() ? fi.canonicalFilePath() : fi.absolutePath();

    return QDesktopServices::openUrl(QUrl::fromLocalFile(target));
}

QListWidgetItem*
GtModuleDirectoriesTab::currentUserItem() const
{
    auto* it = ui->directoriesList->currentItem();
    if (!it)
        return nullptr;
    const int row = ui->directoriesList->row(it);
    return (row >= 2) ? it : nullptr;
}

void
GtModuleDirectoriesTab::applyUserItemAppearance(QListWidgetItem* item)
{
    if (!item)
        return;
    const bool disabled = (item->checkState() == Qt::Unchecked);

    QFont f = item->font();
    f.setStrikeOut(disabled);
    item->setFont(f);

    item->setToolTip(disabled ? tr("Disabled — this path will be ignored")
                              : tr("Enabled"));
}
