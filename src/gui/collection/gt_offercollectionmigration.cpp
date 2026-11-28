/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_offercollectionmigration.h"

#include "gt_application.h"
#include "gt_applicationpaths.h"
#include "gt_collectionsinstallpath.h"
#include "gt_migratecollectionsdata.h"
#include "gt_settings.h"

#include "gt_logging.h"

#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QLockFile>
#include <QMessageBox>
#include <QObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSaveFile>
#include <QStringList>
#include <QVBoxLayout>


bool
gt::detail::collectionmigration::offerMigration(GtSettings& appSettings)
{
    const QString title = QObject::tr("Migrate Collections");

    if (gt::detail::collectionmigration::settingToState(appSettings) ==
        gt::detail::collectionmigration::State::Pending)
    {
        auto ret = gt::detail::collectionmigration::legacyPath();
        if (!ret)
        {
            return false;
        }
        QDir legacy(ret.value());
        const QFileInfo legacyInfo(ret.value());

        if ((legacyInfo.isSymLink() || legacyInfo.isDir()) &&
            canonicalMigrationSource(ret.value()).isEmpty())
        {
            gtError() << GtApplication::tr(
                             "Cannot migrate collections. The legacy folder "
                             "must be a directory at its original location: %1")
                             .arg(QDir::toNativeSeparators(ret.value()));
            return false;
        }

        if (!legacy.exists() ||
            legacy.isEmpty(QDir::Files | QDir::Dirs | QDir::Hidden |
                           QDir::System | QDir::NoDotAndDotDot))
        {
            appSettings.setCollectionMigrationState(
                gt::detail::collectionmigration::State::Completed);
            return true;
        }

        auto lock = gt::detail::collectionmigration::lockFile();
        if (!lock.has_value())
        {
            gtError() << QObject::tr(
                "Could not provide a lock file for collection migration");
            return false;
        }
        const QString lockPath = lock->absoluteFilePath();
        QLockFile lockCheck(lockPath);
        lockCheck.setStaleLockTime(0);

        while (!lockCheck.tryLock(0))
        {
            if (lockCheck.error() != QLockFile::LockFailedError)
            {
                gtError()
                    << GtApplication::tr(
                           "Could not check the collection migration lock: %1")
                           .arg(QDir::toNativeSeparators(lockPath));
                return false;
            }

            QMessageBox busy(QMessageBox::Information, title, QString{},
                             QMessageBox::Ok | QMessageBox::Retry);
            busy.setTextFormat(Qt::PlainText);
            busy.setText(
                GtApplication::tr(
                    "Another GTlab instance is migrating collections.\n\n"
                    "Lock file: %1\n\n"
                    "Select OK to continue without migrating, or Retry "
                    "to check again.")
                    .arg(QDir::toNativeSeparators(lockPath)));
            busy.setDefaultButton(QMessageBox::Ok);
            busy.setEscapeButton(QMessageBox::Ok);
            if (busy.exec() == QMessageBox::Ok)
            {
                return true;
            }
        }
        lockCheck.unlock();

        QMessageBox prompt(QMessageBox::Question, title, QString{},
                           QMessageBox::Yes | QMessageBox::No);
        prompt.setTextFormat(Qt::PlainText);
        prompt.setText(
            GtApplication::tr(
                "GTlab found collections in the old installation folder. "
                "Copy them to the default Collections folder?\n\n%1")
                .arg(collectionsInstallPath(false)));
        const auto* neverAsk = prompt.addButton(
            QObject::tr("No, and don't ask again"), QMessageBox::RejectRole);
        prompt.setDefaultButton(QMessageBox::No);
        prompt.setEscapeButton(QMessageBox::No);

        auto answer = prompt.exec();

        if (answer == QMessageBox::Yes)
        {
            bool destinationValid = false;
            const QString destination =
                collectionsInstallPath(false, &destinationValid);
            if (!destinationValid) return false;

            if (!gt::detail::collectionmigration::isMigrationDestinationSafe(
                    legacy.absolutePath(), destination))
            {
                QMessageBox error(QMessageBox::Critical, title, QString{},
                                  QMessageBox::Ok);
                error.setTextFormat(Qt::PlainText);
                error.setText(
                    GtApplication::tr(
                        "Collection migration is not possible. Choose a "
                        "destination "
                        "outside the legacy Collections folder.\n\n"
                        "Legacy Collections folder\n%1\n\n"
                        "Configured destination\n%2")
                        .arg(QDir::toNativeSeparators(legacy.absolutePath()),
                             QDir::toNativeSeparators(destination)));
                // The error has been displayed. Continue startup.
                error.exec();
                return true;
            }

            QString migratedDestination;
            if (!gt::detail::collectionmigration::runMigration(
                    &migratedDestination))
            {
                return false;
            }

            appSettings.setCollectionMigrationState(
                gt::detail::collectionmigration::State::Completed);

            QMessageBox completed(QMessageBox::Information, title, QString{},
                                  QMessageBox::Ok);
            completed.setTextFormat(Qt::PlainText);
            completed.setText(
                GtApplication::tr(
                    "Collection migration is complete. You can now delete the "
                    "old Collections folder.\n\n"
                    "Old location\n%1\n\n"
                    "New location\n%2")
                    .arg(QDir::toNativeSeparators(legacy.absolutePath()),
                         QDir::toNativeSeparators(migratedDestination)));
            completed.exec();
        }
        else if (prompt.clickedButton() == neverAsk)
        {
            appSettings.setCollectionMigrationState(
                gt::detail::collectionmigration::State::Suppressed);
        }
    }
    return true;
}
