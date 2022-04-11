/* GTlab - Gas Turbine laboratory
 * Source File: gt_aboutdialog.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QIcon>
#include <QDateTime>
#include <QFile>

#include "gt_application.h"
#include "gt_filedialog.h"
#include "gt_footprint.h"
#include "gt_logging.h"
#include "gt_icons.h"

#include "gt_aboutdialog.h"

GtAboutLogo::GtAboutLogo(QWidget* parent) :
    QWidget(parent),
    m_clicks(0),
    m_pixmap(GtGUI::Pixmap::splash())
{
}

void
GtAboutLogo::paintEvent(QPaintEvent* e)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, m_pixmap);

    QWidget::paintEvent(e);
}

void
GtAboutLogo::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
}


GtAboutDialog::GtAboutDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("About GTlab"));
    setWindowIcon(GtGUI::Icon::info16());

    /// Turn  off the "?"-Button in the header
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;

    GtAboutLogo* logo = new GtAboutLogo;
    logo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding));

    layout->addWidget(logo);

    QHBoxLayout* hLayout = new QHBoxLayout;

    QLabel* copyLabel =
            new QLabel(QStringLiteral(
                "GTlab - Gas Turbine laboratory\ncopyright 2009-2021 by DLR"));

    hLayout->addWidget(copyLabel);

    QString vers = gtApp->versionToString() +
                   QStringLiteral("\nHydrogen");

    QLabel* versLabel = new QLabel(vers);
    versLabel->setAlignment(Qt::AlignRight);

    hLayout->addWidget(versLabel);

    layout->addLayout(hLayout);

    layout->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::Minimum,
                                          QSizePolicy::Minimum));

    QHBoxLayout* btnLayout = new QHBoxLayout;


    btnLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                             QSizePolicy::Minimum));

    auto exportFootprintBtn = new QPushButton(tr("Export Framework Footprint"));
    exportFootprintBtn->setIcon(GtGUI::Icon::export16());
    exportFootprintBtn->setFocusPolicy(Qt::NoFocus);
    btnLayout->addWidget(exportFootprintBtn);

    layout->addLayout(btnLayout);

    layout->setSpacing(0);

    setLayout(layout);

    setMinimumSize(465, 380);
    setMaximumSize(465, 380);

    connect(exportFootprintBtn, SIGNAL(clicked()), SLOT(exportFootprint()));
}

void
GtAboutDialog::exportFootprint()
{
    QString initFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd") +
                           QStringLiteral("_gtlab_env_footprint.xml");

    QString filename = GtFileDialog::getSaveFileName(this,
                       tr("Choose File"),
                       QString(), tr("XML files (*.xml)"),
                       initFileName);

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open file!");
        return;
    }

    QTextStream stream(&file);
    stream << GtFootprint().exportToXML();

    file.close();
}

