/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2012 by DLR
 *
 *  Created on: 16.09.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTOOLBARHANDLER_H
#define GTTOOLBARHANDLER_H

#include <QObject>
#include <QPointer>

#include "gt_object.h"
#include "gt_qmlaction.h"

#include <QQuickWidget>

#include "gt_qmlobjectlistmodel.h"

/**
 * @brief Class for controlling the QML Toolbar.
 */
class GtQmlToolbar: public QQuickWidget
{
    Q_OBJECT

public:
    explicit GtQmlToolbar(class GtMainWin* parent = nullptr);

public slots:
    /**
     * @brief Called by clicking a qml button
     * @param btnId button identification string
     */
    void buttonClicked(const QString& btnId);

    /**
     * @brief Called on object selection change
     * @param obj selected object. nullptr if no object is selected
     */
    void onObjectSelected(GtObject* obj);

    /**
     * @brief Returns true if current project has an information
     * file (readme.md) inside the project directory
     * @return true if current project has an information file. otherwise
     * false is returned
     */
    bool projectHasInfo();

    /**
     * @brief Adds an custom button to the right side of the toolbar
     */
    Q_INVOKABLE class GtQmlAction *addCustomButton(const QString& text, const QUrl& iconUrl);

private:

    /// Pointer to selected object
    QPointer<GtObject> m_selectedObj;
    QPointer<GtQmlObjectListModel> m_customActions;

signals:
    /**
     * @brief Emitted after new project button was clicked.
     */
    void newProjectButtonClicked();

    /**
     * @brief Emitted after save project button was clicked.
     */
    void saveProjectButtonClicked();

    /**
     * @brief Emitted after open project button was clicked.
     */
    void openProjectButtonClicked();

    /**
     * @brief Emitted after undo button was clicked.
     */
    void undoButtonClicked();

    /**
     * @brief Emitted after redo button was clicked.
     */
    void redoButtonClicked();

};

#endif // GTTOOLBARHANDLER_H
