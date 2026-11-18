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

    /**
     * @brief Get the filled value of collections path in form
     * @return
     */
    const QString collectionsPath();

    /**
     * @brief Set the collection path in form
     * @param dir
     */
    void setCollectionsPath(const QString& dir);

    /**
     * @brief Set an error message for the colletions path
     * @param msg
     */
    void setCollectionPathsError(const QString& msg);


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
