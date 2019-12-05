/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on:
 *  Author:   (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "QVBoxLayout"
#include "QPushButton"
#include "QMenu"
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPainter>
#include <QTimer>
#include <QDir>
#include "qscrollarea.h"
#include <QLabel>

#include <qwt_plot_renderer.h>

#include "gt_templateviewer.h"
#include "gt_application.h"
#include "gt_project.h"
#include "gt_posttemplatepath.h"
#include "gt_abstractitemui.h"
#include "gt_abstractpostwidget.h"
#include "gt_posttemplatefactory.h"
#include "gt_abstractobjectfactory.h"
#include "gt_posttemplate.h"
#include "gt_posttemplateitem.h"
#include "gt_abstractpostwidget.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_collectionloadingwidget.h"
#include "gt_scrollarea.h"
#include "gt_postmodel.h"
#include "gt_stylesheets.h"


GtTemplateViewer::GtTemplateViewer() :
    m_templatePath(Q_NULLPTR), m_titleLineEdit(Q_NULLPTR)
{
    setObjectName(tr("Template Viewer"));

    // timer for queued updating
    connect(&m_timer, SIGNAL(timeout()), SLOT(reloadWidgets()));

    QFrame* frame = new QFrame;
    QPalette palette = frame->palette();
    palette.setColor(frame->backgroundRole(), QColor(255, 255, 255));
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);

    m_titleLineEdit = new QLabel();
    QFont font;
    font.setPointSize(14);
    m_titleLineEdit->setFont(font);
    m_titleLineEdit->setAlignment(Qt::AlignCenter);
    m_titleLineEdit->setLayoutDirection(Qt::LeftToRight);
    //m_titleLineEdit->setFrame(false);

    m_scrollarea = new GtScrollArea;
    m_scrollarea->setFrameStyle(GtScrollArea::NoFrame);
    //m_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollarea->setWidgetResizable(true); // !!important to let widgets resize
    //m_scrollarea->setMinimumSize(QSize(400, 300));

    m_plotcontainerwidget = new QWidget(widget());

    m_plotcontainerwidget->setObjectName("mPlotContainerWidgetMain");

    QVBoxLayout* laymainmain = new QVBoxLayout();
    laymainmain->setContentsMargins(0, 0, 0, 0);
    laymainmain->setSpacing(1);
    QHBoxLayout* laymain = new QHBoxLayout();
    QHBoxLayout* laybutton = new QHBoxLayout();
    laybutton->setContentsMargins(3, 3, 3, 3);
    m_layplot = new QVBoxLayout();
    m_layplot->setContentsMargins(0, 0, 0, 0);

    QPushButton* buttonAdd = new QPushButton(tr("Add"));
    buttonAdd->setToolTip("Adds a plot to current template.");
    buttonAdd->setIcon(GtApplication::icon("addIcon_16.png"));
    buttonAdd->setMaximumWidth(60);
    buttonAdd->setStyleSheet(GtStyleSheets::buttonStyleSheet());

    QPushButton* buttonPDF = new QPushButton(tr("To PDF"));
    buttonPDF->setToolTip("Prints template to PDF.");
    buttonPDF->setIcon(GtApplication::icon("printPDFIcon.png"));
    buttonPDF->setMaximumWidth(60);
    buttonPDF->setStyleSheet(buttonAdd->styleSheet());

    QPushButton* buttonPrint = new QPushButton(tr("Print..."));
    buttonPrint->setToolTip("Prints template.");
    buttonPrint->setIcon(GtApplication::icon("printerIcon_16.png"));
    buttonPrint->setMaximumWidth(60);
    buttonPrint->setStyleSheet(buttonAdd->styleSheet());

    connect(buttonAdd, SIGNAL(clicked(bool)), SLOT(addButtonClicked()));
    connect(buttonPrint, SIGNAL(clicked(bool)), SLOT(printButtonClicked()));
    connect(buttonPDF, SIGNAL(clicked(bool)), SLOT(pdfButtonClicked()));

    laybutton->addWidget(m_titleLineEdit);

    laybutton->addWidget(buttonPrint);
    laybutton->addWidget(buttonPDF);
    laybutton->addWidget(buttonAdd);

    m_plotcontainerwidget->setLayout(m_layplot);
    m_scrollarea->setWidget(m_plotcontainerwidget);

    laymain->addWidget(m_scrollarea);

    m_fetchMovie = new GtCollectionLoadingWidget;
    m_fetchMovie->hide();

    laymain->addWidget(m_fetchMovie);


    laybutton->setSizeConstraint(QLayout::SetMinimumSize);
    frame->setLayout(laybutton);
    laymainmain->addWidget(frame);

    QFrame* line = new QFrame;
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    laymainmain->addWidget(line);

    laymainmain->addLayout(laymain);

    widget()->setLayout(laymainmain);

    m_postTemplateDM = new GtPostTemplate();

    m_plotcontainerwidget->adjustSize();

    connect(&m_fileWatcher, SIGNAL(fileChanged(QString)),
            SLOT(onFileChanged(QString)));
}

