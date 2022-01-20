/* GTlab - Gas Turbine laboratory
 * Source File: gt_changelogdialog.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QApplication>

#include "gt_application.h"
#include "gt_logging.h"

#include "gt_changelogdialog.h"

GtChangelogDialog::GtChangelogDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Changelog"));
    setWindowIcon(gtApp->icon(QStringLiteral("logIcon_16.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel* titleLabel = new QLabel;

    titleLabel->setPixmap(QPixmap(
                              QStringLiteral(":/pixmaps/changelogLogo.png")));

    layout->addWidget(titleLabel);

    QTextEdit* textEdit = new QTextEdit;
    textEdit->setReadOnly(true);

    layout->addWidget(textEdit);

    setLayout(layout);

    setMaximumWidth(350);

    if (!readChangelog(textEdit))
    {
        textEdit->setPlainText(tr("could not read changelog file!"));
    }
}

void
GtChangelogDialog::setToPopup()
{
    setWindowFlags(Qt::Popup);
}

bool
GtChangelogDialog::readChangelog(QTextEdit* textEdit)
{
    if (textEdit == Q_NULLPTR)
    {
        return false;
    }

    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();

    QFile file(appDir.absoluteFilePath("changelog.txt"));

    if (!file.exists())
    {
        gtDebug() << tr("file not exists!");
        return false;
    }

    if (!file.open(QFile::ReadOnly))
    {
        gtDebug() << tr("could not open file!");
        return false;
    }

    QByteArray data = file.readAll();
    QString str(data);
    QStringList lines = str.split(QRegExp("[\n]"));

    if (lines.size() <= 10)
    {
        gtDebug() << tr("changelog file corrupted!");
        return false;
    }

    for (int i = 0; i < 10; i++)
    {
        lines.removeFirst();
    }

    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i].contains(QStringLiteral("####")))
        {
            QString verLine = lines[i].replace("####", "version");
            textEdit->insertHtml(
                "<b>" + verLine + "</b><br>");
        }
        else
        {
            textEdit->insertPlainText(lines[i] + "\n");
        }
    }

    file.close();

    QTextCursor cur = textEdit->textCursor();
    cur.movePosition(QTextCursor::Start);
    textEdit->setTextCursor(cur);

    return true;
}
