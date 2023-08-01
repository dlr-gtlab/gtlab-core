/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFileDialog>

#include "gt_filedialog.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_logging.h"

GtFileDialog::GtFileDialog()
{

}

QString
GtFileDialog::getOpenFileName(QWidget* parent, const QString& caption,
                              const QString& dir, const QString& filter,
                              QString* selectedFilter)
{
    QString retval = dir;

    if (dir.isEmpty())
    {
        retval = readLastPath();
    }

    retval = QFileDialog::getOpenFileName(parent, caption, retval, filter,
                                          selectedFilter);

    if (!retval.isEmpty())
    {
        writeLastPath(retval);
    }

    return retval;
}

QString
GtFileDialog::getSaveFileName(QWidget* parent,
                              const QString& caption,
                              const QString& dir,
                              const QString& filter,
                              const QString& initFilename,
                              QString* selectedFilter)
{
    QString retval = dir;

    if (dir.isEmpty())
    {
        retval = readLastPath();

        if (!initFilename.isEmpty())
        {
            retval = retval + QDir::separator() + initFilename;
        }
    }

    retval = QFileDialog::getSaveFileName(parent, caption, retval, filter,
                                          selectedFilter);

    if (!retval.isEmpty())
    {
        writeLastPath(retval);
    }

    return retval;
}

QString
GtFileDialog::getExistingDirectory(QWidget* parent, const QString& caption,
                                   const QString& dir)
{
    QString retval = dir;

    if (dir.isEmpty())
    {
        retval = readLastPath();
    }

    retval = QFileDialog::getExistingDirectory(parent, caption, retval);

    if (!retval.isEmpty())
    {
        writeLastPath(retval);
    }

    return retval;
}

QString
GtFileDialog::readLastPath()
{
    QString lastPath = gtApp->settings()->lastPath();

    gtDebug().medium() << QObject::tr("last path: '%1'").arg(lastPath);

    return lastPath;
}

void
GtFileDialog::writeLastPath(const QString& val)
{
    QFileInfo info(val);

    // last path should point to a directory
    QString lastPath = info.isFile() ? info.absolutePath() : val;

    gtApp->settings()->setLastPath(info.absolutePath());

    gtDebug().medium() << QObject::tr("new last path") << ": " << lastPath;
}

