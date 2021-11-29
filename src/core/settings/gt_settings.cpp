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

    /// rename object
    shortCutData.insert(QStringLiteral("rename"),
                        QStringList{QKeySequence(Qt::Key_F2).toString(),
                                    catCore});

    /// openContectMenu
    shortCutData.insert(QStringLiteral("OpenContextMenu"),
                        QStringList{QKeySequence(Qt::Key_F4).toString(),
                                    catCore});

    /// ShowFootprint
    shortCutData.insert(QStringLiteral("ShowFootprint"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_I).toString(), catCore});
    /// redo
    shortCutData.insert(QStringLiteral("redo"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_Y).toString(), catCore});

    /// undo
    shortCutData.insert(QStringLiteral("undo"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_Z).toString(), catCore});

    /// cut
    shortCutData.insert(QStringLiteral("cut"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_X).toString(), catCore});

    /// copy
    shortCutData.insert(QStringLiteral("copy"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_C).toString(), catCore});

    /// paste
    shortCutData.insert(QStringLiteral("paste"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_V).toString(), catCore});

    /// delete
    shortCutData.insert(QStringLiteral("delete"),
                        QStringList{QKeySequence(Qt::Key_Delete).toString(),
                                    catCore});

    /// runProcess
    shortCutData.insert(QStringLiteral("runProcess"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_R).toString(), catCore});

    /// unskipProcess
    shortCutData.insert(QStringLiteral("unskipProcess"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_T).toString(), catCore});

    /// skipProcess
    shortCutData.insert(QStringLiteral("skipProcess"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_G).toString(), catCore});

    /// toggleDebugOutput
    shortCutData.insert(QStringLiteral("toggleDebugOutput"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_D).toString(), catOutput});

    /// toggleInfoOutput
    shortCutData.insert(QStringLiteral("toggleInfoOutput"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_I).toString(), catOutput});

    /// toggleWarningOutput
    shortCutData.insert(QStringLiteral("toggleWarningOutput"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_W).toString(), catOutput});

    /// toggleErrorOutput
    shortCutData.insert(QStringLiteral("toggleErrorOutput"),
                        QStringList{QKeySequence(Qt::CTRL +
                                    Qt::Key_E).toString(), catOutput});

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
