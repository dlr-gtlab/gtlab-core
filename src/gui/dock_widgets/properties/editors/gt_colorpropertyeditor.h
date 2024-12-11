/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_colorpropertyeditor.h
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTCOLORPROPERTYEDITOR_H
#define GTCOLORPROPERTYEDITOR_H

#include "gt_gui_exports.h"

#include <QWidget>
#include <QPointer>

class QLineEdit;
class QPushButton;
class GtColorProperty;

/**
 * @brief The GtColorPropertyEditor class
 */
class GT_GUI_EXPORT GtColorPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    explicit GtColorPropertyEditor(QWidget* parent = nullptr);

    /**
     * @brief Sets pointer to corresponding color property.
     * @param Color property.
     */
    void setColorProperty(GtColorProperty* prop);

private:
    /// Line edit for color string
    QLineEdit* m_colorLineEdit;

    /// Select color button
    QPushButton* m_selectButton;

    /// Color property
    QPointer<GtColorProperty> m_prop;

    /**
     * @brief Updates line edit text and color button.
     */
    void update();

    /**
     * @brief Sets property value considering undo/redo functionality.
     * @param val New property value.
     */
    void setPropertyValue(const QColor &val);

private slots:
    /**
     * @brief selectColor
     */
    void selectColor();

    /**
     * @brief Called by property change signal to update editor text.
     */
    void propertyValueChanged();

signals:
    void colorSelected(const QColor& c);

};

#endif // GTCOLORPROPERTYEDITOR_H
