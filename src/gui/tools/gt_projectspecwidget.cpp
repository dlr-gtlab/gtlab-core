/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectspecwidget.cpp
 *
 *  Created on: 27.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QRegularExpressionValidator>

#include "gt_filedialog.h"
#include "gt_icons.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_projectprovider.h"
#include "gt_regexp.h"

#include "gt_projectspecwidget.h"

GtProjectSpecWidget::GtProjectSpecWidget(QWidget* parent) : QWidget(parent),
    m_tmpName(QStringLiteral("New Project")),
    m_valid(false)
{
    assert(gtApp);

    QGridLayout* layout = new QGridLayout;

    // project name
    QLabel* nameLabel = new QLabel(tr("Name") + QStringLiteral(":"));
    layout->addWidget(nameLabel, 0, 0);
    m_nameLine = new QLineEdit;

    QValidator* validator = new QRegExpValidator(
                gt::re::onlyLettersAndNumbersAndSpace(), this);
    m_nameLine->setValidator(validator);

    m_nameLine->setText(m_tmpName);
    layout->addWidget(m_nameLine, 0, 1);
    m_nameCheck = new QLabel;
    layout->addWidget(m_nameCheck, 0, 2);

    connect(m_nameLine, SIGNAL(textEdited(QString)),
            SLOT(onNameChange(QString)));

    // project path
    QLabel* pathLabel = new QLabel(tr("Directory")+ QStringLiteral(":"));
    layout->addWidget(pathLabel, 1, 0);
    m_pathLine = new QLineEdit;
    m_pathLine->setText(gtApp->settings()->lastPath());
    layout->addWidget(m_pathLine, 1, 1);
    m_pathCheck = new QLabel;
    layout->addWidget(m_pathCheck, 1, 2);
    QPushButton* pathButton = new QPushButton();
    pathButton->setMaximumWidth(30);
    pathButton->setFlat(true);
    pathButton->setIcon(gt::gui::icon::dots());
    layout->addWidget(pathButton, 1, 3);

    connect(pathButton, SIGNAL(clicked(bool)), SLOT(browseDirectory()));

    setLayout(layout);

    onNameChange(m_tmpName);

    connect(m_nameLine, SIGNAL(textEdited(QString)), SLOT(updateStates()));
    connect(m_pathLine, SIGNAL(textEdited(QString)), SLOT(updateStates()));

    updateStates();
}

QLineEdit*
GtProjectSpecWidget::nameLine()
{
    return m_nameLine;
}

QLineEdit*
GtProjectSpecWidget::pathLine()
{
    return m_pathLine;
}

bool
GtProjectSpecWidget::isValid()
{
    return m_valid;
}

void
GtProjectSpecWidget::setProjectInformation(const QString& initName,
                                           const QString& initPath)
{
    m_nameLine->setText(initName);
    m_pathLine->setText(initPath);

    onNameChange(initName);
}

bool
GtProjectSpecWidget::updateStates()
{
    QPixmap check = gt::gui::icon::check().pixmap(QSize(20, 20));
    QPixmap cross = gt::gui::icon::cross().pixmap(QSize(20, 20));

    bool retval = true;

    m_nameCheck->setPixmap(check);
    m_nameCheck->setToolTip(QString());
    m_pathCheck->setPixmap(check);
    m_pathCheck->setToolTip(QString());

    if (m_nameLine->text().isEmpty())
    {
        m_nameCheck->setPixmap(cross);
        m_nameCheck->setToolTip(tr("Please specify a project name!"));
        retval = false;
    }
    else
    {
        if (GtProjectProvider::projectExistsInSession(m_nameLine->text()))
        {
            m_nameCheck->setPixmap(cross);
            m_nameCheck->setToolTip(tr("Project name already "
                                       "exists in session!"));
            retval = false;
        }
    }

    if (m_pathLine->text().isEmpty())
    {
        m_pathCheck->setPixmap(cross);
        m_pathCheck->setToolTip(tr("Please specify a project directory!"));
        retval = false;
    }
    else
    {
        if (GtProjectProvider::projectFileExists(m_pathLine->text()))
        {
            m_pathCheck->setPixmap(cross);
            m_pathCheck->setToolTip(tr("Project file already "
                                       "exists in directory!"));
            retval = false;
        }
    }

    emit statesUpdated();

    m_valid = retval;

    emit validated(retval);

    return retval;
}

void
GtProjectSpecWidget::onNameChange(const QString& val)
{
    QString path = m_pathLine->text();

    if (path.isEmpty())
    {
        return;
    }

    if (!m_tmpName.isEmpty() && path.endsWith(m_tmpName))
    {
        path.remove(path.size() - m_tmpName.size(), m_tmpName.size());
    }

    if (!path.endsWith(QStringLiteral("/")) &&
            !path.endsWith(QStringLiteral("\\")))
    {
        path.append(QDir::separator());
    }

    // convert all separators to native separators for current system
    path = QDir::toNativeSeparators(path);

    // append selected directory name
    path.append(val);

    m_pathLine->setText(path);
    m_tmpName = val;

    updateStates();
}

void
GtProjectSpecWidget::browseDirectory()
{
    QString directory = GtFileDialog::getExistingDirectory(this,
                          tr("Select new project directory"),
                          m_pathLine->text());

    if (!directory.isEmpty())
    {
        m_pathLine->setText(directory);
        onNameChange(m_tmpName);
    }
}
