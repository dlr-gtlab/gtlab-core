/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include "gt_textedit.h"
#include "gt_changelogwidget.h"

GtChangeLogWidget::GtChangeLogWidget(QString const& mdContent,
                                     QWidget* parent) :
    QWidget(parent),
    m_textEdit(nullptr),
    m_parser(mdContent)
{
    auto* l = new QVBoxLayout;

    QString text = m_parser.currentReleaseMD().toMarkDownText();
    if (text.isEmpty())
    {
        text = QStringLiteral("No changelog could be found");
    }

    if (m_parser.versions().size() > 1)
    {
        text += QString("...");
    }

    m_textEdit = new GtTextEdit(text, GtTextEdit::MD);
    m_textEdit->setFrameStyle(QTextEdit::NoFrame);
    m_textEdit->setReadOnly(true);

    QList<GtChangeLogEntry> entries = m_parser.allReleasesDM();
    QString longText;

    for (GtChangeLogEntry const& e : qAsConst(entries))
    {
        /// the part in the middle is needed to add a blank line between
        /// the entries
        // cppcheck-suppress useStlAlgorithm
        longText = e.toMarkDownText() + QString("&nbsp;     \r") + longText;
    }

    if (longText.isEmpty())
    {
        longText = "";
    }

    m_textEdit->setLongText(longText);

    if (m_parser.versions().size() > 1)
    {
        m_textEdit->setToolTip(tr("Double click to expand whole changelog"));
    }

    l->addWidget(m_textEdit);
    setLayout(l);
}
