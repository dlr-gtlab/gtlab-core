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

#include "gt_icons.h"
#include "gt_filedialog.h"
#include "gt_openfilenameproperty.h"
#include "gt_savefilenameproperty.h"
#include "gt_existingdirectoryproperty.h"
#include "gt_application.h"
#include "gt_command.h"
#include "gt_project.h"

#include "gt_propertyfilechoosereditor.h"

GtPropertyFileChooserEditor::GtPropertyFileChooserEditor(QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* lay = new QHBoxLayout;
    m_filePath = new QLineEdit;
    lay->addWidget(m_filePath);

    m_restoreButton = new QPushButton(QLatin1String{});
    m_restoreButton->setIcon(gt::gui::icon::clear());
    m_restoreButton->setFlat(true);
    m_restoreButton->setMaximumWidth(16);
    m_restoreButton->setAutoDefault(false);
    m_restoreButton->setToolTip(tr("Clear File Path"));

    m_selectButton = new QPushButton();
    m_selectButton->setFlat(true);
    m_selectButton->setIcon(gt::gui::icon::dots());
    m_selectButton->setMaximumWidth(16);
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

    if (m_prop)
    {
        connect(m_prop.data(), SIGNAL(changed()), SLOT(propertyValueChanged()));
    }

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
GtPropertyFileChooserEditor::setPropertyValue(const QString &val)
{
    if (!m_prop) return;

    const QString commandMsg = m_prop->objectName() + QStringLiteral(" ") +
            QObject::tr("changed");

    auto cmd = gtApp->makeCommand(gtApp->currentProject(), commandMsg);
    Q_UNUSED(cmd)

    m_prop->setVal(val);
}

void
GtPropertyFileChooserEditor::selectFilePath()
{
    if (!m_prop)
    {
        return;
    }

    QString selfilter = gt::detail::buildFileDialogFileFilter(m_prop->filter());

    QString filename;


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
        setPropertyValue(filename);
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

    setPropertyValue(QString());

    emit fileSelected(QString());
}

void
GtPropertyFileChooserEditor::propertyValueChanged()
{
    updateText();
}
