/*
 * Author: Matthias Schuff
 */

#include "gt_preferencescommandlinetemplates.h"
#include "ui_gt_preferencescommandlinetemplates.h"
#include "gt_settings.h"
#include "gt_logging.h"
#include "gt_coreapplication.h"

#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>


#include "gt_commandlinetemplatefinder.h"


GtPreferencesCommandlineTemplates::GtPreferencesCommandlineTemplates(QWidget* parent) :
    GtPreferencesPage(tr("Commandline Templates"), parent),
    ui(new Ui::GtPreferencesCommandlineTemplates)
{
    ui->setupUi(this);
    setTitleShort("Commandline Templates");

    auto os = ui->os;
    os->addItem("Unknown", "unknown");
    os->addItem("Windows", "win");
    os->addItem("Unix", "unix");

    connect(ui->buttonOpenPathInExplorer, SIGNAL(clicked()), this, SLOT(onClickOpenInExplorer()));
    connect(ui->buttonRestoreDefaults, SIGNAL(clicked()), this, SLOT(onClickRestoreDefaults()));
    connect(ui->buttonCheckTemplateExists, SIGNAL(clicked()), this, SLOT(onClickCheckTemplateExists()));
}

GtPreferencesCommandlineTemplates::~GtPreferencesCommandlineTemplates()
{
    delete ui;
}


void GtPreferencesCommandlineTemplates::saveSettings(GtSettings &s) const
{

    auto path = ui->dir;
    auto machine = ui->machine;
    auto os = ui->os;
    auto shell = ui->shell;

    s.setCommandlineTemplatesPath(path->text());
    s.setCommandlineTemplatesMachine(machine->text());
    s.setCommandlineTemplatesDefaultShell(shell->text());
    s.setCommandlineTemplatesOs(os->currentData().toString());
}

void GtPreferencesCommandlineTemplates::loadSettings(const GtSettings &s)
{

    auto path = ui->dir;
    auto machine = ui->machine;
    auto os = ui->os;
    auto shell = ui->shell;

    path->setText(s.commandlineTemplatesPath());
    machine->setText(s.commandlineTemplatesMachine());
    QString osStr = s.commandlineTemplatesOs();

    int index = os->findData(osStr);

    if ( index != -1 )
    { // -1 for not found
        os->setCurrentIndex(index);
    }
    else
    {
        gtWarning() << "OS Type: "+osStr+" is unkown, resetting";
        os->setCurrentIndex(0);
    }

    shell->setText(s.commandlineTemplatesDefaultShell());

}

void GtPreferencesCommandlineTemplates::onClickOpenInExplorer()
{
    QString path = ui->dir->text();

    if (path.startsWith("~"))
    {
        path = QDir::homePath() + path.mid(1);
    }

    QDir d(path);

    if (!d.exists())
    {

        auto reply = QMessageBox::question(
            this,
            "Directory does not exist",
            QString("The directory:\n%1\n\ndoes not exist.\nCreate it?")
                .arg(QDir::toNativeSeparators(path)),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes)
        {
            if (!d.mkpath("."))
            {
                QMessageBox::critical(
                    this,
                    "Error",
                    "Failed to create directory."
                    );
                return;
            }
        }
        else
        {
            return;
        }
    }

    QUrl url = QUrl::fromLocalFile(QDir::cleanPath(path));
    QDesktopServices::openUrl(url);
}

void GtPreferencesCommandlineTemplates::onClickRestoreDefaults()
{

    gtInfo() << "onClickRestoreDefaults: TODO";

}

void GtPreferencesCommandlineTemplates::onClickCheckTemplateExists()
{
    gtInfo() << "onClickCheckTemplateExists";

    GtCommandlineTemplateFinder f(true);


    auto tool = ui->testToolname->text();
    auto version = ui->testToolversion->text();
    auto exectempl = ui->testTemplatename->text();

    bool hastemplate = f.hasExecutionTemplate(tool, version, exectempl);

    gtDebug() << "tool: " << tool << " version: " << version << " templatename:" << exectempl;
    if (hastemplate)
    {
        gtInfo() << "Template was found!";
        ui->labelTemplateFound->setText("Found!");
        ui->labelTemplateFound->setStyleSheet("color: green;");
    }
    else
    {
        gtError() << "Template was NOT found! Did you save/close/re-open the preferences?";
        ui->labelTemplateFound->setText("Error, NOT found!");
        ui->labelTemplateFound->setStyleSheet("color: red;");
    }

}
