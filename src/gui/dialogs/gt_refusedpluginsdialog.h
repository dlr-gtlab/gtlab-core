/* GTlab - Gas Turbine laboratory
 * Source File: gt_refusedpluginsdialog.h
 * copyright 2009-2019 by DLR
 *
 *  Created on: 04.09.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_REFUSEDPLUGINSDIALOG_H
#define GT_REFUSEDPLUGINSDIALOG_H

#include "gt_gui_exports.h"

#include <QDialog>

class GtCheckableStringListModel;

/**
 * @brief The GtRefusedPluginsDialog class
 */
class GT_GUI_EXPORT GtRefusedPluginsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    GtRefusedPluginsDialog(QWidget* parent = Q_NULLPTR);

    /**
     * @brief Writes any unsaved changes to permanent storage.
     */
    void sync() const;

private:
    /// Model including list of all refused plugins.
    GtCheckableStringListModel* m_model;

};

#endif // GT_REFUSEDPLUGINSDIALOG_H
