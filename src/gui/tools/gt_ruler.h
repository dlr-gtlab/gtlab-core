/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_ruler.h
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_RULER_H
#define GTD_RULER_H

#include "gt_gui_exports.h"
#include "gt_grid.h"
#include "gt_version.h"

#include <QWidget>
#include <QPainterPath>

class QGraphicsView;
struct GtGridSpacing;
class GT_GUI_EXPORT GtRuler : public QWidget
{
    Q_OBJECT

public:
    explicit GtRuler(Qt::Orientation o = Qt::Horizontal);
    ~GtRuler() override;

    /** Returns Buffer.
        @return Buffer */
    QPixmap& buffer();

    /** Returns ruler orientation.
        @return Ruler orientation */
    Qt::Orientation orientation() const;

    /** Returns custom ruler font.
        @return Ruler font */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `font()` instead")
    QFont getFont() const
    {
        return font();
    }

    /** Returns size of given string.
        @param str String
        @return Size of string */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `textSizeHint(str)` instead")
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

    void setGridSpacing(GtGridSpacing spacing);

    /** Sets cursor postion to given point.
        @param pos New cursor position */
    void setCursorPosition(const QPoint& pos);

    /** Returns whether ruler needs a repaint or not.
        @return Repaint indicator */
    bool needsRepaint() const { return true; }
    bool needsRepaint(QRectF sceneRect, QTransform sceneTransform) const;

    /** Sets repaint indicator.
        @param val New repaint indicator */
    void setNeedsRepaint(bool val) { if (val) invalidateCache(); }
    void invalidateCache();

    virtual void paint(QRectF sceneRect, QTransform sceneTransform);

protected:
    /** Overloaded Paint Function
     * @param e paint event */
    void paintEvent(QPaintEvent* e) override;

    /** Resize Ruler scale
     * @param e resize event
     */
    void resizeEvent(QResizeEvent* e) override;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTD_RULER_H