GtTemplateViewer::~GtTemplateViewer()
{
    saveButtonClicked();

    if (m_templatePath != Q_NULLPTR)
    {
        delete m_templatePath;
    }

    if (m_postTemplateDM != Q_NULLPTR)
    {
        delete m_postTemplateDM;
    }
}

void
GtTemplateViewer::setData(GtObject* obj)
{
    if (obj == Q_NULLPTR)
    {
        return;
    }

    m_templatePath = qobject_cast<GtPostTemplatePath*>(obj);

    if (m_templatePath == Q_NULLPTR)
    {
        return;
    }

    if (!m_fileWatcher.addPath(m_templatePath->path()))
    {
        gtError() << tr("template path not found!");
        return;
    }

    readFromXMLmemento(m_templatePath->path());

    createPlots();

    m_plotcontainerwidget->adjustSize();

    m_titleLineEdit->clearFocus();

    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            this, SLOT(deleteLater()));

}

void
GtTemplateViewer::readFromXMLmemento(const QString& path)
{
    QFile file(path);

    if (!file.exists())
    {
        gtDebug() << tr("No previous template views "
                        "can not get possible saved template views");
        return;
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!document.setContent(&file, true, &errorStr,
                             &errorLine, &errorColumn))
    {
        gtDebug() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return;
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTlabPost")))
    {
        gtDebug() << tr("ERROR: Invalid GTlab Template view file!");
        return;
    }

    m_templateId = root.attribute(QStringLiteral("name"));

    m_titleLineEdit->setText(m_templateId);

    QString title = m_templateId + QStringLiteral(" - ") +
                    tr("Template Viewer");
    setObjectName(title);

    root = root.firstChildElement(); // go to <objectlist>
    root = root.firstChildElement(); // go to <object>

    while (!root.isNull())
    {
        GtObjectMemento memento(root);

        GtObject* obj = memento.restore(gtPostTemplateFactory);

        if (obj == Q_NULLPTR)
        {
            gtWarning() << tr("Object cannot be restored while reading "
                              "template viewer post file");
        }

        // createUIs by calling abstract DM class function
        GtPostTemplateItem* abstractDM = dynamic_cast<GtPostTemplateItem*>(obj);

        if (abstractDM != Q_NULLPTR)
        {
            if (!m_postTemplateDM->appendChild(abstractDM))
            {
                gtWarning() << tr("Could not append child '")
                            << abstractDM->objectName()
                            << tr("' to datamodel whilst reading xml file!");
            }
        }

        root = root.nextSiblingElement();
    }
}


void
GtTemplateViewer::copyUI(GtPostTemplateItem* ti)
{
    if (ti == Q_NULLPTR)
    {
        return;
    }

    GtPostTemplateItem* copy = qobject_cast<GtPostTemplateItem*>(ti->copy());

    m_postTemplateDM->appendChild(copy);

    createPlot(copy);
}

void
GtTemplateViewer::printPreview(QPrinter* printer)
{
    QPainter painter;

    painter.begin(printer);

    QFont font = painter.font();
    font.setPointSize(14);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(50, 90, objectName());

    font.setPointSize(10);
    font.setBold(false);
    painter.setFont(font);

    QPixmap headline(":/pixmaps/print-headline.png");
    painter.drawPixmap(50, 130, printer->width() - 100, headline.height(),
                       headline);

    int pos = 0;

    for (int i = 0; i < m_itemUIList.size(); ++i)
    {
        GtAbstractItemUI* ui = m_itemUIList.at(i);

        if (ui == Q_NULLPTR)
        {
            return;
        }

        QString diagramTitle = "";

        if (ui->widget() == Q_NULLPTR)
        {
            gtDebug() << "Data is nullPointer";
        }
        else
        {
            /// get objectName of provider
            diagramTitle = ui->widget()->providerName();
        }

        bool isLast = true;

        if (m_itemUIList.last() != ui)
        {
            isLast = false;
        }

        int w = printer->width() - 100;
        int h = printer->height() / 2 - 370;

        /// + 200 for the additional header of the plot (BS 04.01.18)
        int off = 200;

        if (pos != 160 + off)
        {
            pos = 160 + off;
        }
        else
        {
            pos = pos + (printer->height() / 2) - 120;
        }

        //write title
        font.setPointSize(12);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(50, /*300*/ pos - 60, diagramTitle);

        ui->printEmbedded(printer, &painter, QRectF(50, pos, w, h));

        if (!isLast && (pos != 160 + off))
        {
            printer->newPage();
        }
    }

    painter.end();
}

void
GtTemplateViewer::print()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);

    Qt::WindowFlags flags = preview.windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    preview.setWindowFlags(flags);

    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
            SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void
