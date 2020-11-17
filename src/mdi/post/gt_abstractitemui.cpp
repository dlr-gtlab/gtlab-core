/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on:
 *  Author:  (AT-TWK)
 *  Tel.:
 */

#include "gt_abstractitemui.h"
#include "gt_application.h"
#include "gt_abstractpostwidget.h"
#include "gt_posttemplateitem.h"
#include "gt_customactionmenu.h"
#include "gt_stylesheets.h"

#include "QMenu"
#include "QLineEdit"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QLabel"
#include "QPushButton"
#include <QPrinter>

#include "QsLog.h"

GtAbstractItemUI::GtAbstractItemUI(GtAbstractPostWidget* widget)
{
    setWidget(widget);

    QVBoxLayout* main = new QVBoxLayout();

    m_tollBarLayout = new QHBoxLayout();
    m_tollBarLayout->setSpacing(3);

    //    QSpacerItem* spacer = new QSpacerItem(10, 20,
    //                                 QSizePolicy::Minimum, QSizePolicy::Minimum);
    //    QSpacerItem* spacer2 = new QSpacerItem(10, 20,
    //                                 QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_iconLabel = new QLabel();


    QPushButton* moveUpButton = new QPushButton();
    moveUpButton->setToolTip(tr("Move plot up by one."));
    moveUpButton->setIcon(gtApp->icon("arrowUpBlueAll.png"));
    moveUpButton->setIconSize(QSize(16, 16));
    moveUpButton->setFlat(true);
    moveUpButton->setMaximumWidth(23);

    connect(moveUpButton, SIGNAL(clicked(bool)),
            SLOT(moveUp()));

    QPushButton* moveDownButton = new QPushButton();
    moveDownButton->setToolTip(tr("Move plot down by one."));
    moveDownButton->setIcon(gtApp->icon("arrowDownBlueAll.png"));
    moveDownButton->setIconSize(QSize(16, 16));
    moveDownButton->setFlat(true);
    moveDownButton->setMaximumWidth(23);

    connect(moveDownButton, SIGNAL(clicked(bool)),
            SLOT(moveDown()));

    m_configButton = new QPushButton();
    m_configButton->setToolTip(tr("Configurate the plot."));
    m_configButton->setIcon(gtApp->icon("configIcon_16.png"));
    m_configButton->setIconSize(QSize(16, 16));
    m_configButton->setFlat(true);
    m_configButton->setMaximumWidth(23);

    connect(m_configButton, SIGNAL(clicked(bool)),
            SLOT(configurePlot()));

    m_printButton = new QPushButton();
    m_printButton->setToolTip(tr("Print the plot."));
    m_printButton->setIcon(gtApp->icon("printerIcon_16.png"));
    m_printButton->setIconSize(QSize(16, 16));
    m_printButton->setFlat(true);
    m_printButton->setMaximumWidth(23);

    connect(m_printButton, SIGNAL(clicked(bool)),
            SLOT(exportPlot()));

    m_copyButton = new QPushButton();
    m_copyButton->setToolTip(tr("Copy the plot."));
    m_copyButton->setIcon(gtApp->icon("copyIcon_16.png"));
    m_copyButton->setIconSize(QSize(16, 16));
    m_copyButton->setFlat(true);
    m_copyButton->setMaximumWidth(23);

    connect(m_copyButton, SIGNAL(clicked(bool)),
            SLOT(copyPlot()));

    m_closeButton = new QPushButton();
    m_closeButton->setToolTip(tr("Delete the plot."));
    m_closeButton->setIcon(gtApp->icon("closeIcon_16.png"));
    m_closeButton->setIconSize(QSize(16, 16));
    m_closeButton->setFlat(true);
    m_closeButton->setMaximumWidth(23);

    connect(m_closeButton, SIGNAL(clicked(bool)),
            SLOT(deletePlot()));

    m_titleLineEdit = new QLineEdit();
    QFont font;
    font.setPointSize(14);
    m_titleLineEdit->setFont(font);
    m_titleLineEdit->setAlignment(Qt::AlignCenter);
    m_titleLineEdit->setLayoutDirection(Qt::LeftToRight);
    m_titleLineEdit->setFrame(false);
    m_titleLineEdit->setStyleSheet(GtStyleSheets::standardLineEdit());

    QString plotName = m_widget->providerName(); //m_widget->objectName();
    m_titleLineEdit->setText(plotName);

    connect(m_widget, SIGNAL(providerNameChanged(QString)),
            this, SLOT(onProvidernameChanged(QString)));

    connect(m_titleLineEdit, SIGNAL(textChanged(QString)),
            SLOT(plotNameChanged(QString)));

    m_tollBarLayout->addWidget(m_iconLabel);
    //    m_tollBarLayout->addSpacerItem(spacer);
    m_tollBarLayout->addWidget(m_titleLineEdit);
    //    m_tollBarLayout->addSpacerItem(spacer2);
    m_tollBarLayout->addWidget(moveUpButton);
    m_tollBarLayout->addWidget(moveDownButton);
    m_tollBarLayout->addWidget(m_configButton);
    m_tollBarLayout->addWidget(m_copyButton);
    m_tollBarLayout->addWidget(m_printButton);
    m_tollBarLayout->addWidget(m_closeButton);

    m_mainLayout = new QHBoxLayout();

    main->addLayout(m_tollBarLayout);
    main->addLayout(m_mainLayout);

    m_mainLayout->addWidget(m_widget);

    adjustToolBar();

    this->setLayout(main);
}

