/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labeldelegate.h
 *
 *  Created on: 05.12.2017
 *  Author: jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTTEXTFILTERDELEGATE_H
#define GTTEXTFILTERDELEGATE_H

#include <QStyledItemDelegate>

#include "gt_gui_exports.h"

#include <QRegExp>

/**
 * @brief The GtTextFilterDelegate class - Delegate to use for the
 * dock-widgets to use a QRegExp as a filter to exclude umlaut and ß
 */
class GT_GUI_EXPORT GtTextFilterDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief The validatorflag enum
     */
    enum validatorflag
    {
        uiFilter = 0,
        allowSpaces
    };

    /**
     * @brief GtTextFilterDelegate - Constructor
     */
    explicit GtTextFilterDelegate(QObject *parent, validatorflag filter = uiFilter);

    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                          const QModelIndex&index) const override;

    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override;

private:
    validatorflag m_validatorflag;

    /**
     * @brief updateRegExpSetupByObject
     * The function checks if the used objectui provides
     * a definition for the regular expression used for the renaming
     * and additionally if a hint for the renaming is available and
     * if the renaming should be strict (permits invalid intermediate states)
     * or not.
     * These values are used to set the arguments given by reference
     * @param index which is asked for the renaming
     * @param regExp to overwrite if offered by the used ui element
     * @param hint to overwrite if offered by the used ui element
     * @param checkWhileEditing to overwrite if offered by the used ui element
     */
    void updateRegExpSetupByObject(const QModelIndex& index, QRegExp& regExp,
                                   QString& hint) const;
};

#endif // GTTEXTFILTERDELEGATE_H
