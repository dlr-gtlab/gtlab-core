/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcutedit.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 09.12.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */


#include "gt_shortcutedit.h"

#include <QKeyEvent>


GtShortCutEdit::GtShortCutEdit(const QKeySequence& shortcut,
                               const QString& shortcutID,
                               QWidget* parent) :
    GtLineEdit(parent),
    m_keySequence(shortcut),
    m_lastKey(0)
{
    // object name for identification
    setObjectName(shortcutID);

    setAcceptDrops(false);
    setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    setKeySequence(m_keySequence);

    connect(this, SIGNAL(focusIn()), this, SLOT(onFocusIn()));
    connect(this, SIGNAL(focusOut()), this, SLOT(onFocusOut()));
    connect(this, SIGNAL(clearFocusOut()), this, SLOT(onClearFocusOut()));
}

QKeySequence
GtShortCutEdit::keySequence() const
{
    return m_keySequence;
}

void
GtShortCutEdit::clear()
{
    GtLineEdit::clear();
    m_keySequence = QKeySequence();
}

void
GtShortCutEdit::setKeySequence(const QKeySequence& seq)
{
    m_keySequence = seq;
    this->setText(seq.toString());
}

void
GtShortCutEdit::keyPressEvent(QKeyEvent* event)
{
    event->accept();

    // ESC should abort the input
    if (event->key() == Qt::Key_Escape)
    {
        // clear input if nothing was set
        if (this->text().isEmpty())
        {
            this->clear();
        }

        this->clearFocus();
        return;
    }

    auto modifiers = event->modifiers();
    int  key = event->key();

    // use old key code if current is a modifier
    if (isKeyModifier(key))
    {
        key = m_lastKey;
    }

    QString text;

    // only modifiers -> incomplete shortcut
    if (key < 1)
    {
        // modifers to string
        if (modifiers & Qt::ControlModifier)
        {
            text += QStringLiteral("Ctrl+");
        }
        if (modifiers & Qt::ShiftModifier)
        {
            text += QStringLiteral("Shift+");
        }
        if (modifiers & Qt::MetaModifier)
        {
            text += QStringLiteral("Meta+");
        }
        if (modifiers & Qt::AltModifier)
        {
            text += QStringLiteral("Alt+");
        }
        if (modifiers & Qt::KeypadModifier)
        {
            text += QStringLiteral("Num+");
        }

        if (!text.isEmpty())
        {
            this->setText(text + QStringLiteral("..."));
        }
        return;
    }

    // only a key and no modifiers -> incomplete shortcut
    if (!modifiers)
    {
        text = QKeySequence(key).toString();

        // dont allow simple keys (eg. chars and numbers)
        if (text.length() == 1)
        {
            this->setText(QStringLiteral("...") + text);
            return;
        }
    }

    // valid key combination
    m_lastKey = key;
    this->setText(QKeySequence(key | modifiers).toString());
}

void
GtShortCutEdit::keyReleaseEvent(QKeyEvent* event)
{
    event->accept();

    // clear key buffer
    if (m_lastKey == event->key())
    {
        m_lastKey = 0;
    }
}

void
GtShortCutEdit::onFocusIn()
{
    // clear visuals and key code buffer
    GtLineEdit::clear();
    m_lastKey = 0;
    setPlaceholderText(tr("<waiting for input>"));
}

void
GtShortCutEdit::onFocusOut()
{
    setPlaceholderText(QStringLiteral(""));

    // incomplete keysequence (only modifiers)
    if (this->text().contains(QStringLiteral("...")))
    {
        //restore old keysequence
        setKeySequence(m_keySequence);
        return;
    }

    setKeySequence(this->text());
}

void
GtShortCutEdit::onClearFocusOut()
{
    setPlaceholderText(QStringLiteral(""));
    //restore old keysequence
    setKeySequence(m_keySequence);
}

bool
GtShortCutEdit::isKeyModifier(int key)
{
    switch (key)
    {
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
        return true;
    default:
        return false;
    }
}
