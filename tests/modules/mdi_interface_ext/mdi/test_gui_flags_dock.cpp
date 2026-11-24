/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_flags_dock.h"

#include "test_gui_item_multiple.h"
#include "test_gui_item_reopen.h"
#include "test_gui_item_bound.h"
#include "test_gui_item_customid.h"
#include "test_gui_item_nodata.h"

#include "gt_object.h"
#include "gt_mdiitem.h"
#include "gt_guilauncher.h"
#include "gt_logging.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
QString
nameOf(GtObject* obj)
{
    return obj ? obj->objectName() : QStringLiteral("<none>");
}
} // namespace

TestGuiFlagsDock::TestGuiFlagsDock()
{
    setObjectName(tr("GUI Item Flags"));

    auto* content = new QWidget{this};
    m_layout = new QVBoxLayout{content};
    m_layout->setSpacing(6);

    addButton(tr("Close all GUI Items"), &TestGuiFlagsDock::closeAll);

    addSection(tr("Multi Item Viewer - allows multiple instances"));
    addButton(tr("Open A"), &TestGuiFlagsDock::openMultipleA);
    addButton(tr("Open B"), &TestGuiFlagsDock::openMultipleB);

    addSection(tr("Reopen Viewer - shared, reopens with new object"));
    addButton(tr("(Re)Open A"), &TestGuiFlagsDock::openReopenA);
    addButton(tr("(Re)Open B"), &TestGuiFlagsDock::openReopenB);
    m_reopenStatus = addStatusLabel();

    addSection(tr("Shared Viewer - shared, no reopening"));
    addButton(tr("Open A"), &TestGuiFlagsDock::openSharedA);
    addButton(tr("Open B"), &TestGuiFlagsDock::openSharedB);
    m_sharedStatus = addStatusLabel();

    addSection(tr("Bound Viewer - bound to the lifetime of Object A"));
    addButton(tr("Open A"), &TestGuiFlagsDock::openBoundA);
    addButton(tr("Open without data (expect refused)"),
              &TestGuiFlagsDock::openBoundWithoutData);
    addButton(tr("Delete Object A"), &TestGuiFlagsDock::deleteObjectA);
    m_boundStatus = addStatusLabel();

    addSection(tr("Free Viewer - opens without any object"));
    addButton(tr("Open Viewer"), &TestGuiFlagsDock::openFreeViewer);

    m_layout->addStretch();

    auto* scroll = new QScrollArea{this};
    scroll->setWidgetResizable(true);
    scroll->setWidget(content);
    scroll->setFrameShape(QFrame::NoFrame);
    setWidget(scroll);
}

TestGuiFlagsDock::~TestGuiFlagsDock()
{
    delete m_objA;
    delete m_objB;
}

Qt::DockWidgetArea
TestGuiFlagsDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}

GtObject*
TestGuiFlagsDock::objectA()
{
    if (!m_objA)
    {
        m_objA = new GtObject{};
        m_objA->setObjectName(QStringLiteral("Object A"));
    }
    return m_objA;
}

GtObject*
TestGuiFlagsDock::objectB()
{
    if (!m_objB)
    {
        m_objB = new GtObject{};
        m_objB->setObjectName(QStringLiteral("Object B"));
    }
    return m_objB;
}

void
TestGuiFlagsDock::addSection(const QString& title)
{
    m_layout->addSpacing(8);

    auto* label = new QLabel{title};
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);
    label->setWordWrap(true);
    m_layout->addWidget(label);
}

void
TestGuiFlagsDock::addButton(const QString& text,
                            void (TestGuiFlagsDock::*slot)())
{
    auto* button = new QPushButton{text};
    connect(button, &QPushButton::clicked, this, slot);
    m_layout->addWidget(button);
}

QLabel*
TestGuiFlagsDock::addStatusLabel()
{
    auto* label = new QLabel{tr("(no action yet)")};
    label->setWordWrap(true);
    label->setStyleSheet(QStringLiteral("color: gray;"));
    m_layout->addWidget(label);
    return label;
}

