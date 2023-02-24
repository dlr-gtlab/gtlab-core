/* GTlab - Gas Turbine laboratory
 * Source File: gt_moduledetailsdialog.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_moduledetailsdialog.h"

#include "gt_application.h"
#include "gt_versionnumber.h"
#include "gt_icons.h"
#include "gt_changelogwidget.h"
#include "gt_logging.h"

#include <QGridLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QDir>
#include <QTextEdit>
#include <QTextStream>
#include <QTabWidget>

GtModuleDetailsDialog::GtModuleDetailsDialog(QString const& moduleId,
                                             QWidget* parent) :
    GtDialog(parent),
    m_moduleId(moduleId),
    m_rowCounter(0)
{
    setWindowTitle(m_moduleId);
    setWindowIcon(gt::gui::icon::plugin());

    auto l = new QVBoxLayout;
    auto tab = new QTabWidget;

    if (QWidget* genInfos = generalInformation())
    {
        tab->addTab(genInfos, tr("General"));
    }
    else
    {
        gtWarning() << tr("Cannot find basic information about module");
    }

    if (QWidget* readMe = readMeWidget())
    {
        tab->addTab(readMe, tr("ReadMe"));
    }
    else
    {
        gtWarning() << tr("Cannot find read me information of module");
    }


    if (QWidget* changeLogW = changeLogWidget())
    {
        tab->addTab(changeLogW, tr("Changelog"));
    }
    else
    {
        gtWarning() << tr("Cannot find changelog of module");
    }

    l->addWidget(tab);

    setFixedWidth(600);
    setMinimumHeight(400);

    setLayout(l);
}

QWidget*
GtModuleDetailsDialog::generalInformation()
{
    QWidget* w = new QWidget;

    m_lay = new QGridLayout;
    m_lay->setColumnStretch(0, 0);
    m_lay->setColumnStretch(1, 1);

    QString moduleVersion = gtApp->moduleVersion(m_moduleId).toString();
    QString moduleDescription = gtApp->moduleDescription(m_moduleId);

    addLine(tr("Name:"), m_moduleId);
    addLine(tr("Version:"), moduleVersion);
    addLine(tr("Description:"), moduleDescription);

    QString moduleAuthor = gtApp->moduleAuthor(m_moduleId);
    QString moduleContact = gtApp->moduleContact(m_moduleId);
    QString moduleLicense = gtApp->moduleLicence(m_moduleId);

    addLine(tr("Author:"), moduleAuthor);
    addLine(tr("Contact:"), moduleContact);
    addLine(tr("Licence:"), moduleLicense);

    m_lay->setRowStretch(m_rowCounter, 10);

    w->setLayout(m_lay);

    return w;
}

QWidget*
GtModuleDetailsDialog::changeLogWidget()
{
    std::tuple<QString,GtTextEdit::contentType> t = loadInfoFile("CHANGELOG.*");

    QString infoContent = std::get<0>(t);
    GtTextEdit::contentType type = std::get<1>(t);

    /// for the recommended mark down structure of the changelog file
    /// the ChangeLog Widget should be used utilizing a specific parser
    if (type == GtTextEdit::MD)
    {
        auto e = new GtChangeLogWidget(infoContent);
        return e;
    }

    auto* w = new QWidget;
    auto* l = new QVBoxLayout;
    auto e = new GtTextEdit(infoContent, type);
    e->setReadOnly(true);
    e->setMinimumWidth(400);
    l->addWidget(e);
    w->setLayout(l);

    return w;
}

QWidget*
GtModuleDetailsDialog::readMeWidget()
{
    std::tuple<QString,GtTextEdit::contentType> t =
            loadInfoFile("README.*");

    GtTextEdit::contentType type = std::get<1>(t);
    if (type == GtTextEdit::NONE)
    {
        gtWarning() << std::get<0>(t);
        return nullptr;
    }

    QString shortTxt = std::get<0>(t);

    int cut = shortTxt.indexOf(QRegExp("\\n"), 250) + 1;

    shortTxt.truncate(cut);

    if (shortTxt.isEmpty())
    {
        shortTxt = QStringLiteral("No read me could be found");
    }

    if (std::get<0>(t).size() > cut)
    {
        shortTxt.append("\n...");
    }

    auto* w = new QWidget;
    auto* l = new QVBoxLayout;
    auto* e = new GtTextEdit(shortTxt, type);
    e->setReadOnly(true);
    e->setMinimumWidth(400);
    e->setLongText(std::get<0>(t));
    l->addWidget(e);
    w->setLayout(l);

    return w;
}

void
GtModuleDetailsDialog::addLine(const QString& title, const QString& value)
{
    auto textLabel = new QLabel(title);

    m_lay->addWidget(textLabel, m_rowCounter, 0, Qt::AlignRight);

    auto valueLabel = new QLabel(value);

    m_lay->addWidget(valueLabel, m_rowCounter, 1, Qt::AlignLeft);
    m_rowCounter++;
}

std::tuple<QString,GtTextEdit::contentType>
GtModuleDetailsDialog::loadInfoFile(QString const& filter)
{
#ifndef Q_OS_ANDROID
    QString path = QCoreApplication::applicationDirPath() +
            QDir::separator() + QStringLiteral("modules") +
            QDir::separator() + QStringLiteral("meta") + QDir::separator();
#else
    QString path = QCoreApplication::applicationDirPath();
#endif

    QDir modulesDir(path + windowTitle());

    GtTextEdit::contentType none = GtTextEdit::NONE;

    if (modulesDir.exists())
    {
        QStringList infos = modulesDir.entryList({filter});

        if (infos.isEmpty())
        {
            return {QStringLiteral("Cannot find valid file for this module"),
                        none};
        }

        QFile file(modulesDir.absoluteFilePath(infos.constFirst()));

        if (!file.exists())
        {
            return {QStringLiteral("Cannot find file for this module"), none};
        }

        if (!file.open(QIODevice::ReadOnly))
        {
            return {QStringLiteral("Cannot open file for this module"), none};
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        return {content, GtTextEdit::typeFromFile(file.fileName())};
    }

    return {QStringLiteral("Cannot find directory for additional information"),
                none};
}