GtTemplateViewer::onFileChanged(const QString& path)
{
    if (!QFile(path).exists())
    {
        deleteLater();
    }
}

void
GtTemplateViewer::onRefreshRequest()
{
    m_scrollarea->hide();
    m_fetchMovie->show();
    m_fetchMovie->runAnimation();

    // lock mutex
    m_mutex.lock();

    GtAbstractPostWidget* wid = qobject_cast<GtAbstractPostWidget*>(sender());

    if (wid == Q_NULLPTR)
    {
        return;
    }

    // check whether item is already in the queue
    if (!m_widgetList.contains(wid))
    {
        // add widget to queue
        m_widgetList.append(wid);
    }

    // restart timer
    m_timer.start(500);

    // unlock mutex
    m_mutex.unlock();
}

void
GtTemplateViewer::createSpecialPlot(QString plotName)
{
    GtObject* obj = gtPostTemplateFactory->newObject(plotName,
                    m_postTemplateDM);

    GtPostTemplateItem* abstractDM = qobject_cast<GtPostTemplateItem*>(obj);

    createPlot(abstractDM);
}

bool
GtTemplateViewer::createPlot(GtPostTemplateItem* data)
{
    if (data == Q_NULLPTR)
    {
        gtWarning() << tr("Data is a Nullptr, cannot create Plot.");

        return false;
    }

    GtAbstractPostWidget* widget = data->createUI(data, m_plotcontainerwidget);

    if (widget == Q_NULLPTR)
    {
        gtWarning() << tr("Widget is a Nullptr, cannot create Plot.");

        return false;
    }

    GtAbstractItemUI* uiItem = new GtAbstractItemUI(widget);

    if (m_itemUIList.contains(uiItem))
    {
        gtWarning() << tr("AbstractItemUI already contained in item List.");

        return false;
    }

    m_itemUIList.append(uiItem);
    m_layplot->addWidget(uiItem);

    // connect important for delete
    connect(uiItem, SIGNAL(deleteUI(GtAbstractItemUI*)),
            SLOT(deleteUI(GtAbstractItemUI*)));

    // connect important for delete
    connect(uiItem, SIGNAL(copyUI(GtPostTemplateItem*)),
            SLOT(copyUI(GtPostTemplateItem*)));

    connect(uiItem->widget(), SIGNAL(refreshRequest()),
            SLOT(onRefreshRequest()));

    connect(uiItem, SIGNAL(moveSignal(GtTemplateViewer::MoveDirection)),
            SLOT(moveWidget(GtTemplateViewer::MoveDirection)));

    return true;
}

