/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_ruler.h
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#ifndef GT_RULER_H
#define GT_RULER_H

#include <gt_gui_exports.h>
#include <gt_gridspacing.h>
#include <gt_version.h>

#include <QWidget>

#include <memory>

class QGraphicsView;
struct GtGridSpacing;
class GT_GUI_EXPORT GtRuler : public QWidget
{
    Q_OBJECT

public:

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtRuler(Qt::Orientation, QWidget*)` instead.")
    explicit GtRuler() : GtRuler(Qt::Horizontal) { }
    explicit GtRuler(Qt::Orientation o, QWidget* parent = nullptr);
    ~GtRuler() override;

    /**
     * @brief Returns the buffer used for caching the rendered ruler.
     * @return Buffer
     */
    QPixmap& buffer();

    /**
     * @brief Returns ruler orientation.
     * @return Ruler orientation
     */
    Qt::Orientation orientation() const;

    /**
     * @brief Sets the orientation of the ruler.
     * @param o New orientation
     */
    void setOrientation(Qt::Orientation o);

    /**
     * @brief Sets whether the axis is flipped, meaning that x becomes -x.
     * By default the axis is flipped for vertical rulers.
     * @param flipped Whether to flip the axis.
     */
    void flipAxis(bool flipped = true);

    /**
     * @brief Returns whether the axis is flipped, meaning that x becomes -x.
     * @return Whether axis is flipped.
     */
    bool isAxisFlipped() const;

    /**
     * @brief Sets an offset to shift the ruler, which may be used in case the
     * widget that is operated on is not aligned perfectly to the ruler
     * (e.g. due to frame boundaries)
     * @param offset Offset
     */
    void setDrawOffset(int offset);

    /**
     * @brief Returns the draw offset.
     * @return Offset
     */
    int drawOffset() const;

    /**
     * @brief Returns custom ruler font.
     * @return Ruler font
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `font()` instead")
    QFont getFont() const
    {
        return font();
    }

    /**
     * @brief Returns size of given string.
     * @param str String
     * @return Size of string
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `textSizeHint(QString)` instead")
    QSize getFontSizeHint(const QString& str) const
    {
        return textSizeHint(str);
    }
    /**
     * @brief Returns the size of the given string
     * @param str String
     * @return Size of the string
     */
    QSize textSizeHint(const QString& str) const;

    /**
     * @brief Sets cursor position to given point.
     * @param pos New cursor position
     */
    void setCursorPosition(const QPoint& pos);

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `needsRepaint(QRectF, QTransform)` instead")
    bool needsRepaint() const { return true; }
    /**
     * @brief Returns whether ruler needs a repaint or not.
     * @return Whether to repaint the ruler.
     */
    bool needsRepaint(QRectF backgroundRect, QTransform viewportTransform) const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `invalidate()` instead")
    void setNeedsRepaint(bool val) { if (val) invalidate(); }
    /**
     * @brief Invalidates the ruler, causing it to redraw.
     */
    void invalidate();

    /**
     * @brief Paints the ruler to the buffer. Triggers a redraw of the widget.
     * @param spacing Current (scaled) grid spacing, indicating the distance
     * between ticks. Zero aligned.
     * @param backgroundRect Rect of the graphicsview background
     * @param viewportTransform Transform of the viewport
     */
    virtual void paint(GtGridSpacing spacing, QRectF backgroundRect, QTransform viewportTransform);

protected:

    /**
     * @brief Overloaded Paint Function. Renders the caches pixmap.
     * @param e paint event
     */
    void paintEvent(QPaintEvent* e) override;

    /**
     * @brief Resizes the ruler buffer
     * @param e resize event
     */
    void resizeEvent(QResizeEvent* e) override;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GT_RULER_H
