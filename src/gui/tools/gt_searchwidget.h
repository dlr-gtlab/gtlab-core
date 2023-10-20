/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSEARCHWIDGET_H
#define GTSEARCHWIDGET_H

#include "gt_gui_exports.h"

#include <QWidget>

class GtLineEdit;
class QPushButton;
class QLabel;

/**
 * @brief The GtSearchWidget class
 */
class GT_GUI_EXPORT GtSearchWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtSearchWidget
     * @param parent
     */
    explicit GtSearchWidget(QWidget* parent = nullptr);

    /**
     * @brief text
     * @return
     */
    QString text();

    /**
     * @brief setText
     * @param text
     */
    void setText(const QString& text);

public slots:
    /**
     * @brief enableSearch
     */
    void enableSearch();

    /**
     * @brief disableSearch
     */
    void disableSearch();

    /**
     * @brief shortCutChangeReaction
     */
    void onShortCutChanged();

protected:
    /**
     * @brief Event filter to catch mouse press events of given object.
     * @param obj Object pointer.
     * @param event Current event of given object.
     * @return Returns event filter acceptance.
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    /// Search line
    GtLineEdit* m_searchLine;

    /// Search label
    QLabel* m_searchLabel;

    /// Search button
    QPushButton* m_searchButton;

    /// Clear search button
    QPushButton* m_clearButton;

signals:
    /**
     * @brief textEdited
     */
    void textEdited(const QString&);

    /**
     * @brief textChanged
     */
    void textChanged(const QString&);

    /**
     * @brief returnPressed
     */
    void returnPressed();

    /**
     * @brief searchEnabled
     */
    void searchEnabled();

    /**
     * @brief searchDisabled
     */
    void searchDisabled();

};

#endif // GTSEARCHWIDGET_H
