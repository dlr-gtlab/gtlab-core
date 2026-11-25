/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_COLLECTIONSDIRECTORIESTAB_H
#define GT_COLLECTIONSDIRECTORIESTAB_H

#include <QWidget>

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
     * @brief Get the full collections directory shown in the form.
     */
    QString collectionsPath() const;

    /**
     * @brief Set the collection path in form
     * @param dir
     * @param defaultPath
     */
    void setCollectionsPath(const QString& dir, const QString& defaultPath);

    /**
     * @brief Set an error message for the collections path
     * @param msg
     */
    void setCollectionPathsError(const QString& msg);

private:
    void updatePathToolTip();

    //! Auto-generated UI.
    Ui::GtCollectionsDirectoriesTab* ui;

    QString m_defaultPath;
    QString m_pathError;

private slots:
    /**
     * @brief Open the system file manager for the collection path
     */
    void onCollectionsDirShowInExplorer();

    /**
     * @brief Reset to GTlab default
     */
    void onCollectionsDirResetDefault();

    /**
     * @brief Button to select collection path is pressed
     */
    void onBtnCollectionsDirSelectPressed();

    /**
     * @brief Line edit for path of collection changed
     */
    void onEditCollectionsDirPathEdited();
};

#endif // GT_COLLECTIONSDIRECTORIESTAB_H