//void
//GtAbstractItemUI::refresh()
//{
////    m_widget->updatePlot();
//    m_widget->initFillingUI();
//}

void
GtAbstractItemUI::resize()
{
    m_widget->resizePlot();
}

void
GtAbstractItemUI::setWidget(GtAbstractPostWidget* widget)
{
    if (widget == Q_NULLPTR)
    {
        gtDebug() << tr("Cannot set Widget in GtAbstractItemUI, "
                        "because widget is a Nullptr.");
        return;
    }

    m_widget = widget;
}

void
GtAbstractItemUI::setHasConfigMenu(bool show)
{
    if (!show)
    {
        m_configButton->setEnabled(false);
    }
}

void
GtAbstractItemUI::setIsPrintable(bool show)
{
    if (!show)
    {
        m_printButton->setEnabled(false);
    }
}

GtAbstractPostWidget*
GtAbstractItemUI::widget()
{
    return m_widget;
}

void
GtAbstractItemUI::createConfigMenu(const QList<GtPostAction*>& /*list*/)
{

}

void
GtAbstractItemUI::adjustToolBar()
{
    if (m_widget == Q_NULLPTR)
    {
        gtDebug() << tr("Widget is a Nullptr, cannot adjust toolbar!");
        return;
    }

    // tool buttons initialization
    setHasConfigMenu(m_widget->hasConfigMenu());
    setIsPrintable(m_widget->isPrintable());

    // icon initialization
    QString icon = m_widget->iconString();

    if (icon.isEmpty())
    {
        icon = "emptyElementIcon_16.png";
    }

    QIcon plotIcon = gtApp->icon(icon);
    m_iconLabel->setPixmap(plotIcon.pixmap(20, 20));
}

void
GtAbstractItemUI::printEmbedded(QPrinter* printer,
                                QPainter* painter,
                                QRectF rect)
{
    m_widget->printEmbedded(printer, painter, rect);
}

GtPostTemplateItem*
GtAbstractItemUI::data()
{
    if (m_widget == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (m_widget->data() == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return m_widget->data();
}

void
GtAbstractItemUI::exportPlot()
{
    QMenu menu;

    //QAction* actprint = menu.addAction(tr("Print..."));
    QAction* actexport = menu.addAction(tr("Export to PDF..."));
    QAction* expPng = menu.addAction(tr("Export to PNG..."));

    //actprint->setIcon(gtApp->icon("printerIcon_16.png"));
    actexport->setIcon(gtApp->icon("printPDFIcon.png"));
    expPng->setIcon(gtApp->icon("saveProjectIcon_24.png"));

    QAction* a = menu.exec(QCursor::pos());

    /*    if (a == actprint)
        {
            //        plotToPrinter();
        }

        else if (a == actexport)*/
    if (a == actexport)
    {
        m_widget->plotToPdf();
    }
    else if (a == expPng)
    {
        m_widget->plotToPng();
    }
}

void
GtAbstractItemUI::copyPlot()
{
    // parameter is the datamodel representation
    emit copyUI(m_widget->data());
}

void
GtAbstractItemUI::deletePlot()
{
    // parameter is the datamodel representation
    emit deleteUI(this);
}

void
GtAbstractItemUI::configurePlot()
{
    if (m_widget == Q_NULLPTR)
    {
        gtWarning() << tr("Widget is a Nullptr, cannot show config menu.");
    }

    if (m_widget->data() == Q_NULLPTR)
    {
        gtWarning() << tr("Data of Widget is a Nullptr, "
                          "cannot show config menu.");
    }

    QMenu menu(this);

    GtCustomActionMenu* cam = new GtCustomActionMenu(m_widget->configActions(),
            m_widget->data(),
            m_widget, &menu);

    Q_UNUSED(cam);

    menu.exec(QCursor::pos());
}

void
GtAbstractItemUI::plotNameChanged(QString s)
{
    m_widget->changePlotName(s);
}

void
GtAbstractItemUI::moveUp()
{
    emit moveSignal(GtTemplateViewer::MOVE_UP);
}

void
GtAbstractItemUI::moveDown()
{
    emit moveSignal(GtTemplateViewer::MOVE_DOWN);
}

void
GtAbstractItemUI::onProvidernameChanged(QString s)
{
    Q_UNUSED(s)
    // nothing to do in here
//    m_titleLineEdit->setText(s);
}

void
GtAbstractItemUI::updatePlots()
{
    updatePlot();
}
