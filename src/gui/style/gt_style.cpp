/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 15.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_style.h"
#include "gt_colors.h"
#include "gt_icons.h"
#include "gt_application.h"
#include "gt_palette.h"

#include <QStyleFactory>
#include <QWizard>
#include <QPen>
#include <QPainter>
#include <QStyleOption>

#include <mutex>

using namespace gt::gui;

GtStyle::GtStyle() :
    QProxyStyle(QStyleFactory::create(
        gtApp->inDarkMode() ?
            QStringLiteral("Fusion") :
#ifndef Q_OS_WIN
            QStringLiteral("Default")
#else
            QStringLiteral("windowsvista")
#endif
        ))
{ }

QPalette
GtStyle::standardPalette() const
{
    return currentTheme();
}

int
GtStyle::styleHint(StyleHint hint,
                   QStyleOption const* option,
                   QWidget const* widget,
                   QStyleHintReturn* returnData) const
{
    switch(hint)
    {
    // removes blurry disabled text
    case QStyle::SH_EtchDisabledText:
    case QStyle::SH_DitherDisabledText:
        return false;
    // better movement in menu using keyboard?
    case QStyle::SH_Menu_KeyboardSearch:
        return true;
    // default wizard style
    case QStyle::SH_WizardStyle:
        return QWizard::ModernStyle;
    // grid color
    case QStyle::SH_Table_GridLineColor:
        return color::lightFrame().rgba();
    }

    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void
GtStyle::drawPrimitive(PrimitiveElement element,
                       QStyleOption const* option,
                       QPainter* painter,
                       QWidget const* widget) const
{
    switch(element)
    {
    // general apeearance of frames
    case QStyle::PE_Frame:
    case QStyle::PE_FrameDockWidget:
    case QStyle::PE_FrameGroupBox:
    case QStyle::PE_FrameLineEdit:
    case QStyle::PE_FrameWindow:
    case QStyle::PE_FrameMenu:
    case QStyle::PE_FrameStatusBarItem:
    case QStyle::PE_FrameButtonBevel:
    case QStyle::PE_FrameButtonTool:
    {
        auto* opt = qstyleoption_cast<QStyleOptionFrame const*>(option);
        if (opt && opt->frameShape == QFrame::NoFrame) break;

        QPen pen(color::lightFrame());
        painter->setPen(pen);
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
        return;
    }

    case QStyle::PE_FrameTabBarBase:
    {
        // reduce width of tabbar frame
        QRect rect = option->rect;
        if (qobject_cast<QTabBar const*>(widget) && rect.height() > 1)
        {
            rect.setHeight(1);
        }

        // NOTE: ignoring tab shape
        QPen pen(color::lightFrame());
        painter->setPen(pen);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        return;
    }



    // overrides frame of checkbox
    case QStyle::PE_IndicatorCheckBox:
    {
        // draw checkbox using base style
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        // override frame of checkbox
        QPen pen(color::frame());
        painter->setPen(pen);
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
        return;
    }

    }

    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void
GtStyle::drawControl(ControlElement element,
                     QStyleOption const* option,
                     QPainter* painter,
                     QWidget const* widget) const
{
    return QProxyStyle::drawControl(element, option, painter, widget);
}

QIcon
GtStyle::standardIcon(StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const
{
    switch(standardIcon)
    {
    case QStyle::SP_TitleBarMinButton:
        return icon::minimize();
    case QStyle::SP_TitleBarMaxButton:
        return icon::dock();
    case QStyle::SP_TitleBarCloseButton:
        return icon::close();
    case QStyle::SP_TitleBarContextHelpButton:
        return icon::help();

    case QStyle::SP_DockWidgetCloseButton:
        return icon::close();

    case QStyle::SP_DialogOkButton:
        return icon::check();
    case QStyle::SP_DialogAbortButton:
    case QStyle::SP_DialogCancelButton:
        return icon::cancel();
    case QStyle::SP_DialogHelpButton:
        return icon::help();
    case QStyle::SP_DialogOpenButton:
        return icon::folderOpen();
    case QStyle::SP_DialogSaveButton:
    case QStyle::SP_DialogSaveAllButton:
        return icon::save();
    case QStyle::SP_DialogCloseButton:
        return icon::close();
    case QStyle::SP_DialogApplyButton:
        return icon::check();
    case QStyle::SP_DialogResetButton:
        return icon::revert();
    case QStyle::SP_DialogDiscardButton:
        return icon::delete_();
    case QStyle::SP_DialogYesButton:
    case QStyle::SP_DialogYesToAllButton:
        return icon::check();
    case QStyle::SP_DialogNoButton:
    case QStyle::SP_DialogNoToAllButton:
        return icon::cross();
    case QStyle::SP_DialogRetryButton:
        return icon::sync();
    case QStyle::SP_DialogIgnoreButton:
        return icon::cross();

    case QStyle::SP_LineEditClearButton:
        return icon::clear();

    case QStyle::SP_RestoreDefaultsButton:
        return icon::revert();
    }

    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

namespace
{

/**
 * This manager can hold all the different styles
 *
 * it will create and store styles on the fly
 */
class StyleManager
{
public:
    static StyleManager& instance()
    {
        static StyleManager inst;
        return inst;
    }

    /// Returns the current style
    GtStyle& currentStyle() const
    {
        const auto currentStyleMode = gtApp->inDarkMode();
        const auto styleIter = map.find(currentStyleMode);

        // we need to create new styles on the fly, as GtStyle cannot be set to a specific style
        if (styleIter == std::end(map))
        {
            std::lock_guard<std::mutex> _(mapMutex);
            map.emplace(currentStyleMode, std::make_unique<GtStyle>());
        }

        return *map[currentStyleMode];
    }

private:
    StyleManager() = default;

    mutable std::map<bool, std::unique_ptr<GtStyle>> map;
    mutable std::mutex mapMutex;
};

} // namespace

GtStyle &GtStyle::current()
{
    return StyleManager::instance().currentStyle();
}
