/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 9.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTSVGICONENGINE_H
#define GTSVGICONENGINE_H

#include "gt_gui_exports.h"

#include <QIconEngine>

namespace gt
{
namespace gui
{

/// functor type for getting a color
using ColorFunctor = std::function<QColor()>;

/// function pointer type for getting a color
using ColorFunctionPtr = QColor(*)();

/**
 * @brief The SvgColorData struct. Simple data struct to set the colors of an
 * svg icon for its different modes
 */
struct SvgColorData
{
    GT_GUI_EXPORT SvgColorData();

    // cppcheck-suppress noExplicitConstructor
    GT_GUI_EXPORT SvgColorData(ColorFunctor _normal,
                               ColorFunctor _disabled = {},
                               ColorFunctor _selected = {});

    // cppcheck-suppress noExplicitConstructor
    GT_GUI_EXPORT SvgColorData(QColor const& _normal,
                               QColor const& _disabled = {},
                               QColor const& _selected = {});

    /// overload constructor for implicitly accepting a plain color function ptr
    // cppcheck-suppress noExplicitConstructor
    SvgColorData(ColorFunctionPtr colorFunction) :
        SvgColorData(ColorFunctor{colorFunction})
    { }

    /// overload constructor for implicitly accepting a Qt color enum value
    // cppcheck-suppress noExplicitConstructor
    SvgColorData(Qt::GlobalColor color) :
        SvgColorData(QColor{color})
    { }

    SvgColorData& setNormalColor(ColorFunctor color)
    {
        if (color) normal = std::move(color);
        return *this;
    }
    SvgColorData& setNormalColor(QColor const& color)
    {
        return setNormalColor([=](){ return color; });
    }

    SvgColorData& setDisabledColor(ColorFunctor color)
    {
        if (color) disabled = std::move(color);
        return *this;
    }
    SvgColorData& setDisabledColor(QColor const& color)
    {
        return setNormalColor([=](){ return color; });
    }

    SvgColorData& setSelectedColor(ColorFunctor color)
    {
        if (color) selected = std::move(color);
        return *this;
    }
    SvgColorData& setSelectedColor(QColor const& color)
    {
        return setNormalColor([=](){ return color; });
    }

    /**
    * @brief Returns the color for the icon mode
    * @param mode Icon mode
    * @return Color
    */
    QColor color(QIcon::Mode mode) const noexcept
    {
        assert(normal); assert(disabled); assert(selected);

        switch(mode)
        {
        case QIcon::Disabled:
            return disabled();
        case QIcon::Selected:
            return selected();
        case QIcon::Normal:
        default:
            return normal();
        }
    }

    ColorFunctor normal;
    ColorFunctor disabled;
    ColorFunctor selected;
};

} // namespace gui

} // namespace gt

/**
 * @brief The GtSvgIconEngine class. Can colorize a svg image.
 *
 * Source:
 *
 * https://stackoverflow.com/questions/43125339/
 *      creating-qicon-from-svg-contents-in-memory
 */
class GtSvgIconEngine : public QIconEngine
{
public:

    using ColorFunctor = gt::gui::ColorFunctor;
    using SvgColorData = gt::gui::SvgColorData;

    GtSvgIconEngine();

    explicit GtSvgIconEngine(const QString& path);

    explicit GtSvgIconEngine(const QString& path,
                             SvgColorData colorData);

    /**
     * @brief Clones this icon enigne
     * @return clone
     */
    QIconEngine* clone() const override { return new GtSvgIconEngine(*this); }

    // @TODO: Do we need to return the icon path?

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * @brief Returns the current icon name (path)
     * @return Icon path
     */
    QString iconName() const override { return m_file; }
#endif

    /**
     * @brief Returns the svg data
     * @return Svg data
     */
    QByteArray const& svg() const { return m_svg; }

    /**
     * @brief Updates the svg file
     * @param fileName file name
     * @param size Size fo the pixmap
     * @param mode Icon mode
     * @param state Icon state
     */
    void addFile(const QString& fileName,
                 const QSize& size,
                 QIcon::Mode mode,
                 QIcon::State state) override;

    /**
     * @brief Paints the svg icon
     * @param painter Painter to paint with
     * @param rect Rect specifying icon dimensions
     * @param mode Icon mode
     * @param state Icon state
     */
    void paint(QPainter* painter,
               const QRect& rect,
               QIcon::Mode mode,
               QIcon::State state) override;

    /**
     * @brief Returns a pixmap of the svg icon
     * @param size Size fo the pixmap
     * @param mode Icon mode
     * @param state Icon state
     * @return pixmap
     */
    QPixmap pixmap(const QSize& size,
                   QIcon::Mode mode,
                   QIcon::State state) override;

private:

    /// filename
    QString m_file;
    /// data for svg
    QByteArray m_svg;
    /// functors to define color of icon
    SvgColorData m_color;

    /// Index in svg where the color will be set.
    /// Used to overwrite color more efficiently
    int m_svgIdx = -1;

    /**
     * @brief Overrides the color of the svg data
     * @param color Color
     */
    void applyColor(const QColor& color);

    /**
     * @brief This icon engine does not support pixmaps
     */
    void addPixmap(const QPixmap& fileName,
                   QIcon::Mode mode,
                   QIcon::State state) override;
};

#endif // GTSVGICONENGINE_H
