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

struct GtSearchWidget::Impl
{
    /// Search line
    GtLineEdit* m_searchLine;

    /// Search label
    QLabel* m_searchLabel;

    /// Search button
    QPushButton* m_searchButton;

    /// Clear search button
    QPushButton* m_clearButton;

    /// Next match button
    QPushButton* m_nextButton;

    /// Previous match button
    QPushButton* m_prevButton;

    bool m_useNextButtons = false;

    void prepareSearchButton()
    {
        m_searchButton->setIcon(gt::gui::icon::search());
        m_searchButton->setMaximumSize(QSize(20, 20));
        m_searchButton->setFlat(true);
        m_searchButton->setToolTip(tr("Search Output"));
    }

    void prepareClearButton()
    {
        m_clearButton->setIcon(gt::gui::icon::backspaceFlipped());
        m_clearButton->setMaximumSize(QSize(20, 20));
        m_clearButton->setFlat(true);
        m_clearButton->setToolTip(tr("Clear Search"));
        m_clearButton->setVisible(false);
    }

    void prepareNextButton()
    {
        m_nextButton->setIcon(gt::gui::icon::arrowDown());
        m_nextButton->setMaximumSize(QSize(20, 20));
        m_nextButton->setFlat(true);
        m_nextButton->setToolTip(tr("Next Match"));
        m_nextButton->setVisible(false);
    }

    void preparePrevButton()
    {
        m_prevButton->setIcon(gt::gui::icon::arrowUp());
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
    auto* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    // search log button
    pimpl->m_searchButton = new QPushButton;
    pimpl->prepareSearchButton();
    filterLayout->addWidget(pimpl->m_searchButton);
    connect(pimpl->m_searchButton, SIGNAL(clicked(bool)),
            SLOT(enableSearch()));

    pimpl->m_clearButton = new QPushButton;
    pimpl->prepareClearButton();
    filterLayout->addWidget(pimpl->m_clearButton);
    connect(pimpl->m_clearButton, SIGNAL(clicked(bool)),
            SLOT(disableSearch()));

    // Next match button
    pimpl->m_nextButton = new QPushButton;
    pimpl->prepareNextButton();
    connect(pimpl->m_nextButton, &QPushButton::clicked, this,
            &GtSearchWidget::nextClicked);

    // Previous match button
    pimpl->m_prevButton = new QPushButton;
    pimpl->preparePrevButton();
    connect(pimpl->m_prevButton, &QPushButton::clicked, this,
            &GtSearchWidget::prevClicked);

    filterLayout->addWidget(pimpl->m_prevButton);
    filterLayout->addWidget(pimpl->m_nextButton);

    QKeySequence s = gtApp->getShortCutSequence("search");
    QString searchShortCut = s.toString();
    pimpl->m_searchLabel = new QLabel("<font color='grey'>  (" + searchShortCut
                               + ")</font>");
    QFont font = pimpl->m_searchLabel->font();
    font.setItalic(true);
    font.setPointSize(7);
    pimpl->m_searchLabel->setFont(font);
    pimpl->m_searchLabel->installEventFilter(this);
    filterLayout->addWidget(pimpl->m_searchLabel);

    connect(gtApp, SIGNAL(shortCutsChanged()), this,
            SLOT(onShortCutChanged()));

    pimpl->m_searchLine = new GtLineEdit;
    pimpl->m_searchLine->setMaximumHeight(17);
//    m_searchLine->setFrame(false);
    pimpl->m_searchLine->setStyleSheet(gt::gui::stylesheet::standardLineEdit());
    pimpl->m_searchLine->setVisible(false);
    filterLayout->addWidget(pimpl->m_searchLine);

    filterLayout->addStretch(1);

    connect(pimpl->m_searchLine, SIGNAL(textEdited(QString)), this,
            SIGNAL(textEdited(QString)));
    connect(pimpl->m_searchLine, SIGNAL(textChanged(QString)), this,
            SIGNAL(textChanged(QString)));
    connect(pimpl->m_searchLine, SIGNAL(returnPressed()), this,
            SIGNAL(returnPressed()));
    connect(pimpl->m_searchLine, SIGNAL(clearFocusOut()),
            SLOT(disableSearch()));

    setLayout(filterLayout);
}

GtSearchWidget::~GtSearchWidget() = default;

QString
GtSearchWidget::text()
{
    return pimpl->m_searchLine->text();
}

void
GtSearchWidget::setText(const QString& text)
{
    pimpl->m_searchLine->setText(text);
}

void
GtSearchWidget::enableFindNextButtons()
{
    pimpl->m_useNextButtons = true;
}

void
GtSearchWidget::enableSearch()
{
    pimpl->m_searchButton->setVisible(false);
    pimpl->m_searchLabel->setVisible(false);
    pimpl->m_clearButton->setVisible(true);
    pimpl->m_searchLine->setVisible(true);
    pimpl->m_nextButton->setVisible(pimpl->m_useNextButtons);
    pimpl->m_prevButton->setVisible(pimpl->m_useNextButtons);
    pimpl->m_searchLine->selectAll();
    pimpl->m_searchLine->setFocus();

    emit searchEnabled();
}

void
GtSearchWidget::disableSearch()
{
    pimpl->m_searchButton->setVisible(true);
    pimpl->m_searchLabel->setVisible(true);
    pimpl->m_clearButton->setVisible(false);
    pimpl->m_nextButton->setVisible(false);
    pimpl->m_prevButton->setVisible(false);
    pimpl->m_searchLine->clear();
    pimpl->m_searchLine->setVisible(false);

    emit searchDisabled();
}

void
GtSearchWidget::onShortCutChanged()
{
    QKeySequence s = gtApp->getShortCutSequence("search");
    QString searchShortCut = s.toString();
    pimpl->m_searchLabel->setText("<font color='grey'>  (" + searchShortCut
                           + ")</font>");
}

bool
GtSearchWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == pimpl->m_searchLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            if (auto mouseEvent = dynamic_cast<QMouseEvent*>(event))
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

