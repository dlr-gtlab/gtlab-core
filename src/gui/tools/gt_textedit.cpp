/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */
#include "gt_textedit.h"

GtTextEdit::GtTextEdit(QString text, contentType type, QWidget* parent) :
    QTextEdit(parent),
    m_text(text),
    m_long(QString{}),
    m_type(type)
{
    if (m_type == MD)
    {
        setMarkdown(text);
    }
    else if (m_type == HTML)
    {
        setHtml(text);
    }
    else
    {
        setText(text);
    }
}

GtTextEdit&
GtTextEdit::setLongText(const QString &text)
{
    m_long = text;
    if (!text.isEmpty())
    {
        setToolTip(tr("Double click to see whole text"));
    }
    return *this;
}

void
GtTextEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event)

    if (m_long.isEmpty())
    {
        return;
    }

    if (m_type == MD)
    {
        setMarkdown(m_long);
    }
    else if (m_type == HTML)
    {
        setHtml(m_long);
    }
    else
    {
        setText(m_long);
    }

    setToolTip("");
}

GtTextEdit::contentType
GtTextEdit::typeFromFile(const QString& file)
{
    contentType retVal = NONE;

    if (file.endsWith(".md"))
    {
        retVal = GtTextEdit::MD;
    }
    else if (file.endsWith(".txt"))
    {
        retVal = GtTextEdit::TEXT;
    }
    else if (file.endsWith(".html"))
    {
        retVal = GtTextEdit::HTML;
    }
    else
    {
        retVal = GtTextEdit::ELSE;
    }

    return retVal;
}

void
GtTextEdit::updateText(const QString& text)
{
    if (m_type == MD)
    {
        setMarkdown(text);
    }
    else if (m_type == HTML)
    {
        setHtml(text);
    }
    else
    {
        setText(text);
    }
}