void
TestGuiFlagsDock::reportOpen(QPointer<GtMdiItem>& tracked, QLabel* status,
                             const QString& buttonName, GtMdiItem* result,
                             GtObject* requested)
{
    if (!result)
    {
        status->setText(tr("%1: refused (no item opened)").arg(buttonName));
        tracked = nullptr;
        return;
    }

    const bool reused = (tracked != nullptr && result == tracked);
    const GtObject* shown = result->currentObject();
    const bool updated = (shown == requested);

    status->setText(
        tr("%1: %2, shows '%3' (%4)")
            .arg(buttonName, reused ? tr("reused tab") : tr("new tab"),
                 nameOf(const_cast<GtObject*>(shown)),
                 updated ? tr("label updated") : tr("label unchanged")));

    tracked = result;
}

void
TestGuiFlagsDock::openMultipleA()
{
    GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemMultiple),
                                   objectA());
}

void
TestGuiFlagsDock::openMultipleB()
{
    GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemMultiple),
                                   objectB());
}

void
TestGuiFlagsDock::openReopenA()
{
    auto* item = GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemReopen),
                                                objectA());
    reportOpen(m_reopenItem, m_reopenStatus, tr("(Re)Open A"), item, objectA());
}

void
TestGuiFlagsDock::openReopenB()
{
    auto* item = GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemReopen),
                                                objectB());
    reportOpen(m_reopenItem, m_reopenStatus, tr("(Re)Open B"), item, objectB());
}

void
TestGuiFlagsDock::openSharedA()
{
    auto* item = GtGuiLauncher::instance().open(
        GT_CLASSNAME(TestGuiItemCustomId), objectA());
    reportOpen(m_sharedItem, m_sharedStatus, tr("Open A"), item, objectA());
}

void
TestGuiFlagsDock::openSharedB()
{
    auto* item = GtGuiLauncher::instance().open(
        GT_CLASSNAME(TestGuiItemCustomId), objectB());
    reportOpen(m_sharedItem, m_sharedStatus, tr("Open B"), item, objectB());
}

void
TestGuiFlagsDock::openBoundA()
{
    auto* item = GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemBound),
                                                objectA());
    reportOpen(m_boundItem, m_boundStatus, tr("Open A"), item, objectA());
}

void
TestGuiFlagsDock::openBoundWithoutData()
{
    auto* item =
        GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemBound), nullptr);

    gtInfo() << tr("Bound without data -> ")
             << (item ? tr("opened (unexpected)") : tr("refused (expected)"));

    // do not touch m_boundItem here: a refusal does not affect an already open tab
    m_boundStatus->setText(item ? tr("Open without data: opened (unexpected)")
                                : tr("Open without data: refused (expected)"));
}

void
TestGuiFlagsDock::deleteObjectA()
{
    if (!m_objA)
    {
        m_boundStatus->setText(tr("Object A does not exist"));
        gtWarning() << tr(
            "Object A does not exist. Open the bound item first.");
        return;
    }

    QPointer<GtObject> doomed = m_objA;
    m_objA = nullptr;
    m_boundItem = nullptr;
    doomed->deleteLater();

    m_boundStatus->setText(tr("Object A deleted; bound tab should close"));
    gtInfo() << tr("Object A deleted. The bound tab should close.");
}

void
TestGuiFlagsDock::openFreeViewer()
{
    GtGuiLauncher::instance().open(GT_CLASSNAME(TestGuiItemNoData), nullptr);
}

void
TestGuiFlagsDock::closeAll()
{
    GtGuiLauncher::instance().closeAll();

    m_reopenItem = nullptr;
    m_sharedItem = nullptr;
    m_boundItem = nullptr;
    if (m_reopenStatus)
    {
        m_reopenStatus->setText(tr("(no action yet)"));
    }
    if (m_sharedStatus)
    {
        m_sharedStatus->setText(tr("(no action yet)"));
    }
    if (m_boundStatus)
    {
        m_boundStatus->setText(tr("(no action yet)"));
    }
}
