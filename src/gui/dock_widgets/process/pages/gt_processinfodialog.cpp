/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processinfodialog.cpp
 */

#include "gt_processinfodialog.h"

#include <QMap>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>

GtProcessInfoPopup::GtProcessInfoPopup(
    gt::ProcessInformation const& processInfos,
    QWidget* parent) :
    QFrame(parent, Qt::Popup |
                       Qt::FramelessWindowHint |
                       Qt::NoDropShadowWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    auto* card = new QWidget(this);
    card->setObjectName("card");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->addWidget(card);

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    card->setLayout(cardLayout);

    //
    // Header
    //
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);

    headerLayout->setContentsMargins(16, 12, 16, 12);

    auto* titleLabel = new QLabel(processInfos.id, header);

    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    titleLabel->setFont(titleFont);

    auto* versionLabel = new QLabel(processInfos.version.toString(), header);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(versionLabel);

    //
    // Content
    //
    auto* content = new QWidget(this);
    auto* grid = new QGridLayout(content);

    grid->setContentsMargins(16, 12, 16, 16);
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);

    int row = 0;

    auto addLine = [&](QString const& key, QString const& value) {
        auto* keyLabel = new QLabel(key + ":", content);

        auto* valueLabel = new QLabel(value, content);
        valueLabel->setWordWrap(true);

        QFont bold = keyLabel->font();
        bold.setBold(true);
        keyLabel->setFont(bold);

        grid->addWidget(keyLabel, row, 0, Qt::AlignTop);
        grid->addWidget(valueLabel, row, 1);

        ++row;
    };

    // optional if set
    if (processInfos.description != "-")
        addLine(tr("Description"), processInfos.description);

    if (processInfos.author != "-")
        addLine(tr("Author"), processInfos.author);

    if (processInfos.contact != "-")
        addLine(tr("Contact"), processInfos.contact);

    if (processInfos.company != "-")
        addLine(tr("Copany"), processInfos.company);

    cardLayout->addWidget(header);
    cardLayout->addWidget(content);

    setMinimumWidth(380);

    card->setStyleSheet(R"(
#card {
    background-color: palette(base);
    border-radius: 10px;
    border: 1px solid palette(mid);
}
QLabel {
    border: none;
}
)");

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 80));

    card->setGraphicsEffect(shadow);
}
