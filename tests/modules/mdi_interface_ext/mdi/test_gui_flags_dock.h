/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIFLAGSDOCK_H
#define TESTGUIFLAGSDOCK_H

#include "gt_dockwidget.h"

#include <QPointer>

class GtObject;
class GtMdiItem;
class QLabel;
class QVBoxLayout;

/**
 * @brief Dock widget with buttons to manually exercise the `GtMdiItem` flags
 * (AllowsMultipleInstances, AllowsReopening, BoundToObject), customId and
 * items that do not operate on a data object.
 *
 * Drives `GtGuiLauncher` directly on dock-owned `GtObject`s, so no project or
 * data model is required.
 */
class TestGuiFlagsDock : public GtDockWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiFlagsDock();
    ~TestGuiFlagsDock() override;

    Qt::DockWidgetArea getDockWidgetArea() override;

private slots:
    void openMultipleA();
    void openMultipleB();

    void openReopenA();
    void openReopenB();

    void openSharedA();
    void openSharedB();

    void openBoundA();
    void openBoundWithoutData();
    void deleteObjectA();

    void openFreeViewer();

    void closeAll();

private:
    GtObject* objectA();
    GtObject* objectB();

    void addSection(const QString& title);
    void addButton(const QString& text, void (TestGuiFlagsDock::*slot)());
    QLabel* addStatusLabel();

    /// Reports "<button>: <new tab|reused tab>, shows '<name>' (<updated|unchanged>)"
    /// and tracks the returned item for the next call.
    void reportOpen(QPointer<GtMdiItem>& tracked, QLabel* status,
                    const QString& buttonName, GtMdiItem* result,
                    GtObject* requested);

    QVBoxLayout* m_layout{nullptr};
    QLabel* m_reopenStatus{nullptr};
    QLabel* m_sharedStatus{nullptr};
    QLabel* m_boundStatus{nullptr};

    QPointer<GtMdiItem> m_reopenItem;
    QPointer<GtMdiItem> m_sharedItem;
    QPointer<GtMdiItem> m_boundItem;

    QPointer<GtObject> m_objA{nullptr};
    QPointer<GtObject> m_objB{nullptr};
};

#endif // TESTGUIFLAGSDOCK_H
