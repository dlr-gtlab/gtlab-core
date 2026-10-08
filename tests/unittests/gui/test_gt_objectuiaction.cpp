/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_objectuiaction.h"

#include "gt_object.h"

#include <QKeySequence>
#include <QIcon>

TEST(GtObjectUIAction, default_constructor)
{
    GtObjectUIAction action;

    EXPECT_TRUE(action.isEmpty());
    EXPECT_TRUE(action.isSeparator());
    EXPECT_TRUE(action.name().isEmpty());
    EXPECT_TRUE(action.icon().isNull());
    EXPECT_EQ(action.orderPriority(), gt::gui::OrderPriority::Default);
}

TEST(GtObjectUIAction, copy_constructor)
{
    bool verificationCalled = false;
    bool visibilityCalled = false;

    GtObjectUIAction action{"myAction", [](GtObject*) { }};
    action.setOrderPriority(gt::gui::OrderPriority::ExportAction);
    action.setVerificationMethod([&verificationCalled](GtObject*) {
        verificationCalled = true;
        return true;
    });
    action.setVisibilityMethod([&visibilityCalled](GtObject*) {
        visibilityCalled = true;
        return true;
    });
    action.setShortCut(QKeySequence{Qt::Key_F1});

    GtObjectUIAction copy{action};

    EXPECT_FALSE(copy.isEmpty());
    EXPECT_FALSE(copy.isSeparator());
    EXPECT_EQ(copy.name(), action.name());
    EXPECT_EQ(copy.name(), "myAction");
    EXPECT_EQ(copy.orderPriority(), gt::gui::OrderPriority::ExportAction);
    EXPECT_EQ(copy.orderPriority(), action.orderPriority());
    EXPECT_EQ(copy.shortCut(), action.shortCut());
    EXPECT_EQ(copy.shortCut(), QKeySequence{Qt::Key_F1});
    EXPECT_TRUE(copy.method());

    EXPECT_FALSE(visibilityCalled);
    EXPECT_FALSE(verificationCalled);

    action.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    action.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);

    visibilityCalled = false;
    verificationCalled = false;

    copy.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    copy.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);
}

TEST(GtObjectUIAction, move_constructor)
{
    bool verificationCalled = false;
    bool visibilityCalled = false;

    GtObjectUIAction action{"myAction", [](GtObject*) { }};
    action.setOrderPriority(gt::gui::OrderPriority::DeleteAction);
    action.setVerificationMethod([&verificationCalled](GtObject*) {
        verificationCalled = true;
        return true;
    });
    action.setVisibilityMethod([&visibilityCalled](GtObject*) {
        visibilityCalled = true;
        return true;
    });
    action.setShortCut(QKeySequence{Qt::Key_F1});

    GtObjectUIAction moved{std::move(action)};

    EXPECT_FALSE(moved.isEmpty());
    EXPECT_EQ(moved.name(), "myAction");
    EXPECT_EQ(moved.orderPriority(), gt::gui::OrderPriority::DeleteAction);
    EXPECT_EQ(moved.shortCut(), QKeySequence{Qt::Key_F1});
    EXPECT_TRUE(moved.method());
    EXPECT_TRUE(moved.verificationMethod());
    EXPECT_TRUE(moved.visibilityMethod());

    EXPECT_TRUE(action.isEmpty());
    EXPECT_FALSE(action.method());
    EXPECT_FALSE(action.verificationMethod());
    EXPECT_FALSE(action.visibilityMethod());

    moved.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    moved.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);
}

