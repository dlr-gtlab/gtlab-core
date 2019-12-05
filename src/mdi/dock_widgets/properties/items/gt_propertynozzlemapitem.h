#ifndef GTPROPERTYNOZZLEMAPITEM_H
#define GTPROPERTYNOZZLEMAPITEM_H

#include "gt_propertyitem.h"

class GtNozzleMapProperty;

/**
 * @brief The GtPropertyNozzleMapItem class
 */
class GtPropertyNozzleMapItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtPropertyNozzleMapItem();

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const Q_DECL_OVERRIDE;

    /**
     * @brief setData
     * @param column
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(int column,
                         const QVariant& value,
                         GtObject* obj,
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

    /**
     * @brief modeProperty
     * @return
     */
    GtNozzleMapProperty* dataProperty() const;

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

};

#endif // GTPROPERTYNOZZLEMAPITEM_H
