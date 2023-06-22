/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 9.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_svgiconengine.h"

#include "gt_logging.h"
#include "gt_colors.h"
#include "gt_application.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QSvgRenderer>


gt::gui::SvgColorData::SvgColorData() :
    normal(color::text),
    disabled(color::disabled),
    selected(color::textHighlight)
{ }

gt::gui::SvgColorData::SvgColorData(ColorFunctor _normal,
                                    ColorFunctor _disabled,
                                    ColorFunctor _selected) :
    normal(_normal ?     std::move(_normal)   : color::text),
    disabled(_disabled ? std::move(_disabled) : this->normal),
    selected(_selected ? std::move(_selected) : this->normal)
{ }

gt::gui::SvgColorData::SvgColorData(QColor const& _normal,
                                    QColor const& _disabled,
                                    QColor const& _selected) :
    normal(_normal.isValid() ?     ColorFunctor([=](){ return _normal; })   :
                                   color::text),
    disabled(_disabled.isValid() ? ColorFunctor([=](){ return _disabled; }) :
                                   this->normal),
    selected(_selected.isValid() ? ColorFunctor([=](){ return _selected; }) :
                                   this->normal)
{ }

GtSvgIconEngine::GtSvgIconEngine() = default;

GtSvgIconEngine::GtSvgIconEngine(const QString& path) :
    GtSvgIconEngine()
{
    GtSvgIconEngine::addFile(path, QSize{}, QIcon::Normal, QIcon::On);
}

GtSvgIconEngine::GtSvgIconEngine(const QString& path,
                                 SvgColorData colorData) :
    GtSvgIconEngine()
{
    m_color = std::move(colorData);

    if (!m_color.normal)
    {
        SvgColorData dummy;
        m_color.normal = dummy.normal;
    }
    if (!m_color.disabled)
    {
        m_color.disabled = m_color.normal;
    }
    if (!m_color.selected)
    {
        m_color.selected = m_color.normal;
    }

    GtSvgIconEngine::addFile(path, QSize{}, QIcon::Normal, QIcon::On);
}

void
GtSvgIconEngine::addFile(const QString& fileName,
                         const QSize& /*size*/,
                         QIcon::Mode mode,
                         QIcon::State /*state*/)
{
    if (!QFileInfo::exists(fileName))
    {
        gtWarningId("GtSvgIconEngine")
                << QObject::tr("Invalid icon:") << fileName;
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        gtWarningId("GtSvgIconEngine")
                << QObject::tr("Failed to open icon:") << fileName;
        return;
    }
    auto data = file.readAll();

    m_file = fileName;
    m_svg = std::move(data);

    applyColor(mode);
}

void
GtSvgIconEngine::paint(QPainter* painter,
                       const QRect& rect,
                       QIcon::Mode mode,
                       QIcon::State /*state*/)
{
    if (m_svg.isEmpty()) return;

    assert(painter);
    painter->setRenderHint(QPainter::Antialiasing);

    applyColor(m_color.color(mode));

    QSvgRenderer renderer(m_svg);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(painter, rect);
}

QPixmap
GtSvgIconEngine::pixmap(const QSize& size,
                        QIcon::Mode mode,
                        QIcon::State state)
{
    // create transparent pixmap
    QPixmap pix{size};
    pix.fill(Qt::transparent);

    if (!m_svg.isEmpty())
    {
        QPainter painter(&pix);
        QRect r{QPoint{0, 0}, size};
        this->paint(&painter, r, mode, state);
    }
    return pix;
}

void
GtSvgIconEngine::applyColor(const QColor& color)
{
    // simply replace color by index
    if (m_svgIdx > 0 && m_svgIdx < m_svg.size())
    {
        constexpr int colorCodeLength = 7; // #000000 = 7 characters
        m_svg.replace(m_svgIdx,
                      colorCodeLength,
                      color.name().toUtf8());
        return;
    }

    m_svgIdx = -1;

    // regexp for setting svg path fill color
    static const auto re = [](){
        QRegularExpression re{R"(path\s(fill=\"[#\w\d]+\")?)"};
        re.optimize();
        return re;
    }();

    auto match = re.match(m_svg);
    int idx = match.capturedStart();
    if (idx > 1)
    {
        static const auto prefix = QByteArrayLiteral("path fill=\"");
        static const auto suffix = QByteArrayLiteral("\" ");

        m_svg.replace(idx, match.capturedLength(),
                      prefix + color.name().toUtf8() + suffix);
        m_svgIdx = idx + prefix.size();
    }
}

void
GtSvgIconEngine::addPixmap(const QPixmap& /*pixmap*/,
                           QIcon::Mode /*mode*/,
                           QIcon::State /*state*/)
{
    gtWarningId("GtSvgIconEngine")
            << QObject::tr("SVG Icon Engine does not accept QPixmaps!");
}
