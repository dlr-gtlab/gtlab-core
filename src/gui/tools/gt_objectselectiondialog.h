/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectselectiondialog.h
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTSELECTIONDIALOG_H
#define GTOBJECTSELECTIONDIALOG_H

#include "gt_gui_exports.h"

#include "gt_dialog.h"

#include <QPointer>

class GtObject;
class GtObjectFilterModel;
class GtObjectModel;
class GtStyledModel;
class GtTreeView;

/**
 * @brief The GtObjectSelectionDialog class
 */
class GT_GUI_EXPORT GtObjectSelectionDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtObjectSelectionDialog
     * @param root
     * @param parent
     */
    explicit GtObjectSelectionDialog(GtObject* root, QWidget* parent = nullptr);

    /**
     * @brief setFilterData
     * @param filter
     */
    void setFilterData(const QStringList& filter);

    /**
     * @brief currentObject
     * @return
     */
    GtObject* currentObject();

private:
    /// Root object.
    QPointer<GtObject> m_root;

    /// Filter model.
    GtObjectFilterModel* m_filterModel;

    /// Source model.
    GtObjectModel* m_srcModel;

    /// Styled model.
    GtStyledModel* m_styledModel;

    /// Tree view.
    GtTreeView* m_treeView;

private slots:
    /**
     * @brief onDoubleClicked
     * @param index
     */
    void onDoubleClicked(const QModelIndex& index);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);
};

#endif // GTOBJECTSELECTIONDIALOG_H
