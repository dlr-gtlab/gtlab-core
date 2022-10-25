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

#include "gt_dialog.h"

class GtCheckableStringListModel;

/**
 * @brief The GtRefusedPluginsDialog class
 */
class GtRefusedPluginsDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    explicit GtRefusedPluginsDialog(QWidget* parent = nullptr);

    /**
     * @brief Writes any unsaved changes to permanent storage.
     */
    void sync() const;

private:
    /// Model including list of all refused plugins.
    GtCheckableStringListModel* m_model;

};

#endif // GT_REFUSEDPLUGINSDIALOG_H
