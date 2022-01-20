/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfilechooseritem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYFILECHOOSERITEM_H
#define GTPROPERTYFILECHOOSERITEM_H

#include "gt_propertyitem.h"

class GtFileChooserProperty;

/**
 * @brief The GtPropertyFileChooserItem class
 */
class GtPropertyFileChooserItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param scope
     * @param property
     * @param model
     * @param parent
     */
    Q_INVOKABLE GtPropertyFileChooserItem();

    /**
     * @brief fileChooserProperty
     * @return
     */
    GtFileChooserProperty* fileChooserProperty() const;

    /**
     * @brief editorWidget
     * @return
     */
    virtual QWidget* editorWidget(
            QWidget* parent,
            const GtPropertyValueDelegate* delegate) const Q_DECL_OVERRIDE;

    /**
     * @brief setEditorData
     * @param var
     */
    virtual void setEditorData(QWidget* editor,
                               QVariant& var) const Q_DECL_OVERRIDE;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     */
    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
    /// File chooser property.
    GtFileChooserProperty* m_fileChooserProperty;

};

#endif // GTPROPERTYFILECHOOSERITEM_H
