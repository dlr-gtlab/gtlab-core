/* GTlab - Gas Turbine laboratory
 * Source File: gt_selectfiletoexportpage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "gt_abstractexporter.h"
#include "gt_filedialog.h"
#include "gt_logging.h"

#include "gt_selectfiletoexportpage.h"

GtSelectFileToExportPage::GtSelectFileToExportPage(
        GtObject* obj, GtAbstractExporter* exporter) :
    GtAbstractExporterPage(obj),
    m_exporter(exporter),
    m_fileExtension(exporter->extension())
{
    if (m_exporter)
    {
        setTitle(tr("Select") + QStringLiteral(" ") +
                 m_exporter->objectName() + QStringLiteral(" ") +
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
GtSelectFileToExportPage::doExport(QFile& file)
{
    if (!m_exporter)
    {
        gtError() << tr("no exporter set! export function not found!");
        return false;
    }

    return m_exporter->doExport(object(), file);
}

void
GtSelectFileToExportPage::setFileExtension(const QString& fileExt)
{
    m_fileExtension = fileExt;
}

bool
GtSelectFileToExportPage::validatePage()
{
    QFile file(m_lineEdit->text());

    if (!doExport(file))
    {
        gtError() << tr("could not finish export!");
        wizard()->reject();
        return false;
    }

    return true;
}

bool
GtSelectFileToExportPage::isComplete() const
{
    return !m_lineEdit->text().isEmpty();
}

void
GtSelectFileToExportPage::selectFilePath()
{
    // check object
    if (!object())
    {
        return;
    }

    QString initFileName = object()->objectName().toLower();

    if (m_fileExtension.contains(QStringLiteral(".")))
    {
        QString ext = m_fileExtension;
        ext.remove(QStringLiteral("(*"));
        ext.remove(QStringLiteral(")"));
        QStringList extSplit = ext.split(".", QString::SkipEmptyParts);

        initFileName += QStringLiteral(".") + extSplit.last();
    }

    initFileName.replace(" ", "_");

    QString filename = GtFileDialog::getSaveFileName(this,
                       tr("Choose File"),
                       QString(), m_fileExtension, initFileName);

    gtDebug() << "selected file = " << filename;

    m_lineEdit->setText(filename);

    emit pathSelected();
}

void
GtSelectFileToExportPage::onPathSelected()
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
