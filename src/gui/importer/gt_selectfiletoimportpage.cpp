/* GTlab - Gas Turbine laboratory
 * Source File: gt_selectfiletoimportpage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "gt_abstractimporter.h"
#include "gt_filedialog.h"
#include "gt_logging.h"
#include "gt_filechooserproperty.h"

#include "gt_selectfiletoimportpage.h"

GtSelectFileToImportPage::GtSelectFileToImportPage(
        GtObject* obj, GtAbstractImporter* importer) :
    GtAbstractImporterPage(obj),
    m_fileExtensions(importer->extensions()),
    m_importer(importer)

{
    if (m_importer)
    {
        setTitle(tr("Select") + QStringLiteral(" ") +
                 m_importer->objectName() + QStringLiteral(" ") +
                 tr("File"));
    }
    else
    {
        setTitle(tr("Select File"));
    }

    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* fpLay = new QHBoxLayout;

    QLabel* lbl = new QLabel(tr("File Path"));

    fpLay->addWidget(lbl);

    m_lineEdit = new QLineEdit;

    fpLay->addWidget(m_lineEdit);

    QPushButton* btn = new QPushButton(QStringLiteral("..."));

    fpLay->addWidget(btn);

    layout->addLayout(fpLay);
    layout->addStretch(1);

    setLayout(layout);

    connect(btn, SIGNAL(clicked(bool)), SLOT(selectFilePath()));
    connect(m_lineEdit, SIGNAL(textChanged(QString)),
            SIGNAL(completeChanged()));
    connect(m_lineEdit, SIGNAL(textEdited(QString)),
            SIGNAL(completeChanged()));
    connect(this, SIGNAL(pathSelected()), this, SLOT(onPathSelected()));
}

bool
GtSelectFileToImportPage::import(QFile& file)
{
    if (!m_importer)
    {
        gtError() << tr("no importer set! import function not found!");
        return false;
    }

    return m_importer->import(object(), file);
}

void
GtSelectFileToImportPage::setFileExtensions(const QStringList& fileExt)
{
    m_fileExtensions = fileExt;
}

QString
GtSelectFileToImportPage::selectedFileName()
{
    return m_lineEdit->text();
}

bool
GtSelectFileToImportPage::validatePage()
{
    QFile file(m_lineEdit->text());

    if (!import(file))
    {
        gtError() << tr("could not finish import!");
        wizard()->reject();
        return false;
    }

    return true;
}

bool
GtSelectFileToImportPage::isComplete() const
{
    return QFile(m_lineEdit->text()).exists();
}

void
GtSelectFileToImportPage::selectFilePath()
{
    QString selfilter = gt::detail::buildFileDialogFileFilter(m_fileExtensions);

    QString filename = GtFileDialog::getOpenFileName(this,
                       tr("Choose File"),
                       QString(), selfilter);

    gtDebug() << "selected file = " << filename;

    m_lineEdit->setText(filename);

    emit pathSelected();
}

void
GtSelectFileToImportPage::onPathSelected()
{
    if (isComplete())
    {
        if (nextId() > 0)
        {
            wizard()->button(QWizard::NextButton)->click();
        }
        else
        {
            wizard()->button(QWizard::FinishButton)->click();
        }
    }
}