TEST(GtObjectUIAction, copy_assignment)
{
    bool verificationCalled = false;
    bool visibilityCalled = false;

    GtObjectUIAction action{"myAction", [](GtObject*) { }};
    action.setOrderPriority(gt::gui::OrderPriority::ExportAction);
    action.setVerificationMethod([&verificationCalled](GtObject*) {
        verificationCalled = true;
        return true;
    });
    action.setVisibilityMethod([&visibilityCalled](GtObject*) {
        visibilityCalled = true;
        return true;
    });
    action.setShortCut(QKeySequence{Qt::Key_F1});

    GtObjectUIAction copy{"myAction2",
                          GtObjectUIAction::InvokableActionMethod(nullptr)};
    EXPECT_FALSE(copy.method());

    copy = action;

    EXPECT_FALSE(copy.isEmpty());
    EXPECT_FALSE(copy.isSeparator());
    EXPECT_EQ(copy.name(), action.name());
    EXPECT_EQ(copy.name(), "myAction");
    EXPECT_EQ(copy.orderPriority(), gt::gui::OrderPriority::ExportAction);
    EXPECT_EQ(copy.orderPriority(), action.orderPriority());
    EXPECT_EQ(copy.shortCut(), action.shortCut());
    EXPECT_EQ(copy.shortCut(), QKeySequence{Qt::Key_F1});
    EXPECT_TRUE(copy.method());

    EXPECT_FALSE(visibilityCalled);
    EXPECT_FALSE(verificationCalled);

    action.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    action.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);

    visibilityCalled = false;
    verificationCalled = false;

    copy.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    copy.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);
}

TEST(GtObjectUIAction, move_assignment)
{
    bool verificationCalled = false;
    bool visibilityCalled = false;

    GtObjectUIAction action{"myAction", [](GtObject*) { }};
    action.setOrderPriority(gt::gui::OrderPriority::DeleteAction);
    action.setVerificationMethod([&verificationCalled](GtObject*) {
        verificationCalled = true;
        return true;
    });
    action.setVisibilityMethod([&visibilityCalled](GtObject*) {
        visibilityCalled = true;
        return true;
    });
    action.setShortCut(QKeySequence{Qt::Key_F1});

    GtObjectUIAction moved{"myAction2",
                           GtObjectUIAction::InvokableActionMethod(nullptr)};
    EXPECT_FALSE(moved.method());

    moved = std::move(action);

    EXPECT_FALSE(moved.isEmpty());
    EXPECT_EQ(moved.name(), "myAction");
    EXPECT_EQ(moved.orderPriority(), gt::gui::OrderPriority::DeleteAction);
    EXPECT_EQ(moved.shortCut(), QKeySequence{Qt::Key_F1});
    EXPECT_TRUE(moved.method());
    EXPECT_TRUE(moved.verificationMethod());
    EXPECT_TRUE(moved.visibilityMethod());

    EXPECT_TRUE(action.isEmpty());
    EXPECT_FALSE(action.method());
    EXPECT_FALSE(action.verificationMethod());
    EXPECT_FALSE(action.visibilityMethod());

    moved.visibilityMethod()(nullptr, nullptr);
    EXPECT_TRUE(visibilityCalled);
    moved.verificationMethod()(nullptr, nullptr);
    EXPECT_TRUE(verificationCalled);
}

TEST(GtObjectUIAction, method_invocation)
{
    bool methodCalled = false;
    GtObject* capturedTarget = nullptr;

    GtObjectUIAction action{"TestAction",
                            [&methodCalled, &capturedTarget](GtObject* target) {
                                methodCalled = true;
                                capturedTarget = target;
                            }};

    GtObject target;
    action.method()(nullptr, &target);

    EXPECT_TRUE(methodCalled);
    EXPECT_EQ(capturedTarget, &target);
}

TEST(GtObjectUIAction, verification_method)
{
    bool verificationResult = true;
    GtObject* capturedTarget = nullptr;

    GtObjectUIAction action;
    action.setVerificationMethod(
        [&verificationResult, &capturedTarget](GtObject* target) -> bool {
            capturedTarget = target;
            return verificationResult;
        });

    GtObject target;
    bool result = action.verificationMethod()(nullptr, &target);

    EXPECT_EQ(verificationResult, result);
    EXPECT_EQ(capturedTarget, &target);
}

TEST(GtObjectUIAction, visibility_method)
{
    bool visibilityResult = false;
    GtObject* capturedTarget = nullptr;

    GtObjectUIAction action;
    action.setVisibilityMethod(
        [&visibilityResult, &capturedTarget](GtObject* target) -> bool {
            capturedTarget = target;
            return visibilityResult;
        });

    GtObject target;
    bool result = action.visibilityMethod()(nullptr, &target);

    EXPECT_EQ(visibilityResult, result);
    EXPECT_EQ(capturedTarget, &target);
}
