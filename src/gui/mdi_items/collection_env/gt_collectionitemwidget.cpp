/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionitemwidget.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 10.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFrame>
#include <QTabWidget>
#include <QScrollArea>
#include <QLabel>

#include "gt_logging.h"
#include "gt_collectionitem.h"
#include "gt_application.h"
#include "gt_palette.h"

#include "gt_collectionitemwidget.h"

GtCollectionItemWidget::GtCollectionItemWidget(
        const GtCollectionItem& collectionItem,
        QWidget* parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle(collectionItem.ident());
    setWindowIcon(gtApp->icon(QStringLiteral("infoIcon_16.png")));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    QFrame* frame = new QFrame;
    frame->setObjectName(QStringLiteral("frame"));
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);

    if (gtApp->inDarkMode())
    {
        frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background_dark.png)}");
    }
    else
    {
        frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background.png)}");

    }


    QVBoxLayout* frameLay = new QVBoxLayout;
    frameLay->setContentsMargins(10, 10, 10, 10);

    QTabWidget* tabWidget = new QTabWidget;

    QWidget* overviewWidget = new QWidget;

    QVBoxLayout* overviewLay = new QVBoxLayout;

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setStyleSheet(
                "QAbstractScrollArea { background-color: transparent; } "
                "QWidget#scrollAreaWidgetContents{"
                " background-color: transparent;} ");
    scrollArea->setFrameStyle(QScrollArea::NoFrame);
    scrollArea->setWidgetResizable(true);

    QWidget* scrollWidget = new QWidget;

    QVBoxLayout* scrollLay = new QVBoxLayout;
    scrollLay->setContentsMargins(0, 0, 0, 0);

    // title widget
    QFrame* titleWidget = newBox(tr("Name:"),
                                collectionItem.ident());
    scrollLay->addWidget(titleWidget);

    // description widget
    QFrame* descWidget = newBox(tr("Description:"),
                                collectionItem.description());
    scrollLay->addWidget(descWidget);

    // version widget
    QFrame* versWidget = newBox(tr("Version:"),
                                QString::number(collectionItem.version()));
    scrollLay->addWidget(versWidget);

    // collection specific properties
    foreach (const QString& str, collectionItem.propertyIds())
    {
        QFrame* specWidget = newBox(str,
                                    collectionItem.property(str).toString());
        scrollLay->addWidget(specWidget);
    }

    // files widget
    QFrame* filesWidget = newFileBox(collectionItem.files());
    scrollLay->addWidget(filesWidget);

    scrollLay->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Minimum,
                                             QSizePolicy::Expanding));

    scrollWidget->setLayout(scrollLay);
    scrollArea->setWidget(scrollWidget);

    overviewLay->addWidget(scrollArea);

    overviewWidget->setLayout(overviewLay);

    tabWidget->addTab(overviewWidget,
                      gtApp->icon(QStringLiteral("listIcon_16.png")),
                      tr("Overview"));

    QWidget* docWidget = new QWidget;

    tabWidget->addTab(docWidget,
                      gtApp->icon(QStringLiteral("questionIcon_16.png")),
                      tr("Documentation"));

    tabWidget->setTabEnabled(1, false);

    frameLay->addWidget(tabWidget);

    frame->setLayout(frameLay);

    lay->addWidget(frame);

    setLayout(lay);

    GtPalette::applyThemeToWidget(this, gtApp->inDarkMode());

    resize(600, 400);
}

GtCollectionItemWidget::~GtCollectionItemWidget()
{
    gtDebug() << "collection item widget deleted!";
}

QString
GtCollectionItemWidget::filterString(const QString& str)
{
    if (m_filter.isEmpty())
    {
        return str;
    }

    QString retval = str;
    QString newStr = QStringLiteral("<font color=red>") + m_filter +
                     QStringLiteral("</font>");

    retval = retval.replace(m_filter, newStr);

    return retval;
}

QFrame*
GtCollectionItemWidget::newFrame()
{
    QFrame* frame = new QFrame(this);

    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);

    return frame;
}

QFrame*
GtCollectionItemWidget::newBox(const QString& title, const QString& content)
{
    QFrame* box = newFrame();

    QVBoxLayout* boxLayout = new QVBoxLayout;

    QLabel* boxTitle = new QLabel(QStringLiteral("<b>") + title +
                                   QStringLiteral("</b>"), this);
    QLabel* boxContent = new QLabel(filterString(content), this);
    boxLayout->addWidget(boxTitle);
    boxLayout->addWidget(boxContent);    

    box->setLayout(boxLayout);

    return box;
}

QFrame*
GtCollectionItemWidget::newFileBox(const QStringList& fileNames)
{
    QFrame* box = newFrame();

    QVBoxLayout* boxLayout = new QVBoxLayout;

    QLabel* boxTitle = new QLabel(QStringLiteral("<b>") + tr("Files") +
                                   QStringLiteral("</b>"), this);
    boxLayout->addWidget(boxTitle);

    foreach (const QString& fileName, fileNames)
    {
        QLayout* fileLay = newFileLayout(fileName);
        boxLayout->addLayout(fileLay);
    }

    box->setLayout(boxLayout);

    return box;
}

QLayout*
GtCollectionItemWidget::newFileLayout(const QString& filename)
{
    QHBoxLayout* fileLay = new QHBoxLayout;
    QLabel* iconLabel = new QLabel(this);
    iconLabel->setMaximumWidth(16);
    iconLabel->setPixmap(
                gtApp->icon(
                    QStringLiteral("fileIcon_16.png")).pixmap(QSize(16, 16)));
    QLabel* fileLabel = new QLabel(filename, this);
    fileLay->addWidget(iconLabel);
    fileLay->addWidget(fileLabel);

    return fileLay;
}