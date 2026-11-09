/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_COLLECTIONSDIRECTORIESTAB_H
#define GT_COLLECTIONSDIRECTORIESTAB_H

#include <QWidget>
#include <QStringList>

namespace Ui
{
class GtCollectionsDirectoriesTab;
}

class GtCollectionsDirectoriesTab : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Construct the widget.
     * @param parent Optional parent QWidget.
     */
    explicit GtCollectionsDirectoriesTab(QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~GtCollectionsDirectoriesTab() override;

    QString collectionsPath();
    void setCollectionsPath(const QString& dir);


private:
    //! Auto-generated UI.
    Ui::GtCollectionsDirectoriesTab* ui;

private slots:
    /**
     * @brief Open the system file manager for the colletion path
     */
    void onShowInExplorer();
};

#endif // GT_COLLECTIONSDIRECTORIESTAB_H
