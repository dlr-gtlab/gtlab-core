/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_taskgrouprenamedialog.h
 *
 */

#ifndef GT_TASKGROUPRENAMEDIALOG_H
#define GT_TASKGROUPRENAMEDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class GtTaskGroupRenameDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param currentName - current task group name
     * @param existingNames - list of existing task group names (case-insensitive)
     * @param parent widget
     */
    GtTaskGroupRenameDialog(const QString& currentName,
                            const QStringList& existingNames,
                            QWidget* parent = nullptr);

    /**
     * @brief Returns the new name entered by user.
     */
    QString newName() const;

private slots:
    /**
     * @brief Validates the new name and updates OK button state.
     * @param text - the text to validate
     */
    void validateName(const QString& text);

private:
    QLineEdit* m_lineEdit;

    QLabel* m_warningLabel;

    QPushButton* m_okButton;

    QStringList m_existingNames;

    QString m_currentName;

    void setLabelColor(QColor newColor);
};

#endif // GT_TASKGROUPRENAMEDIALOG_H