bool
GtTemplateViewer::createPlots()
{
    foreach (GtPostTemplateItem* item, m_postTemplateDM->items())
    {
        if (item == Q_NULLPTR)
        {
            gtWarning() << tr("GtTemplateViewer::createPlots() Item is null");

            return false;
        }

        if (!createPlot(item))
        {
            gtWarning() << tr("Could not create Plot with name '")
                        << item->objectName() << "'";
            return false;
        }
    }

    return true;
}

void
GtTemplateViewer::showEvent()
{
    foreach (GtAbstractItemUI* ai, m_itemUIList)
    {
        ai->checkDataConnections();
    }
}

void
GtTemplateViewer::moveWidget(GtTemplateViewer::MoveDirection d)
{
    // sender is the AbstractItemUI
    GtAbstractItemUI* w = qobject_cast<GtAbstractItemUI*>(sender());

    if (w == Q_NULLPTR)
    {
        gtWarning() << tr("Abstract item is null");

        return;
    }

    // get index of widget in layout
    const int index = m_layplot->indexOf(w);

    // get total number of widgets in layout
    const int total = m_layplot->count();

    // cannot move first widget up
    if (index == 0 && d == MOVE_UP)
    {
        return;
    }

    // cannot move last widget down
    if (index == total - 1 && d == MOVE_DOWN)
    {
        return;
    }

    // get datamodel from widget
    GtPostTemplateItem* item = w->data();

    // determine new index
    const int newIndex = d == MOVE_UP ? index - 1 : index + 1;

    // remove widget
    m_layplot->removeWidget(w);

    // insert widget at dedicated new position
    m_layplot->insertWidget(newIndex, w);

    // move child in data model structure
    item->setParent(Q_NULLPTR);
    m_postTemplateDM->insertChild(newIndex, item);
}

void
GtTemplateViewer::setPlotIcon(QAction* actPlot, QString str)
{
    if (str == "GtBarProvider")
    {
        actPlot->setIcon(GtApplication::icon("histogramIcon_16.png"));
    }
    else if (str == "GtXyProvider")
    {
        actPlot->setIcon(GtApplication::icon("schedules4Icon_16.png"));
    }
    else if (str == "GtpXyProvider")
    {
        actPlot->setIcon(GtApplication::icon("schedules4Icon_16.png"));
    }
    else if (str == "GtppMapProvider")
    {
        actPlot->setIcon(GtApplication::icon("mapIcon_16.png"));
    }
    else if (str == "GtCarpetProvider")
    {
        actPlot->setIcon(GtApplication::icon("carpetPlotIcon2.png"));
    }
    else if (str == "GtdAeroPlot")
    {
        actPlot->setIcon(GtApplication::icon("carpetPlotIcon2.png"));
    }
    else if (str == "GtTSDiagramProvider")
    {
        actPlot->setIcon(GtApplication::icon("tsDiagramm_16.png"));
    }
    else
    {
        actPlot->setIcon(GtApplication::icon("postIcon.png"));
    }
}

