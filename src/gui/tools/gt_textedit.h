/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTTEXTEDIT_H
#define GTTEXTEDIT_H

#include "gt_gui_exports.h"

#include <QTextEdit>

/**
 * @brief The GtTextEdit class
 *
 * The GtTextEdit class is a GTlab version of the QTextEdit.
 *
 * Differences are the intial definition of a type of the content
 * to set as a rendering hint (e.g. MarkDown or HTML).
 *
 * Additonally you can give the widget a long version of its content which is
 * shown after a double click on the widget
 */
class GT_GUI_EXPORT GtTextEdit : public QTextEdit
{
    Q_OBJECT
public:

    enum contentType{
        NONE = 0,
        TEXT = 1,
        MD = 2,
        HTML = 4,
        ELSE = 8
    };

    /**
     * @brief GtTextEdit - Constructor to set the member data
     * @param text
     * @param type
     * @param parent
     */
    explicit GtTextEdit(QString text,
                        contentType type = TEXT,
                        QWidget* parent = nullptr);
    /**
     * @brief setLongText
     * Sets the long text and changes tool tip to give hint to double click
     * @param text to display after double click
     * @return a reference to the object itself
     */
    GtTextEdit& setLongText(QString const& text);

    /**
     * @brief mouseDoubleClickEvent
     * event to handle a double click on the widget.
     * The widget will show the long text member data if it is set,
     * else nothing happens
     * @param event (unused)
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    /**
     * @brief typeFromEnding
     * @param ending
     * @return content type based on file ending
     */
    static contentType typeFromFile(QString const& file);

public slots:
    /**
     * @brief updateText
     * @param text to use
     * Updates the current text to the one given by the function.
     */
    void updateText(QString const& text);

private:
    QString m_text;

    QString m_long;

    contentType m_type;
};

#endif // GTTEXTEDIT_H
