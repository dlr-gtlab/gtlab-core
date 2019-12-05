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

#include "gt_mdi_exports.h"

#include <QWidget>
#include <QPointer>

class QLineEdit;
class GtObjectLinkProperty;
class GtObject;

/**
 * @brief The GtPropertyObjectLinkEditor class
 */
class GT_MDI_EXPORT GtPropertyObjectLinkEditor : public QWidget
{
    Q_OBJECT

public:
    GtPropertyObjectLinkEditor(QWidget* parent = Q_NULLPTR);

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
    QList<GtObject*> allowedObjects(GtObject* obj);

private slots:
    /**
     * @brief selectObjectLink
     */
    void selectObjectLink();

    /**
     * @brief deleteObjectLink
     */
    void deleteObjectLink();

signals:
    /**
     * @brief objectSelected
     */
    void objectSelected();

};

#endif // GTPROPERTYOBJECTLINKEDITOR_H
