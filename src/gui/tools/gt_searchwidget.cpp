/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 21.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>

#include "gt_searchwidget.h"
#include "gt_lineedit.h"
#include "gt_application.h"
#include "gt_stylesheets.h"

GtSearchWidget::GtSearchWidget(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    // search log button
    m_searchButton = new QPushButton;
    m_searchButton->setIcon(gtApp->icon("searchIcon_16.png"));
    m_searchButton->setMaximumSize(QSize(20, 20));
    m_searchButton->setFlat(true);
    m_searchButton->setToolTip(tr("Search Output"));
    filterLayout->addWidget(m_searchButton);
    connect(m_searchButton, SIGNAL(clicked(bool)),
            SLOT(enableSearch()));

    m_clearButton = new QPushButton;
    m_clearButton->setIcon(gtApp->icon("clearIcon2_16.png"));
    m_clearButton->setMaximumSize(QSize(20, 20));
    m_clearButton->setFlat(true);
    m_clearButton->setToolTip(tr("Clear Search"));
    m_clearButton->setVisible(false);
    filterLayout->addWidget(m_clearButton);
    connect(m_clearButton, SIGNAL(clicked(bool)),
            SLOT(disableSearch()));

    m_searchLabel = new QLabel("<font color='grey'>  (Ctrl+F)</font>");
    QFont font = m_searchLabel->font();
    font.setItalic(true);
    font.setPointSize(7);
    m_searchLabel->setFont(font);
    m_searchLabel->installEventFilter(this);
    filterLayout->addWidget(m_searchLabel);

    m_searchLine = new GtLineEdit;
    m_searchLine->setMaximumHeight(17);
//    m_searchLine->setFrame(false);
    m_searchLine->setStyleSheet(GtStyleSheets::standardLineEdit());
    m_searchLine->setVisible(false);
    filterLayout->addWidget(m_searchLine);

    filterLayout->addStretch(1);

    connect(m_searchLine, SIGNAL(textEdited(QString)), this,
            SIGNAL(textEdited(QString)));
    connect(m_searchLine, SIGNAL(textChanged(QString)), this,
            SIGNAL(textChanged(QString)));
    connect(m_searchLine, SIGNAL(returnPressed()), this,
            SIGNAL(returnPressed()));
    connect(m_searchLine, SIGNAL(clearFocusOut()),
            SLOT(disableSearch()));

    setLayout(filterLayout);
}

QString
GtSearchWidget::text()
{
    return m_searchLine->text();
}

void
GtSearchWidget::setText(const QString& text)
{
    m_searchLine->setText(text);
}

void
GtSearchWidget::enableSearch()
{
    m_searchButton->setVisible(false);
    m_searchLabel->setVisible(false);
    m_clearButton->setVisible(true);
    m_searchLine->setVisible(true);
    m_searchLine->selectAll();
    m_searchLine->setFocus();

    emit searchEnabled();
}

void
GtSearchWidget::disableSearch()
{
    m_searchButton->setVisible(true);
    m_searchLabel->setVisible(true);
    m_clearButton->setVisible(false);
    m_searchLine->clear();
    m_searchLine->setVisible(false);

    emit searchDisabled();
}

bool
GtSearchWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_searchLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (mouseEvent)
            {
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    enableSearch();
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

