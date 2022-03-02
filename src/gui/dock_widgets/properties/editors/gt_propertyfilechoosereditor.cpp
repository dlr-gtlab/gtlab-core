/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfilechoosereditor.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFile>
#include <QDir>

#include "gt_application.h"
#include "gt_filedialog.h"
#include "gt_openfilenameproperty.h"
#include "gt_savefilenameproperty.h"
#include "gt_existingdirectoryproperty.h"

#include "gt_propertyfilechoosereditor.h"

GtPropertyFileChooserEditor::GtPropertyFileChooserEditor(QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* lay = new QHBoxLayout;
    m_filePath = new QLineEdit;
    lay->addWidget(m_filePath);

    m_restoreButton = new QPushButton(QStringLiteral(""));
    m_restoreButton->setIcon(gtApp->icon(QStringLiteral("restoreIcon_16.png")));
    m_restoreButton->setFlat(true);
    m_restoreButton->setMaximumWidth(15);
    m_restoreButton->setAutoDefault(false);
    m_restoreButton->setToolTip(tr("Clear File Path"));

    m_selectButton = new QPushButton(QStringLiteral("..."));
    m_selectButton->setMaximumWidth(30);
    m_selectButton->setAutoDefault(false);
    m_selectButton->setToolTip(tr("Select File Path"));

    lay->addWidget(m_restoreButton);
    lay->addWidget(m_selectButton);
    setLayout(lay);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_filePath->setFrame(false);
    m_filePath->setReadOnly(true);

    connect(m_selectButton, SIGNAL(clicked(bool)),
            SLOT(selectFilePath()));
    connect(m_restoreButton, SIGNAL(clicked(bool)), SLOT(deleteFilePath()));
}

void
GtPropertyFileChooserEditor::setFileChooserProperty(GtFileChooserProperty* prop)
{
    m_prop = prop;

    updateText();
}

void
GtPropertyFileChooserEditor::updateText()
{
    QString txt;

    if (m_prop && !m_prop->get().isEmpty())
    {
        txt = m_prop->get();
    }

    m_filePath->setText(txt);

    if (qobject_cast<GtExistingDirectoryProperty*>(m_prop))
    {
        m_restoreButton->setToolTip(tr("Clear Path"));
        m_selectButton->setToolTip(tr("Select Path"));
    }
    else
    {
        m_restoreButton->setToolTip(tr("Clear File Path"));
        m_selectButton->setToolTip(tr("Select File Path"));
    }
}

void
GtPropertyFileChooserEditor::selectFilePath()
{
    if (!m_prop)
    {
        return;
    }

    QString selfilter;
    QString filename;

    foreach (const QString& filter, m_prop->filter())
    {
        if (!selfilter.isEmpty())
        {
            selfilter = selfilter + QStringLiteral("; ");
        }

        selfilter = selfilter + QStringLiteral("*") + filter;
    }



    if (qobject_cast<GtSaveFileNameProperty*>(m_prop))
    {
        GtSaveFileNameProperty* sfnp =
            qobject_cast<GtSaveFileNameProperty*>(m_prop);

        QFile file(sfnp->getVal());

        if (file.exists())
        {
            QFileInfo fileInfo(file);

            filename = GtFileDialog::getSaveFileName(this,
                       tr("Choose File"),
                       fileInfo.absolutePath(), selfilter,
                       fileInfo.fileName());
        }
        else
        {
            filename = GtFileDialog::getSaveFileName(this,
                       tr("Choose File"),
                       QString(), selfilter, sfnp->initFileName());
        }
    }
    else if (qobject_cast<GtExistingDirectoryProperty*>(m_prop))
    {
        filename = GtFileDialog::getExistingDirectory(this,
                   tr("Choose Directory"),
                   QString());
    }
    else
    {
        filename = GtFileDialog::getOpenFileName(this,
                   tr("Choose File"),
                   QString(), selfilter);
    }


    if (!filename.isEmpty())
    {
        m_prop->setVal(filename);
    }

    updateText();

    emit fileSelected(filename);
}

void
GtPropertyFileChooserEditor::deleteFilePath()
{
    if (!m_prop)
    {
        return;
    }

    m_prop->setVal(QString());

    updateText();

    emit fileSelected(QString());
}