QString
GtTemplateViewer::shortPlotName(QString providerName)
{
    if (providerName == "GtBarProvider")
    {
        return "Bar-Chart";
    }
    else if (providerName == "GtXyProvider")
    {
        return "XY-Plot";
    }
    else if (providerName == "GtpXyProvider")
    {
        return "XY-Performance-Plot";
    }
    else if (providerName == "GtppMapProvider")
    {
        return "Component-Map";
    }
    else if (providerName == "GtCarpetProvider")
    {
        return "Carpet-Plot";
    }
    else if (providerName == "GtTSDiagramProvider")
    {
        return "TS-Diagram";
    }
    else if (providerName == "GtsCometPostItem")
    {
        return "Comet Post Processing";
    }
    else if (providerName == "GtdAeroPlot")
    {
        return "Aero Plot";
    }
    else if (providerName == "GtpyPythonPlotItem")
    {
        return "Python Plot";
    }
    else if (providerName == "GtatACDCPostItem")
    {
        return "ACDC Post Processing";
    }
    else if (providerName == "GtatACDCRadialPostItem")
    {
        return "ACDC Radial Profils";
    }
    else if (providerName == "GtspDiskPlotData")
    {
        return "Steady/Transient Disks Plot";
    }
    else
    {
        return providerName;
    }
}

void
GtTemplateViewer::projectSavedEvent(GtProject* /*project*/)
{
    saveButtonClicked();
}

bool
GtTemplateViewer::deleteUI(GtAbstractItemUI* ai)
{
    if (ai == Q_NULLPTR)
    {
        return false;
    }

    GtPostTemplateItem* dm = ai->widget()->data();
    delete dm;

    // TODO need to disconnect the signal/slots?

    m_itemUIList.removeOne(ai);


    if (ai == Q_NULLPTR)
    {
        gtError() << "ai is a nullptr, damn it!";
    }
    else
    {
        delete ai;
    }

    return true;
}

void
GtTemplateViewer::addButtonClicked()
{
    QMenu menu;

    QStringList plots = gtPostTemplateFactory->knownPlots();

    foreach (QString str, plots)
    {
        QString shortName = shortPlotName(str);
        QAction* actPlot = menu.addAction(shortName);
        actPlot->setData(str);

        setPlotIcon(actPlot, str);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == Q_NULLPTR)
    {
        return;
    }

    createSpecialPlot(a->data().toString());

    m_plotcontainerwidget->adjustSize();
}

void
GtTemplateViewer::saveButtonClicked()
{
    if (m_templatePath == Q_NULLPTR)
    {
        return;
    }

    // create DomDocument

    QDomDocument doc;

    QDomProcessingInstruction header = GtPostModel::createDocHeader(doc);
    doc.appendChild(header);

    QDomElement root = GtPostModel::createRoot(doc, m_templateId);

    QDomElement elementol = doc.createElement("objectlist");

    foreach (GtObject* o, m_postTemplateDM->findDirectChildren<GtObject*>())
    {
        GtObjectMemento om = o->toMemento();
        elementol.appendChild(om.documentElement());
    }

    root.appendChild(elementol);
    doc.appendChild(root);

    QString string = doc.toString(5);

    QFile file(m_templatePath->path());

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream TextStream(&file);
        TextStream << string;
        file.close();
    }
}

void
GtTemplateViewer::reloadWidgets()
{
    foreach (GtAbstractPostWidget* w, m_widgetList)
    {
        if (w != Q_NULLPTR)
        {
            w->initFillingUI();
        }
    }

    m_fetchMovie->hide();
    m_fetchMovie->stopAnimation();
    m_scrollarea->show();

    m_widgetList.clear();

    m_timer.stop();
}

void
GtTemplateViewer::resizeButtonClicked()
{
    foreach (GtAbstractItemUI* ui, m_itemUIList)
    {
        if (ui != Q_NULLPTR)
        {
            ui->resize();
        }
    }
}

void
GtTemplateViewer::printButtonClicked()
{
    print();
}

void
GtTemplateViewer::pdfButtonClicked()
{
    QStringList temp = objectName().split("-");

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(temp.first() + ".pdf");
    QPrintPreviewDialog preview(&printer);

    Qt::WindowFlags flags = preview.windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    preview.setWindowFlags(flags);

    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
            SLOT(printPreview(QPrinter*)));
    preview.exec();

    return;
}

