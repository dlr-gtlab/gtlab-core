/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSettings>
#include <QMap>
#include <QKeySequence>
#include <QString>

#include "gt_settings.h"
#include "gt_settingsitem.h"

GtSettings::GtSettings()
{
    m_lastSession = registerSetting(
                        QStringLiteral("application/general/lastsession"),
                        QStringLiteral("default"));
    m_openSession = registerSetting(
                        QStringLiteral("application/general/opensession"),
                        false);
    m_language = registerSetting(QStringLiteral("application/general/language"),
                                 QStringLiteral(""));
    m_firstRun = registerSetting(QStringLiteral("application/general/firstRun"),
                                 true);
    m_lastPath = registerSetting(QStringLiteral("application/general/lastPath"),
                                 QStringLiteral(""));

    m_lastPerspective =
            registerSetting(
                QStringLiteral("application/general/lastPerspective"),
                QStringLiteral("default"));
    m_showStartupPage = registerSetting(
                            QStringLiteral("application/general/startuppage"),
                            true);
    m_searchForUpdate =
            registerSetting(
                QStringLiteral("application/general/searchforupdate"),
                true);
    m_lastProject = registerSetting(
                        QStringLiteral("application/general/lastproject"),
                        QStringLiteral(""));
    m_openProject = registerSetting(
                        QStringLiteral("application/general/openproject"),
                        false);
    m_maxLogLength = registerSetting(
                         QStringLiteral("application/general/maxloglength"),
                         (int) 2000);
    m_lastProcessElements =
            registerSetting(
                QStringLiteral("application/process/lastelements"),
                QStringList());
    m_explorerExpandStates =
            registerSetting(
                QStringLiteral("application/explorer/expandstates"),
                QStringList());

    m_shortcutsTable = registerSetting(
                QStringLiteral("application/general/shortCuts"),
                initialShortCuts());

    m_themeSelection = registerSetting(
                        QStringLiteral("application/general/themeSelection"),
                        QStringLiteral("system"));
}

QMap<QString, QStringList>
GtSettings::shortcutsTable() const
{
    QVariant val = QSettings().value(m_shortcutsTable->ident());

    QMap<QString, QVariant> helpingMap = val.toMap();

    QMap<QString, QStringList> retVal;

    for (QString s : helpingMap.keys())
    {
        QVariant v = helpingMap.value(s);

        QStringList list = v.toStringList();

        retVal.insert(s, list);
    }

    return retVal;
}

void
GtSettings::setShortcutsTable(QMap<QString, QStringList> shortcutsTable)
{
    QMap<QString, QVariant> helpingMap;

    for (QString s : shortcutsTable.keys())
    {
        QStringList list = shortcutsTable.value(s);
        helpingMap.insert(s, QVariant(list));
    }

    QVariant val(helpingMap);

    QSettings().setValue(m_shortcutsTable->ident(), val);
}

QVariant
GtSettings::initialShortCuts() const
{
    QMap<QString, QVariant> shortCutData;

    QString catCore = QStringLiteral("Core");
    QString catOutput = QStringLiteral("Output Dock");

    /// openContectMenu
    QStringList s1 = {QKeySequence(Qt::Key_F2).toString(), catCore};
    shortCutData.insert(QStringLiteral("OpenContextMenu"), s1);

    /// ShowFootprint
    QStringList s2 = {QKeySequence(Qt::CTRL + Qt::Key_I).toString(), catCore};
    shortCutData.insert(QStringLiteral("ShowFootprint"), s2);

    /// redo
    QStringList s3 = {QKeySequence(Qt::CTRL + Qt::Key_Y).toString(), catCore};
    shortCutData.insert(QStringLiteral("redo"), s3);

    /// undo
    QStringList s4 = {QKeySequence(Qt::CTRL + Qt::Key_Z).toString(), catCore};
    shortCutData.insert(QStringLiteral("undo"), s4);

    /// cut
    QStringList s5 = {QKeySequence(Qt::CTRL + Qt::Key_X).toString(), catCore};
    shortCutData.insert(QStringLiteral("cut"), s5);

    /// copy
    QStringList s6 = {QKeySequence(Qt::CTRL + Qt::Key_C).toString(), catCore};
    shortCutData.insert(QStringLiteral("copy"), s6);

    /// paste
    QStringList s7 = {QKeySequence(Qt::CTRL + Qt::Key_V).toString(), catCore};
    shortCutData.insert(QStringLiteral("paste"), s7);

    /// delete
    QStringList s8 = {QKeySequence(Qt::Key_Delete).toString(), catCore};
    shortCutData.insert(QStringLiteral("delete"), s8);

    /// runProcess
    QStringList s9 = {QKeySequence(Qt::CTRL + Qt::Key_R).toString(), catCore};
    shortCutData.insert(QStringLiteral("runProcess"), s9);

    /// unskipProcess
    QStringList s10 = {QKeySequence(Qt::CTRL + Qt::Key_T).toString(), catCore};
    shortCutData.insert(QStringLiteral("unskipProcess"), s10);

    /// skipProcess
    QStringList s11 = {QKeySequence(Qt::CTRL + Qt::Key_G).toString(), catCore};
    shortCutData.insert(QStringLiteral("skipProcess"), s11);

    /// toggleDebugOutput
    QStringList s12 = {QKeySequence(Qt::CTRL + Qt::Key_D).toString(),
                       catOutput};
    shortCutData.insert(QStringLiteral("toggleDebugOutput"), s12);

    /// toggleInfoOutput
    QStringList s13 = {QKeySequence(Qt::CTRL + Qt::Key_I).toString(),
                       catOutput};
    shortCutData.insert(QStringLiteral("toggleInfoOutput"), s13);

    /// toggleWarningOutput
    QStringList s14 = {QKeySequence(Qt::CTRL + Qt::Key_W).toString(),
                       catOutput};
    shortCutData.insert(QStringLiteral("toggleWarningOutput"), s14);

    /// toggleErrorOutput
    QStringList s15 = {QKeySequence(Qt::CTRL + Qt::Key_E).toString(),
                       catOutput};
    shortCutData.insert(QStringLiteral("toggleErrorOutput"), s15);

    return QVariant(shortCutData);
}

