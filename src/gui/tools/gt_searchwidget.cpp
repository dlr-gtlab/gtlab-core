/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include "gt_icons.h"
#include "gt_stylesheets.h"
#include "gt_application.h"
#include "gt_logging.h"

struct GtSearchWidget::Impl
{
    /// Next match button
    QPushButton* m_nextButton;
    /// Previous match button
    QPushButton* m_prevButton;

    bool m_useNextButtons = false;;

    void prepareNextButton()
    {
        m_nextButton->setText(QStringLiteral(">"));
        m_nextButton->setMaximumSize(QSize(20, 20));
        m_nextButton->setFlat(true);
        m_nextButton->setToolTip(tr("Next Match"));
        m_nextButton->setVisible(false);
    }

    void preparePrevButton()
    {
        m_prevButton->setText(QStringLiteral("<"));
        m_prevButton->setMaximumSize(QSize(20, 20));
        m_prevButton->setFlat(true);
        m_prevButton->setToolTip(tr("Previous Match"));
        m_prevButton->setVisible(false);
    }
};

GtSearchWidget::GtSearchWidget(QWidget* parent) :
    QWidget(parent),
    pimpl(std::make_unique<Impl>())
{
    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    // search log button
    m_searchButton = new QPushButton;
    m_searchButton->setIcon(gt::gui::icon::search());
    m_searchButton->setMaximumSize(QSize(20, 20));
    m_searchButton->setFlat(true);
    m_searchButton->setToolTip(tr("Search Output"));
    filterLayout->addWidget(m_searchButton);
    connect(m_searchButton, SIGNAL(clicked(bool)),
            SLOT(enableSearch()));

    m_clearButton = new QPushButton;
    m_clearButton->setIcon(gt::gui::icon::backspaceFlipped());
    m_clearButton->setMaximumSize(QSize(20, 20));
    m_clearButton->setFlat(true);
    m_clearButton->setToolTip(tr("Clear Search"));
    m_clearButton->setVisible(false);
    filterLayout->addWidget(m_clearButton);
    connect(m_clearButton, SIGNAL(clicked(bool)),
            SLOT(disableSearch()));

    // Next match button
    pimpl->m_nextButton = new QPushButton;
    pimpl->prepareNextButton();
    filterLayout->addWidget(pimpl->m_nextButton);
    connect(pimpl->m_nextButton, &QPushButton::clicked, this,
            &GtSearchWidget::nextClicked);

    // Previous match button
    pimpl->m_prevButton = new QPushButton;
    pimpl->preparePrevButton();
    filterLayout->addWidget(pimpl->m_prevButton);
    connect(pimpl->m_prevButton, &QPushButton::clicked, this,
            &GtSearchWidget::prevClicked);

    QKeySequence s = gtApp->getShortCutSequence("search");
    QString searchShortCut = s.toString();
    m_searchLabel = new QLabel("<font color='grey'>  (" + searchShortCut
                               + ")</font>");
    QFont font = m_searchLabel->font();
    font.setItalic(true);
    font.setPointSize(7);
    m_searchLabel->setFont(font);
    m_searchLabel->installEventFilter(this);
    filterLayout->addWidget(m_searchLabel);

    connect(gtApp, SIGNAL(shortCutsChanged()), this,
            SLOT(onShortCutChanged()));

    m_searchLine = new GtLineEdit;
    m_searchLine->setMaximumHeight(17);
//    m_searchLine->setFrame(false);
    m_searchLine->setStyleSheet(gt::gui::stylesheet::standardLineEdit());
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

GtSearchWidget::~GtSearchWidget()
{

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
GtSearchWidget::enableFindNextButtons()
{
    pimpl->m_useNextButtons = true;
}

void
GtSearchWidget::enableSearch()
{
    m_searchButton->setVisible(false);
    m_searchLabel->setVisible(false);
    m_clearButton->setVisible(true);
    m_searchLine->setVisible(true);
    pimpl->m_nextButton->setVisible(pimpl->m_useNextButtons);
    pimpl->m_prevButton->setVisible(pimpl->m_useNextButtons);
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
    pimpl->m_nextButton->setVisible(false);
    pimpl->m_prevButton->setVisible(false);
    m_searchLine->clear();
    m_searchLine->setVisible(false);

    emit searchDisabled();
}

void
GtSearchWidget::onShortCutChanged()
{
    QKeySequence s = gtApp->getShortCutSequence("search");
    QString searchShortCut = s.toString();
    m_searchLabel->setText("<font color='grey'>  (" + searchShortCut
                           + ")</font>");
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

