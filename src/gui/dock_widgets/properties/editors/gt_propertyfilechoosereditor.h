/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfilechoosereditor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYFILECHOOSEREDITOR_H
#define GTPROPERTYFILECHOOSEREDITOR_H

#include "gt_gui_exports.h"

#include <QWidget>
#include <QPointer>

class QLineEdit;
class QPushButton;
class GtFileChooserProperty;

/**
 * @brief The GtPropertyFileChooserEditor class
 */
class GT_GUI_EXPORT GtPropertyFileChooserEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    explicit GtPropertyFileChooserEditor(QWidget* parent = nullptr);

    /**
     * @brief Sets pointer to corresponding file chooser property.
     * @param File chooser property.
     */
    void setFileChooserProperty(GtFileChooserProperty* prop);

private:
    /// Line edit for file path string
    QLineEdit* m_filePath;

    /// Restore button
    QPushButton* m_restoreButton;

    /// Select file/folder button
    QPushButton* m_selectButton;

    /// File chooser property
    QPointer<GtFileChooserProperty> m_prop;

    /**
     * @brief Updates line edit text.
     */
    void updateText();

    /**
     * @brief Sets property value considering undo/redo functionality.
     * @param val New property value.
     */
    void setPropertyValue(const QString& val);

private slots:
    /**
     * @brief selectFilePath
     */
    void selectFilePath();

    /**
     * @brief deleteFilePath
     */
    void deleteFilePath();

    /**
     * @brief Called by property change signal to update editor text.
     */
    void propertyValueChanged();

signals:
    void fileSelected(const QString& str);

};

#endif // GTPROPERTYFILECHOOSEREDITOR_H