QMap<QString, QStringList>
GtSettings::intialShortCutsMap()
{
    QVariant v = initialShortCuts();

    QMap<QString, QVariant> map = v.toMap();

    QMap<QString, QStringList> retVal;

    for (QString k : map.keys())
    {
        retVal.insert(k, map.value(k).toStringList());
    }

    return retVal;
}

QString
GtSettings::themeMode()
{
    QVariant val = QSettings().value(m_themeSelection->ident());
    return val.toString();
}

bool
GtSettings::darkMode()
{
    QVariant val = QSettings().value(m_themeSelection->ident());
    QString s = val.toString();

    if (s.toStdString() == ("dark"))
    {
        return true;
    }
    else if (s.toStdString() == ("bright"))
    {
        return false;
    }
    else    // system mode
    {
#ifdef Q_OS_WIN
        std::string set = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        QString set2 = QString::fromStdString(set);

        std::string a = "AppsUseLightTheme";
        QString a2 = QString::fromStdString(a);

        QSettings settings(set2 ,QSettings::NativeFormat);
        if (settings.value(a2) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
#else //Dark mode detection is inactive for linux
        return false;
#endif
    }
}

void
GtSettings::setThemeMode(const QString& theme)
{
    QSettings().setValue(m_themeSelection->ident(), theme);
}

QString
GtSettings::lastSession()
{
    return QSettings().value(m_lastSession->ident()).toString();
}

void
GtSettings::setLastSession(const QString& val)
{
    QSettings().setValue(m_lastSession->ident(), val);
}

QString
GtSettings::lastProject()
{
    return QSettings().value(m_lastProject->ident()).toString();
}

void
GtSettings::setLastProject(const QString& val)
{
    QSettings().setValue(m_lastProject->ident(), val);
}

QString
GtSettings::lastPerspective()
{
    return QSettings().value(m_lastPerspective->ident()).toString();
}

void
GtSettings::setLastPerspective(const QString& val)
{
    QSettings().setValue(m_lastPerspective->ident(), val);
}

QString
GtSettings::lastPath()
{
    return QSettings().value(m_lastPath->ident()).toString();
}

void
GtSettings::setLastPath(const QString& val)
{
    QSettings().setValue(m_lastPath->ident(), val);
}

bool
GtSettings::openLastSession()
{
    return QSettings().value(m_openSession->ident()).toBool();
}

void
GtSettings::setOpenLastSession(bool val)
{
    QSettings().setValue(m_openSession->ident(), val);
}

bool
GtSettings::openLastProject()
{
    return QSettings().value(m_openProject->ident()).toBool();
}

void
GtSettings::setOpenLastProject(bool val)
{
    QSettings().setValue(m_openProject->ident(), val);
}

QString
GtSettings::language()
{
    return QSettings().value(m_language->ident()).toString();
}

void
GtSettings::setLanguage(const QString& val)
{
    QSettings().setValue(m_language->ident(), val);
}

bool
GtSettings::firstApplicationRun()
{
    return !QSettings().value(m_firstRun->ident()).toBool();
}

void
GtSettings::setShowStartupPage(bool val)
{
    QSettings().setValue(m_showStartupPage->ident(), val);
}

bool
GtSettings::showStartupPage()
{
    return QSettings().value(m_showStartupPage->ident()).toBool();
}

void
GtSettings::setSearchForUpdate(bool val)
{
    QSettings().setValue(m_searchForUpdate->ident(), val);
}

bool
GtSettings::searchForUpdate()
{
    return QSettings().value(m_searchForUpdate->ident()).toBool();
}

void
GtSettings::setMaxLogLength(int val)
{
    QSettings().setValue(m_maxLogLength->ident(), val);
}

int
GtSettings::maxLogLength()
{
    return QSettings().value(m_maxLogLength->ident()).toInt();
}

void
GtSettings::setLastProcessElements(const QStringList& list)
{
    if (list.size() > 10)
    {
        return;
    }

    QSettings().setValue(m_lastProcessElements->ident(), list);
}

QStringList
GtSettings::lastProcessElements()
{
    return QSettings().value(m_lastProcessElements->ident()).toStringList();
}

void
GtSettings::setExplorerExpandStates(const QStringList& list)
{
    QSettings().setValue(m_explorerExpandStates->ident(), list);
}

QStringList
GtSettings::explorerExpandStates()
{
    return QSettings().value(m_explorerExpandStates->ident()).toStringList();
}
