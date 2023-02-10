/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectrestorebackupdialog.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 18.11.2022
 *  Author: Jens Schmeinkr (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_projectrestorebackupdialog.h"
#include "gt_icons.h"
#include "gt_logging.h"
#include "gt_textedit.h"
#include "gt_project.h"

#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

GtProjectRestoreBackupDialog::GtProjectRestoreBackupDialog(
        const QList<QDir> &validBackupDirs,
        QWidget* parent) :
    GtDialog(parent),
    m_folders(validBackupDirs)
{
    setWindowTitle(tr("Restore Backup"));
    setWindowIcon(gt::gui::icon::dataArrowUp());
    auto* layout = new QVBoxLayout;
    setLayout(layout);

    auto* textL = new QHBoxLayout;
    layout->addLayout(textL);

    auto* m_listW = new QListWidget;
    textL->addWidget(m_listW);

    QStringList names;

    for (auto const& d :validBackupDirs)
    {
        /// add "-" as seperators to increase the timestamps readability
        QString name = d.dirName();
        name.insert(12, "-");
        name.insert(10, "-");
        name.insert(8, "-");
        name.insert(6, "-");
        name.insert(4, "-");
        names.append(name);
    }
    m_listW->addItems(names);


    m_messageWidget = new GtTextEdit("", GtTextEdit::MD, this);
    m_messageWidget->setReadOnly(true);
    textL->addWidget(m_messageWidget);

    auto* btnLayOut = new QHBoxLayout;
    layout->addLayout(btnLayOut);

    btnLayOut->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding,
                                             QSizePolicy::Expanding));

    m_okBtn = new QPushButton(tr("Ok"));
    btnLayOut->addWidget(m_okBtn);
    m_okBtn->setDisabled(true);
    auto* cancelBtn = new QPushButton(tr("Cancel"));
    btnLayOut->addWidget(cancelBtn);

    connect(m_listW, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onElementSelection(QString)));

    connect(m_listW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

    connect(m_okBtn, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked(bool)), SLOT(close()));

    /// intial selection of first entry
    m_listW->setCurrentRow(0);
    emit m_listW->currentTextChanged(names.first());
}

QString
GtProjectRestoreBackupDialog::selectedFolderName() const
{
    return m_selectedFolderName;
}

void
GtProjectRestoreBackupDialog::onElementSelection(QString newText)
{
    QString folderName = newText.remove("-");

    QDir selection = folderByName(folderName);

    m_messageWidget->updateText(tr("No Message"));

    if (!selection.exists())
    {
        gtDebug() << tr("Cannot handle selection");
        return;
    }

    m_okBtn->setEnabled(true);
    m_selectedFolderName = folderName;


    QFile messageFile (selection.absoluteFilePath(
                           gt::project::backUpMessageFileName()));

    if (!messageFile.exists())
    {
        gtDebug() << tr("Cannot find message");
        return;
    }

    if (!messageFile.open(QIODevice::ReadOnly))
    {
        gtDebug() << tr("Cannot find message");
        return;
    }

    QTextStream in(&messageFile);
    QString content = in.readAll();
    messageFile.close();

    m_messageWidget->updateText(content);
}

void
GtProjectRestoreBackupDialog::onItemDoubleClicked(QListWidgetItem *i)
{
    QString folderName = i->text().remove("-");
    m_selectedFolderName = folderName;
    accept();
}

QDir
GtProjectRestoreBackupDialog::folderByName(const QString& name) const
{
    auto iter = std::find_if(std::begin(m_folders), std::end(m_folders),
                             [&name](const QDir& dir) {
        return dir.dirName() == name;
    });

    if (iter != std::end(m_folders))
    {
        return *iter;
    }
    else
    {
        return {};
    }
}
