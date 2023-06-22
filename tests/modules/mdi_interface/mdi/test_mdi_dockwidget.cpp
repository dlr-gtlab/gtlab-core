/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include "test_mdi_dockwidget.h"

#include <QLabel>

TestMdiDockWidget::TestMdiDockWidget()
{
    setObjectName(tr("Test Dock Widget"));

    m_testLabel = new QLabel(QStringLiteral("Test Label"));

    setWidget(m_testLabel);
}

Qt::DockWidgetArea
TestMdiDockWidget::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}
