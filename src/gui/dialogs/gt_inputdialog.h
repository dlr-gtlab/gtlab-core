/* GTlab - Gas Turbine laboratory
 * Source File: gt_inputdialog.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 10.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_INPUTDIALOG_H
#define GT_INPUTDIALOG_H

#include "gt_gui_exports.h"

#include <QDialog>

class QLabel;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QValidator;

/**
 * @brief The GtInputDialog class
 */
class GT_GUI_EXPORT GtInputDialog : public QDialog
{
    Q_OBJECT

public:
    enum InputMode
    {
        TextInput,
        IntInput,
        DoubleInput
    };

    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    GtInputDialog(GtInputDialog::InputMode mode = TextInput,
                  QWidget* parent = Q_NULLPTR);

    /**
     * @brief Sets text of input dialog label.
     * @param str New label text.
     */
    void setLabelText(const QString& str);

    /**
     * @brief Returns text value for TextInput type dialog.
     * @return Text value.
     */
    QString textValue() const;

    /**
     * @brief Sets initial text value for TextInput mode.
     * @param str New text value.
     */
    void setInitialTextValue(const QString& str);

    /**
     * @brief Returns maximum value of DoubleInput type.
     * @return Maximum value.
     */
    double doubleMaximum() const;

    /**
     * @brief Sets maximum value of DoubleInput type.
     * @param max New maximum value.
     */
    void setDoubleMaximum(double max);

    /**
     * @brief Returns minimum value of DoubleInput type.
     * @return Minimum value.
     */
    double doubleMinimum() const;

    /**
     * @brief Sets minimum value of DoubleInput type.
     * @param min New minimum value.
     */
    void setDoubleMinimum(double min);

    /**
     * @brief Returns maximum value of IntInput type.
     * @return Maximum value.
     */
    double intMaximum() const;

    /**
     * @brief Sets maximum value of IntInput type.
     * @param max New maximum value.
     */
    void setIntMaximum(double max);

    /**
     * @brief Returns minimum value of IntInput type.
     * @return Minimum value.
     */
    double intMinimum() const;

    /**
     * @brief Sets minimum value of IntInput type.
     * @param min New minimum value.
     */
    void setIntMinimum(double min);

    /**
     * @brief Returns true if empty strings are allowed in TextInput mode.
     * @return Whether empty strings are allowed.
     */
    bool emptyStringAllowed() const;

    /**
     * @brief Sets empty strings allowed property.
     * @param emptyStringAllowed empty string allowed property.
     */
    void setEmptyStringAllowed(bool emptyStringAllowed);

    /**
     * @brief Sets validator for TextInput mode.
     * @param validator New validsator.
     */
    void setTextValidator(QValidator* validator);

    /**
     * @brief Returns black list of TextInput mode.
     * @return
     */
    QStringList textBlackList() const;

    /**
     * @brief setTextBlackList
     * @param textBlackList
     */
    void setTextBlackList(const QStringList& textBlackList);

protected:
    /**
     * @brief showEvent
     * @param event
     */
    virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private:
    /// Label text.
    QLabel* m_label;

    /// Input line for TextInput mode
    QLineEdit* m_lineEdit;

    /// Input line for IntInput mode
    QSpinBox* m_intEdit;

    /// Input line for DoubleInput mode
    QDoubleSpinBox* m_doubleEdit;

    /// Accept button.
    QPushButton* m_acceptBtn;

    /// Reject button.
    QPushButton* m_rejectBtn;

    /// Current input mode.
    InputMode m_currentMode;

    /// TextInput mode validator
    QValidator* m_textValidator;

    /// List of forbidden text for TextInput mode
    QStringList m_textBlackList;

    /// Maximum value of DoubleInput type.
    int m_doubleMax;

    /// Minimum value of DoubleInput type.
    int m_doubleMin;

    /// Maximum value of IntInput type.
    int m_intMax;

    /// Minimum value of IntInput type.
    int m_intMin;

    /// Whether empty strings are allowed in TextInput mode.
    bool m_emptyStringAllowed;

    /**
     * @brief Updates input field based on given settings.
     */
    void updateInputFields();

    /**
     * @brief Shows/Hides mode based input lines.
     * @param mode Given input mode.
     */
    void setMode(GtInputDialog::InputMode mode);

private slots:
    /**
     * @brief Validates text input and shows/hides accept button.
     */
    void validateTextInput();

};

#endif // GT_INPUTDIALOG_H
