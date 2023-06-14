/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyobjectlinkeditor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYOBJECTLINKEDITOR_H
#define GTPROPERTYOBJECTLINKEDITOR_H

#include "gt_gui_exports.h"

#include <QWidget>
#include <QPointer>

class QLineEdit;
class GtObjectLinkProperty;
class GtObject;

/**
 * @brief The GtPropertyObjectLinkEditor class
 */
class GT_GUI_EXPORT GtPropertyObjectLinkEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GtPropertyObjectLinkEditor(QWidget* parent = nullptr);

    /**
     * @brief setObjectLinkProperty
     * @param prop
     */
    void setObjectLinkProperty(GtObjectLinkProperty* prop);

    /**
     * @brief setScope
     * @param project
     */
    void setScope(GtObject* scope);

    /**
     * @brief validObjectSelected
     * @return
     */
    bool validObjectSelected();

    /**
     * @brief selectedObject
     * @return
     */
    GtObject* selectedObject();

    /**
     * @brief updateText
     */
    void updateText();

    /**
     * @brief dropEvent
     * overrides function of QWidget to handle a drop event with the event 
     * If the mime data of the event contain information of a valid
     * object which is a valid object for the objectlinkproperty
     * the uuid of the object is set as the new value of the object link
     */
    void dropEvent(QDropEvent* event) override;

    /**
     * @brief dragEnterEvent
     * overrides function of QWidget to handle a drag enter event with the event 
     * If the mime data of the event contain information of a valid
     * object which is a valid object for the objectlinkproperty
     * the widget accepts the drag enter into the window
     */
    void dragEnterEvent(QDragEnterEvent* e) override;
private:
    ///
    QLineEdit* m_objectId;

    ///
    QPointer<GtObjectLinkProperty> m_prop;

    /// Scope of data representation.
    QPointer<GtObject> m_scope;

    /**
     * @brief allowedObjects
     * @return
     */
    QList<GtObject*> allowedObjects(GtObject* obj) const;

    /**
     * @brief Sets property value considering undo/redo functionality.
     * @param val New property value.
     */
    void setPropertyValue(const QString& val);

private slots:
    /**
     * @brief selectObjectLink
     */
    void selectObjectLink();

    /**
     * @brief deleteObjectLink
     */
    void deleteObjectLink();

    /**
     * @brief findObject
     * find the object of the objectlink property in the explorer dock widget
     */
    void findObject();

    /**
     * @brief Called by property change signal to update editor text.
     */
    void propertyValueChanged();

signals:
    /**
     * @brief objectSelected
     */
    void objectSelected();

};

#endif // GTPROPERTYOBJECTLINKEDITOR_H
