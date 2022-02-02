/* GTlab - Gas Turbine laboratory
 * Source File: gt_labeldelegate.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.12.2017
 *  Author: jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTTEXTFILTERDELEGATE_H
#define GTTEXTFILTERDELEGATE_H

#include <QStyledItemDelegate>

#include "gt_gui_exports.h"

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
    GtTextFilterDelegate(QObject* parent, validatorflag filter = uiFilter);

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

};

#endif // GTTEXTFILTERDELEGATE_H
