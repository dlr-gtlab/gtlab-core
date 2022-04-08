/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QDialog>

#include "gt_application.h"
#include "gt_examplesentry.h"
#include "gt_datamodel.h"
#include "gt_logging.h"
#include "gt_icons.h"

#include "gt_examplegraphicalitem.h"

GtExampleGraphicalItem::GtExampleGraphicalItem(GtExamplesEntry data,
                                               QWidget* parent) :
    QWidget(parent),
    m_data(std::move(data)),
    m_picLabel(nullptr),
    m_picFrame(nullptr),
    m_zoomButton(nullptr),
    m_selected(false)
{

    setObjectName(QString(m_data.dirPath() + "#" +  m_data.name()));

    setStyleSheet("border: 0px solid gray;border-radius: 4px");

    setFixedHeight(250);

    setFixedWidth(250);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout* mainLay = new QVBoxLayout;

    mainLay->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLay);

    m_picFrame = new QFrame;

    if (gtApp->inDarkMode())
    {
        m_picFrame->setStyleSheet(
                    "QWidget { border: 1px solid gray;"
                    "border-radius: 4px;"
                    //"background-color: white;"
                    "min-width: 70px;min-height:22px}"
                    "QWidget:hover{background-color:rgb(180,200,200)}"
                    "QWidget:pressed{background-color:rgb(180,213,213)}");
    }
    else
    {
        m_picFrame->setStyleSheet(
                    "QWidget { border: 1px solid gray;"
                    "border-radius: 4px;"
                    "background-color: white;"
                    "min-width: 70px;min-height:22px}"
                    "QWidget:hover{background-color:rgb(220,235,250)}"
                    "QWidget:pressed{background-color:rgb(180,213,246)}");
    }


    m_picFrame->setFrameShape(QFrame::Box);

    m_picFrame->setFixedHeight(250);
    m_picFrame->setFixedWidth(250);

    mainLay->addWidget(m_picFrame);

    QVBoxLayout* picLayout = new QVBoxLayout;

    m_picFrame->setLayout(picLayout);

    m_picLabel = new QLabel;
    m_picLabel->setStyleSheet("background-color: rgba(0,0,0,0%);"
                              "border: 0px solid gray");


    picLayout->addWidget(m_picLabel);

    QLabel* nameLabel = new QLabel(m_data.name());
    QFont fnt("Helvetica", 9);
    fnt.setBold(true);

    nameLabel->setFrameStyle(QFrame::NoFrame);
    nameLabel->setStyleSheet("background-color: rgba(0,0,0,0%);"
                             "border: 0px solid gray");
    nameLabel->setFrameStyle(QFrame::NoFrame);
    nameLabel->setFont(fnt);
    nameLabel->setMaximumHeight(90);
    nameLabel->setMinimumHeight(80);

    picLayout->addWidget(nameLabel);

    nameLabel->setAlignment(Qt::AlignCenter);

    m_zoomButton = new QPushButton;

    m_zoomButton->setVisible(false);

    m_zoomButton->setIcon(GtGUI::Icon::info16());

    picLayout->addWidget(m_zoomButton);

    m_description = m_data.description();

    connect(m_zoomButton, SIGNAL(clicked()), SLOT(showZoom()));



    init();
}


void
GtExampleGraphicalItem::mousePressEvent(QMouseEvent* event)
{
    showZoom();

    QWidget::mousePressEvent(event);
}

void
GtExampleGraphicalItem::enterEvent(QEvent* event)
{
    setAutoFillBackground(true);

    QWidget::enterEvent(event);
}

void
GtExampleGraphicalItem::leaveEvent(QEvent* event)
{
    m_zoomButton->setVisible(false);

    if (!m_selected)
    {
        setAutoFillBackground(false);
    }

    QWidget::leaveEvent(event);
}

void
GtExampleGraphicalItem::init()
{
    setToolTip(m_data.dirPath());

    QFile preview(m_data.pixmapPath());

    if (preview.exists())
    {
        if (m_pixmap.load(m_data.pixmapPath()))
        {
            QPixmap scaledPixmap = m_pixmap.scaled(m_picFrame->size()
                                                   - QSize(18, 18),
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
            m_picLabel->setPixmap(scaledPixmap);
        }

        else
        {
            m_picLabel->setText(tr("No Preview available"));
        }
    }

    else
    {
        m_picLabel->setText(tr("Preview for example not found!"));
    }
}

void
GtExampleGraphicalItem::showZoom()
{
    if (m_pixmap.isNull())
    {
        if (m_pixmap.load(GtGUI::Pixmap::logoString()))
        {
           gtDebug() << tr("No default pic found");
        }
    }


    if (!m_pixmap.isNull())
    {
        QDialog dialog;
        QVBoxLayout* layout = new QVBoxLayout;
        QLabel* label = new QLabel;
        QPushButton* openButton = new QPushButton(tr("Open example"));
        QPixmap scaledPixmap = m_pixmap.scaled(QSize(600, 400),
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);

        label->setPixmap(scaledPixmap);
        label->setStyleSheet("border: 1px solid grey");
        layout->addWidget(label);
        layout->addWidget(openButton);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        if (m_description != "")
        {
            QLabel* descriptionLabel = new QLabel;

            descriptionLabel->setMaximumWidth(600);
            descriptionLabel->setWordWrap(true);

            QFont fnt("Helvetica", 10);
            //fnt.setBold(true);

            descriptionLabel->setAlignment(Qt::AlignCenter);

            descriptionLabel->setMargin(5);
            descriptionLabel->setFont(fnt);
            descriptionLabel->setText(m_description);

            layout->addWidget(descriptionLabel);
        }

        dialog.setLayout(layout);
        dialog.setWindowFlags(Qt::Popup);

        connect(openButton, SIGNAL(clicked()), &dialog, SLOT(accept()));

        if (dialog.exec())
        {
            emit openProject(objectName());
        }
    }
}
