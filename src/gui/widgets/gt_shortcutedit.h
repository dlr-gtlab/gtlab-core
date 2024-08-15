/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_shortcutedit.h
 *
 *  Created on: 09.12.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTSHORTCUTEDIT_H
#define GTSHORTCUTEDIT_H

#include "gt_gui_exports.h"

#include "gt_lineedit.h"
#include <QKeySequence>
#include <QTableWidgetItem>

/**
 * @brief The GtShortCutEdit class
 * Used to input a keysequence for a short cut (single key + multiple modifers).
 */
class GT_GUI_EXPORT GtShortCutEdit : public GtLineEdit, public QTableWidgetItem
{
    Q_OBJECT

    Q_PROPERTY(QKeySequence keySequence READ keySequence WRITE setKeySequence)

public:

    /**
     * @brief operator < for sorting GtLineEdits
     * @param other
     * @return
     */
    bool operator <(const QTableWidgetItem& other) const
    {
        if (other.column() == 1)
        {
            const GtLineEdit *p = dynamic_cast<const GtLineEdit *>(&other);

            if (p)
            {
                if (this->GtLineEdit::text() < p->text()) return true;
            }
        }

        return false;
    }

    /**
     * @brief GtShortCutEdit constructor
     * @param shortcut the initial shortcut
     * @param shortcutID sets the object name
     * @param parent parent widget
     */
    explicit GtShortCutEdit(const QKeySequence& shortcut,
                            const QString& shortcutID,
                            QWidget* parent = nullptr);

    /**
     * @brief keySequence getter for the keysequence entered.
     * @return keysequence entered
     */
    QKeySequence keySequence() const;

    /**
     * @brief setKeySequence setter for the keysequence.
     * @param seq keysequence
     */
    void setKeySequence(const QKeySequence& seq);

public slots:

    /**
     * @brief clear clears the visuals and the underlying keysequence.
     */
    void clear();

protected:

    /**
     * @brief keyPressEvent overloaded to retrieve the pressed keys. Accepts
     * every event.
     * @param event keyevent
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief keyReleaseEvent overloaded, accepts every event.
     * @param event
     */
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:

    /**
     * @brief onFocusIn clears the lineedit visuals.
     */
    void onFocusIn();

    /**
     * @brief onFocusOut sets the keysequence if valid.
     */
    void onFocusOut();

    /**
     * @brief onClearFocusOut restores the old keysequence if possible.
     */
    void onClearFocusOut();

private:

    /// keyseqeuence entered
    QKeySequence m_keySequence;
    /// buffer used to store the last key that was entered
    int m_lastKey;

    /**
     * @brief isKeyModifier checks if key is a modifer for a short cut
     * (eg. Alt, Ctrl, Shift, Meta)
     * @param key Qt key code
     * @return true if key is modifier
     */
    static bool isKeyModifier(int key);
};

#endif // GTSHORTCUTEDIT_H
