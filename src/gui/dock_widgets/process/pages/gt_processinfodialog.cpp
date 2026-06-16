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
    QString const& title,
    QString const& version,
    QList<QPair<QString, QString>> const& processInfos,
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

    auto* titleLabel = new QLabel(title, header);

    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    titleLabel->setFont(titleFont);

    auto* versionLabel = new QLabel(version, header);

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

    for (auto const& info : processInfos)
    {
        // ignore version as it is handled in the header
        if (info.first == "Version" || info.first == "version") continue;

        // ignore ID as it is handled in the header
        if (info.first == "ID" || info.first == "Id") continue;

        auto* key = new QLabel(info.first + ":", content);
        auto* value = new QLabel(info.second, content);

        value->setWordWrap(true);

        QFont bold = key->font();
        bold.setBold(true);
        key->setFont(bold);

        grid->addWidget(key, row, 0, Qt::AlignTop);
        grid->addWidget(value, row, 1);

        ++row;
    }

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
