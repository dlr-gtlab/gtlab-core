/* GTlab - Gas Turbine laboratory
 * Source File: gt_icons.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.04.2022
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_application.h"
#include "gt_logging.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QIconEngine>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QGraphicsEffect>

#include <QScreen>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

/// helper macro for creating an icon:
/// Impelments FUNCTION using
/// a static getter function named NAME to reduce redundant calls.
/// Opens the resource at PATH
#define GT_ICON_IMPL(NAME, FUNCTION, PATH) \
    QIcon const& NAME##_impl() { \
        static QIcon ic = gt::gui::getIcon(QStringLiteral(PATH)); \
        return ic; \
    } \
    QIcon gt::gui::icon::FUNCTION() { \
        return NAME##_impl(); \
    }

/// same as above but for coloring icons
#define GT_DEF_ICON_COLORED(FUNCTION, PATH, ACTIVE) \
    QIcon const& FUNCTION##_impl() { \
        static QIcon ic = getIconHelper(QStringLiteral(PATH), \
                                        gt::gui::ACTIVE, gt::gui::ACTIVE); \
        return ic; \
    } \
    QIcon gt::gui::icon::FUNCTION() { \
        return FUNCTION##_impl(); \
    }

/// Defines an icon implementation
#define GT_DEF_ICON(FUNCTION, PATH) \
    GT_ICON_IMPL(FUNCTION,FUNCTION, PATH)

/// Defines an letter icon implementation
#define GT_DEF_ICON_LETTER(FUNCTION, PATH) \
    GT_ICON_IMPL(letter_##FUNCTION, letter::FUNCTION, PATH)

/// Defines an icon alias
#define GT_DEF_ICON_ALIAS(FUNCTION, TARGET) \
    QIcon gt::gui::icon::FUNCTION() { \
        return TARGET(); \
    }

/// Defines  pixmap implementation
#define GT_DEF_PIXMAP(FUNCTION, PATH) \
    QPixmap gt::gui::pixmap::FUNCTION() { \
        return {QStringLiteral(":/pixmaps/" PATH)}; \
    }

/// function pointer type for getting a color
using ColorFunction = QColor(*)();

/**
 * @brief The GtSvgIconEngine class. Can colorize an svg image.
 *
 * Source:
 *
 * https://stackoverflow.com/questions/43125339/
 *      creating-qicon-from-svg-contents-in-memory
 */
class GtSvgIconEngine : public QIconEngine
{
public:

    GtSvgIconEngine() :
        m_colorActive(gt::gui::color::text),
        m_colorDisabled(gt::gui::color::disabled)
    {
        assert(gtApp);
        m_dark = gtApp->inDarkMode();
    };

    explicit GtSvgIconEngine(QString const& path) :
        GtSvgIconEngine()
    {
        GtSvgIconEngine::addFile(path, QSize{}, QIcon::Active, QIcon::On);
    }

    explicit GtSvgIconEngine(QString const& path,
                             ColorFunction active,
                             ColorFunction disbaled = {}) :
        GtSvgIconEngine()
    {
        if (active) m_colorActive = active;
        if (disbaled) m_colorDisabled = disbaled;
        else m_colorDisabled = m_colorActive;

        GtSvgIconEngine::addFile(path, QSize{}, QIcon::Active, QIcon::On);
    }

    /**
     * @brief Clones this icon enigne
     * @return clone
     */
    QIconEngine* clone() const override { return new GtSvgIconEngine(*this); }

    /**
     * @brief Updates the svg file
     * @param fileName file name
     * @param size Size fo the pixmap
     * @param mode Icon mode
     * @param state Icon state
     */
    void addFile(const QString& fileName,
                 QSize const& /*size*/,
                 QIcon::Mode /*mode*/,
                 QIcon::State /*state*/) override
    {
        if (!QFileInfo::exists(fileName))
        {
            gtWarningId("GtSvgIconEngine")
                    << QObject::tr("Invalid icon:") << fileName;
            return;
        }

        QFile file(fileName);
        file.open(QFile::ReadOnly);
        auto data = file.readAll();
        m_file = fileName;

        m_svg = data;
        m_svgDisabled = std::move(data);
        updateColor();
    }

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
               QIcon::State /*state*/) override
    {
        if (m_svg.isEmpty()) return;
        assert(painter);
        painter->setRenderHint(QPainter::Antialiasing);

        if (m_dark != gtApp->inDarkMode())
        {
            updateColor();
        }

        QSvgRenderer renderer(mode == QIcon::Disabled ? m_svgDisabled : m_svg);
        renderer.setAspectRatioMode(Qt::KeepAspectRatio);
        renderer.render(painter, rect);
    }

    /**
     * @brief Returns a pixmap of the svg icon
     * @param size Size fo the pixmap
     * @param mode Icon mode
     * @param state Icon state
     * @return pixmap
     */
    QPixmap pixmap(const QSize& size,
                   QIcon::Mode mode,
                   QIcon::State state) override
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

private:

    QString m_file;
    QByteArray m_svg;
    QByteArray m_svgDisabled;
    ColorFunction m_colorActive{}, m_colorDisabled{};
    bool m_dark = false;

    /**
     * @brief Overrides the color of the svg data
     * @param data Svg data
     * @param color Color
     * @return New data
     */
    QByteArray setColor(QByteArray data, const QColor& color)
    {
        // regexp for setting svg path fill color
        static const auto re = [](){
            QRegularExpression re{R"(path\s(fill=\"[#\w\d]+\")?)"};
            re.optimize();
            return re;
        }();

        auto match = re.match(data);
        int idx = match.capturedStart();
        if (idx > 1)
        {
            data.replace(idx, match.capturedLength(),
                         QByteArrayLiteral("path fill=\"") +
                         color.name().toUtf8() +
                         QByteArrayLiteral("\" "));
        }
        return data;
    }

    /**
     * @brief Updates the color of the svg icon
     */
    void updateColor()
    {
        m_dark = gtApp->inDarkMode();
        m_svg = setColor(m_svg, m_colorActive());
        m_svgDisabled = setColor(m_svgDisabled, m_colorDisabled());
    }

    /**
     * @brief This icon engine does not support svgs
     */
    void addPixmap(const QPixmap& /*fileName*/,
                   QIcon::Mode /*mode*/,
                   QIcon::State /*state*/) override
    {
        gtWarningId("GtSvgIconEngine")
                << QObject::tr("SVG Icon Engine does not accept QPixmaps!");
    }
};

QIcon getIconHelper(QString const& iconPath,
                    ColorFunction active = {},
                    ColorFunction disabled = {})
{
    QString path = iconPath;
    if (!path.startsWith(QStringLiteral(":/")))
    {
        path.prepend(QStringLiteral(":/icons/"));
    }

    if (path.endsWith(QStringLiteral(".svg")))
    {
        return QIcon(new GtSvgIconEngine(path, active, disabled));
    }

    QIcon ic(path);
    if (ic.isNull())
    {
        gtWarning().medium()
                << QObject::tr("Icon '%1' not found").arg(path);
    }
    return ic;
}

QIcon
gt::gui::getIcon(QString const& iconPath)
{
    return getIconHelper(iconPath);
}

QSize
gt::gui::icon::standardSizeSmall()
{
    return QSize(16, 16) * qApp->desktop()->devicePixelRatio();
}

GT_DEF_ICON(add, "add.svg")
GT_DEF_ICON_ALIAS(add16, add)

GT_DEF_ICON_ALIAS(addProject, projectAdd)

GT_DEF_ICON(application, "application.svg")
GT_DEF_ICON(applicationSettings, "applicationSettings.svg")
GT_DEF_ICON(applicationVar, "applicationVar.svg")

GT_DEF_ICON(arrowDown, "arrowDown.svg")
GT_DEF_ICON_ALIAS(arrowDownBlue, uncollapsedColorized)
GT_DEF_ICON_ALIAS(arrowDownBlueAll, arrowDown)
GT_DEF_ICON(arrowRight, "arrowRight_custom.svg")
GT_DEF_ICON_ALIAS(arrowRightBlue, collapsedColorized)
GT_DEF_ICON(arrowLeft, "arrowLeft_custom.svg")
GT_DEF_ICON(arrowUp, "arrowUp.svg")
GT_DEF_ICON_ALIAS(arrowUpBlueAll, arrowUp)

GT_DEF_ICON(backspace, "backspace.svg")
GT_DEF_ICON(backspaceFlipped, "backspaceFlipped.svg")

GT_DEF_ICON(brush, "brush.svg")

GT_DEF_ICON(bug, "bug.svg")
GT_DEF_ICON_COLORED(bugColorized, "bug.svg", color::debugText)
GT_DEF_ICON(bugPlay, "bugPlay.svg")

GT_DEF_ICON(bulb, "bulb.svg")

GT_DEF_ICON(calculator, "calculator.svg")
GT_DEF_ICON_ALIAS(calculator16, calculator)

GT_DEF_ICON(cancel, "cancel.svg")

GT_DEF_ICON(chain, "chain.svg")
GT_DEF_ICON_ALIAS(chain16, chain)
GT_DEF_ICON(chainOff, "chainOff.svg")

GT_DEF_ICON(chartBar, "chartBar.svg")
GT_DEF_ICON(chartBell, "chartBell.svg")
GT_DEF_ICON(chartCurve, "chartCurve.svg")
GT_DEF_ICON(chartLine, "chartLine.svg")
GT_DEF_ICON(chartScatter, "chartScatter.svg")

GT_DEF_ICON(check, "check.svg")
GT_DEF_ICON_ALIAS(check16, check)
GT_DEF_ICON_ALIAS(check24, check)
GT_DEF_ICON_ALIAS(checkSmall16, check)

GT_DEF_ICON(clear, "clear.svg")
GT_DEF_ICON_ALIAS(clear16, clear);
GT_DEF_ICON_ALIAS(clear2_16, backspaceFlipped);

GT_DEF_ICON(clone, "clone.svg")
GT_DEF_ICON_ALIAS(clone16, clone);

GT_DEF_ICON_ALIAS(closeProject, projectClose);
GT_DEF_ICON_ALIAS(closeProject16, projectClose);
GT_DEF_ICON_ALIAS(closeProject24, projectClose);

GT_DEF_ICON_ALIAS(closedProject16, projectClosed);

GT_DEF_ICON_ALIAS(close, cross)

GT_DEF_ICON(cog, "cog.svg")
GT_DEF_ICON(cogPlus, "cogPlus_custom.svg")

GT_DEF_ICON(collapsed, "collapsed_custom.svg")
GT_DEF_ICON_COLORED(collapsedColorized, "collapsed_custom.svg",
                    color::highlight)
GT_DEF_ICON(uncollapsed, "uncollapsed_custom.svg")
GT_DEF_ICON_COLORED(uncollapsedColorized, "uncollapsed_custom.svg",
                    color::highlight)

GT_DEF_ICON(comment, "comment.svg")

GT_DEF_ICON(config, "config.svg")
GT_DEF_ICON_ALIAS(config16, config);

GT_DEF_ICON(connection, "connection.svg")
GT_DEF_ICON_ALIAS(connection16, connection);

GT_DEF_ICON(copy, "copy.svg")
GT_DEF_ICON_ALIAS(copy16, copy);

GT_DEF_ICON(cross, "cross.svg")
GT_DEF_ICON_ALIAS(cross16, cross);

GT_DEF_ICON(cursor, "cursor.svg")
GT_DEF_ICON(cursorMove, "cursorMove.svg")

GT_DEF_ICON(cut, "cut.svg")
GT_DEF_ICON_ALIAS(cut16, cut);

GT_DEF_ICON(data, "data.svg")
GT_DEF_ICON_ALIAS(data16, data);
GT_DEF_ICON(dataArrowUp, "dataArrowUp.svg")
GT_DEF_ICON(dataEye, "dataEye.svg")

GT_DEF_ICON(delete_, "delete.svg")
GT_DEF_ICON_ALIAS(delete16, delete_);

GT_DEF_ICON_ALIAS(deleteProject16, projectDelete);

GT_DEF_ICON(devices, "devices.svg")

GT_DEF_ICON(dock, "dock.svg")

GT_DEF_ICON(dots, "dots.svg")

GT_DEF_ICON(download, "download.svg")

GT_DEF_ICON_ALIAS(duplicate, clone);

GT_DEF_ICON_ALIAS(empty16, objectEmpty);

GT_DEF_ICON(error, "error.svg")
GT_DEF_ICON_COLORED(errorColorized, "error.svg", color::errorText)
GT_DEF_ICON_ALIAS(error16, error);

GT_DEF_ICON_ALIAS(examples, bulb)
GT_DEF_ICON_ALIAS(examples16, examples)

GT_DEF_ICON(exclamationmark, "exclamationmark.svg")
GT_DEF_ICON_ALIAS(exclamation16, exclamationmark);

GT_DEF_ICON(export_, "export.svg")
GT_DEF_ICON_ALIAS(export16, export_);

GT_DEF_ICON(eye, "eye.svg")
GT_DEF_ICON_ALIAS(eye16, eye);
GT_DEF_ICON_ALIAS(eye24, eye);
GT_DEF_ICON(eyeOff, "eyeOff.svg")
GT_DEF_ICON_ALIAS(eyeOff16, eyeOff);
GT_DEF_ICON_ALIAS(eyeOff24, eyeOff);

GT_DEF_ICON(fatal, "fatal.svg")
GT_DEF_ICON_COLORED(fatalColorized, "fatal.svg", color::fatalText)
GT_DEF_ICON_ALIAS(fatal16, fatal);

GT_DEF_ICON(file, "file.svg")
GT_DEF_ICON(fileAdd, "fileAdd.svg")
GT_DEF_ICON(fileClock, "fileClock.svg")
GT_DEF_ICON(fileCode, "fileCode.svg")
GT_DEF_ICON(fileDelete, "fileDelete.svg")
GT_DEF_ICON(fileDoc, "fileDoc.svg")
GT_DEF_ICON(fileEdit, "fileEdit.svg")
GT_DEF_ICON(fileEye, "fileEye.svg")
GT_DEF_ICON(fileImport, "fileImport_custom.svg")
GT_DEF_ICON(fileStep, "fileStep.png")

GT_DEF_ICON_ALIAS(file16, file)

GT_DEF_ICON(folder, "folder.svg")
GT_DEF_ICON_ALIAS(folder16, folder)
GT_DEF_ICON(folderAdd, "folderAdd.svg")
GT_DEF_ICON(folderEdit, "folderEdit.svg")
GT_DEF_ICON(folderEye, "folderEye.svg")
GT_DEF_ICON(folderOpen, "folderOpen.svg")
GT_DEF_ICON(folderSearch, "folderSearch.svg")

GT_DEF_ICON(font, "font.svg")

GT_DEF_ICON(gas, "gas.svg")
GT_DEF_ICON_ALIAS(gas16, gas)
GT_DEF_ICON_ALIAS(gas24, gas)
GT_DEF_ICON_ALIAS(gasTypes, gas)
GT_DEF_ICON_ALIAS(gasTypes16, gas)
GT_DEF_ICON_ALIAS(gasTypes24, gas)

GT_DEF_ICON_ALIAS(global, web)

GT_DEF_ICON(grid, "grid.svg")

GT_DEF_ICON(help, "help.svg")

GT_DEF_ICON(histogram, "histogram.svg")
GT_DEF_ICON(histogram16, "histogram16.svg")

GT_DEF_ICON(history, "history.svg")
GT_DEF_ICON(history16, "history16.svg")

GT_DEF_ICON(home, "home.svg")
GT_DEF_ICON(homeAccount, "homeAccount.svg")

GT_DEF_ICON(import, "import.svg")
GT_DEF_ICON_ALIAS(import16, import)

GT_DEF_ICON_ALIAS(inconsistentProject16, objectInvalid)

GT_DEF_ICON(info, "info.svg")
GT_DEF_ICON_COLORED(infoColorized, "info.svg", color::infoText)
GT_DEF_ICON(info2, "info2.svg")

GT_DEF_ICON(input, "input.svg")
GT_DEF_ICON(inputForm, "inputForm.svg")
GT_DEF_ICON_ALIAS(input16, rename)
GT_DEF_ICON_ALIAS(input2, inputSettings)
GT_DEF_ICON(inputSettings, "inputSettings.svg")

GT_DEF_ICON(jumpTo, "jumpTo_custom.svg")

GT_DEF_ICON_ALIAS(info16, info)
GT_DEF_ICON_ALIAS(infoBlue, info2)
GT_DEF_ICON_ALIAS(infoBlue16, info2)

GT_DEF_ICON(json, "json.svg")

GT_DEF_ICON(label, "label.svg")

GT_DEF_ICON(language, "language.svg")

GT_DEF_ICON(layers, "layers.svg")

GT_DEF_ICON(list, "list.svg")
GT_DEF_ICON_ALIAS(list16, list)
GT_DEF_ICON(listFormatted, "listFormatted.svg")

GT_DEF_ICON_ALIAS(log, fileClock)
GT_DEF_ICON_ALIAS(log16, log)

GT_DEF_ICON(login, "login.svg")
GT_DEF_ICON_ALIAS(login16, login)

GT_DEF_ICON_ALIAS(loop, repeat)
GT_DEF_ICON_ALIAS(loop16, loop)
GT_DEF_ICON_ALIAS(loop2_16, loop)

GT_DEF_ICON(mathDivision, "mathDivision.svg")
GT_DEF_ICON(mathExponent, "mathExponent.svg")
GT_DEF_ICON(mathMinus, "mathMinus.svg")
GT_DEF_ICON(mathMultiplication, "mathMultiplication.svg")
GT_DEF_ICON(mathPlus, "mathPlus.svg")
GT_DEF_ICON(mathRoot, "mathRoot.svg")

GT_DEF_ICON(network, "network.svg")
GT_DEF_ICON_ALIAS(network16, network)

GT_DEF_ICON_ALIAS(noteEdit, fileEdit)

GT_DEF_ICON(notificationUnread, "notificationUnread.svg")

GT_DEF_ICON(objectEmpty, "objectEmpty.svg")
GT_DEF_ICON(objectInvalid, "objectInvalid.svg")
GT_DEF_ICON(objectUnkown, "objectUnkown.svg")

GT_DEF_ICON(open, "open.svg")
GT_DEF_ICON_ALIAS(open16, open)
GT_DEF_ICON_ALIAS(open2, open)
GT_DEF_ICON_ALIAS(open2_16, open)
GT_DEF_ICON_ALIAS(open2_24, open)

GT_DEF_ICON_ALIAS(openProject16, projectOpen)

GT_DEF_ICON(palette, "palette.svg")

GT_DEF_ICON(paste, "paste.svg")
GT_DEF_ICON_ALIAS(paste16, paste)

GT_DEF_ICON(pause, "pause.svg")

GT_DEF_ICON_ALIAS(perspectives, devices)
GT_DEF_ICON(perspectivesEdit, "devicesEdit.svg")
GT_DEF_ICON_ALIAS(perspectives16, perspectives)

GT_DEF_ICON(pdf, "pdf.svg")

GT_DEF_ICON(play, "play.svg")

GT_DEF_ICON_ALIAS(plugin, puzzle)
GT_DEF_ICON_ALIAS(plugin16, plugin)

GT_DEF_ICON(print, "print.svg")
GT_DEF_ICON_ALIAS(printer, print)

GT_DEF_ICON_ALIAS(process, cog)
GT_DEF_ICON_ALIAS(process16, process)
GT_DEF_ICON_ALIAS(processAdd, cogPlus)
GT_DEF_ICON_ALIAS(processRun, play)

GT_DEF_ICON_ALIAS(processFailed16, warning)

GT_DEF_ICON(projectAdd, "projectAdd.svg")
GT_DEF_ICON(projectClose, "projectClose.svg")
GT_DEF_ICON(projectClosed, "projectClosed.svg")
GT_DEF_ICON(projectDelete, "projectDelete.svg")
GT_DEF_ICON(projectImport, "projectImport_custom.svg")
GT_DEF_ICON(projectOpen, "projectOpen.svg")
GT_DEF_ICON(projectRemove, "projectRemove.svg")

GT_DEF_ICON_ALIAS(property, list)
GT_DEF_ICON_ALIAS(property16, property)

GT_DEF_ICON_ALIAS(prp, table)
GT_DEF_ICON_ALIAS(prp16, prp)

GT_DEF_ICON(puzzle, "puzzle.svg")

GT_DEF_ICON(python, "")
GT_DEF_ICON_ALIAS(python16, python)

GT_DEF_ICON(questionmark, "questionmark.svg")
GT_DEF_ICON_ALIAS(question16, questionmark)

GT_DEF_ICON(queue, "queue.svg")
GT_DEF_ICON_ALIAS(queue16, queue)
GT_DEF_ICON(queueAdd, "queueAdd.svg")

GT_DEF_ICON(redo, "redo.svg")
GT_DEF_ICON_ALIAS(redo24, redo)

GT_DEF_ICON(reload, "reload.svg")

GT_DEF_ICON(remove, "remove.svg")

GT_DEF_ICON(rename, "rename.svg")

GT_DEF_ICON(repeat, "repeat.svg")

GT_DEF_ICON(resize, "resize.svg")

GT_DEF_ICON_ALIAS(results, table)
GT_DEF_ICON_ALIAS(results16, results)

GT_DEF_ICON(revert, "revert.svg")
GT_DEF_ICON_ALIAS(restore16, revert)

GT_DEF_ICON(ruler, "ruler.svg")

GT_DEF_ICON_ALIAS(runProcess16, play)

GT_DEF_ICON(save, "save.svg")
GT_DEF_ICON_ALIAS(saveProject16, save)

GT_DEF_ICON(search, "search.svg")
GT_DEF_ICON_ALIAS(search16, search)

GT_DEF_ICON(select, "select.svg")

GT_DEF_ICON(server, "server.svg")

GT_DEF_ICON(session, "session.svg")
GT_DEF_ICON_ALIAS(session16, session)

GT_DEF_ICON(skip, "skip.svg")
GT_DEF_ICON_ALIAS(skip16, skip)

GT_DEF_ICON(sleep, "sleep.svg")
GT_DEF_ICON_ALIAS(sleep16, sleep)

GT_DEF_ICON(gridSnap, "gridSnap.svg")

GT_DEF_ICON(sort, "sort.svg")
GT_DEF_ICON(sortAsc, "sortAsc.svg")
GT_DEF_ICON(sortDesc, "sortDesc.svg")

GT_DEF_ICON(stop, "stop.svg")
GT_DEF_ICON_ALIAS(stop16, stop)

GT_DEF_ICON(stretch, "stretch.svg")

GT_DEF_ICON(swap, "swap.svg")

GT_DEF_ICON(sync, "sync.svg")

GT_DEF_ICON(table, "table.svg")
GT_DEF_ICON_ALIAS(table16, table)

GT_DEF_ICON(text, "text.svg")

GT_DEF_ICON(timer, "timer.svg")
GT_DEF_ICON(timerSand, "timerSand.svg")

GT_DEF_ICON(trace, "trace.svg")
GT_DEF_ICON_COLORED(traceColorized, "trace.svg", color::debugText)
GT_DEF_ICON_ALIAS(trace16, trace)

GT_DEF_ICON(tree, "tree.svg")

GT_DEF_ICON(triangleSmallDown, "triangleSmallDown.svg")
GT_DEF_ICON(triangleSmallLeft, "triangleSmallLeft.svg")
GT_DEF_ICON(triangleSmallRight, "triangleSmallRight.svg")
GT_DEF_ICON(triangleSmallUp, "triangleSmallUp.svg")

GT_DEF_ICON(turbine, "turbine.svg")

GT_DEF_ICON(undo, "undo.svg")
GT_DEF_ICON_ALIAS(undo24, undo)

GT_DEF_ICON(update, "update.svg")
GT_DEF_ICON_ALIAS(update16, update)

GT_DEF_ICON(upload, "upload.svg")

GT_DEF_ICON(variable, "variable.svg")

GT_DEF_ICON(vectorBezier, "vectorBezier.svg")
GT_DEF_ICON(vectorCurve, "vectorCurve.svg")
GT_DEF_ICON(vectorLine, "vectorLine.svg")
GT_DEF_ICON(vectorPoint, "vectorPoint.svg")
GT_DEF_ICON(vectorPointEdit, "vectorPointEdit.svg")
GT_DEF_ICON(vectorSquare, "vectorSquare.svg")
GT_DEF_ICON(vectorSquareEdit, "vectorSquareEdit.svg")
GT_DEF_ICON(vectorTriangle, "vectorTriangle.svg")

GT_DEF_ICON(warning, "warning.svg")
GT_DEF_ICON_COLORED(warningColorized, "warning.svg", color::warningText)
GT_DEF_ICON_ALIAS(warning16, warning)

GT_DEF_ICON(web, "web.svg")

GT_DEF_ICON(xml, "xml.svg")

GT_DEF_ICON(zoomPlus, "zoomPlus.svg")
GT_DEF_ICON_ALIAS(zoom16, zoomPlus)



GT_DEF_ICON_ALIAS(emptyElement, objectEmpty)

GT_DEF_ICON_ALIAS(inProgress16, timerSand)

GT_DEF_ICON_ALIAS(stopRequest16, stop)

GT_DEF_ICON_ALIAS(pathSettings, applicationVar)

GT_DEF_ICON_ALIAS(frame, layers)

GT_DEF_ICON_ALIAS(unknown, objectUnkown)

GT_DEF_ICON_ALIAS(close24, cross)

GT_DEF_ICON_ALIAS(components16, projectOpen)

GT_DEF_ICON_ALIAS(post, chartLine)

GT_DEF_ICON_ALIAS(labels, label)

GT_DEF_ICON_ALIAS(stack, layers)

GT_DEF_ICON_ALIAS(collection16, server)

GT_DEF_ICON_ALIAS(printPDF, pdf)

GT_DEF_ICON_ALIAS(upgradeProjectData, update)

GT_DEF_ICON_ALIAS(move, cursorMove)
GT_DEF_ICON_ALIAS(move16, move)
GT_DEF_ICON_ALIAS(move24, move)

GT_DEF_ICON_ALIAS(plus, mathPlus)
GT_DEF_ICON_ALIAS(minus, mathMinus)
GT_DEF_ICON_ALIAS(multiply, mathMultiplication)
GT_DEF_ICON_ALIAS(divide, mathDivision)
GT_DEF_ICON_ALIAS(square, mathExponent)
GT_DEF_ICON_ALIAS(squareRoot, mathRoot)

GT_DEF_ICON_ALIAS(stepFile, fileStep)
GT_DEF_ICON_ALIAS(stepFile16, stepFile)
GT_DEF_ICON_ALIAS(stepFile24, stepFile)

GT_DEF_ICON_ALIAS(fileIn, fileImport)
GT_DEF_ICON_ALIAS(fileIn16, fileIn)
GT_DEF_ICON_ALIAS(fileIn24, fileIn)

GT_DEF_ICON_ALIAS(textNote24, text)

GT_DEF_ICON_ALIAS(polyCurve, vectorTriangle)
GT_DEF_ICON_ALIAS(bezierCurve, vectorBezier)
GT_DEF_ICON_ALIAS(simpleCurve, vectorCurve)

GT_DEF_ICON_ALIAS(dataBaseArrowUp, dataArrowUp)
GT_DEF_ICON_ALIAS(dataBaseArrowUp16, dataBaseArrowUp)
GT_DEF_ICON_ALIAS(dataBaseArrowUp24, dataBaseArrowUp)

GT_DEF_ICON_ALIAS(editPen, rename)

GT_DEF_ICON_ALIAS(addProcess16, processAdd)
GT_DEF_ICON_ALIAS(addElement16, processAdd)

GT_DEF_ICON_ALIAS(dropdown, download)

GT_DEF_ICON_ALIAS(globals16, data)


QIcon
gt::gui::icon::pluginSettings()
{
    return gt::gui::getIcon(QStringLiteral("pluginSettingsIcon.png"));
}

QIcon
gt::gui::icon::minimize()
{
    return gt::gui::getIcon(QStringLiteral("minimizeIcons.png"));
}

QIcon
gt::gui::icon::map16()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon_16.png"));
}

QIcon
gt::gui::icon::map()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon.png"));
}

QIcon
gt::gui::icon::map24()
{
    return gt::gui::getIcon(QStringLiteral("mapIcon_24.png"));
}

QIcon
gt::gui::icon::noMap()
{
    return gt::gui::getIcon(QStringLiteral("noMapIcon.png"));
}

QIcon
gt::gui::icon::exportMap()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon.png"));
}

QIcon
gt::gui::icon::exportMap16()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon_16.png"));
}

QIcon
gt::gui::icon::exportMap24()
{
    return gt::gui::getIcon(QStringLiteral("exportMapIcon_24.png"));
}

QIcon
gt::gui::icon::carpetPlot()
{
    return gt::gui::getIcon(QStringLiteral("carpetPlotIcon2.png"));
}

QIcon
gt::gui::icon::xyPlot16()
{
    return gt::gui::getIcon(QStringLiteral("schedules4Icon_16.png"));
}

QIcon
gt::gui::icon::tsDiagram16()
{
    return gt::gui::getIcon(QStringLiteral("tsDiagramm_16.png"));
}

QIcon
gt::gui::icon::blades()
{
    return gt::gui::getIcon(QStringLiteral("bladesIcon.png"));
}

QIcon
gt::gui::icon::showText()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon.png"));
}

QIcon
gt::gui::icon::showText16()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon_16.png"));
}

QIcon
gt::gui::icon::showText24()
{
    return gt::gui::getIcon(QStringLiteral("showTextIcon_24.png"));
}

QIcon
gt::gui::icon::convergence()
{
    return gt::gui::getIcon(QStringLiteral("convIcon.png"));
}

QIcon
gt::gui::icon::convergence16()
{
    return gt::gui::getIcon(QStringLiteral("convIcon_16.png"));
}

QIcon
gt::gui::icon::convergence24()
{
    return gt::gui::getIcon(QStringLiteral("convIcon_24.png"));
}


QIcon
gt::gui::icon::double16()
{
    return gt::gui::getIcon(QStringLiteral("doubleIcon_16.png"));
}

QIcon
gt::gui::icon::int16()
{
    return gt::gui::getIcon(QStringLiteral("intIcon_16.png"));
}

QIcon
gt::gui::icon::o16()
{
    return gt::gui::getIcon(QStringLiteral("oIcon_16.png"));
}

QIcon
gt::gui::icon::string16()
{
    return gt::gui::getIcon(QStringLiteral("stringIcon_16.png"));
}

QIcon
gt::gui::icon::performanceModel()
{
    return gt::gui::getIcon(QStringLiteral("performanceModel_own.png"));
}

QIcon
gt::gui::icon::bleedInPort()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon.png"));
}

QIcon
gt::gui::icon::bleedInPort16()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon_16.png"));
}

QIcon
gt::gui::icon::bleedInPort24()
{
    return gt::gui::getIcon(QStringLiteral("bleedInPort3Icon_24.png"));
}

QIcon
gt::gui::icon::bleedOutPort()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon.png"));
}

QIcon
gt::gui::icon::bleedOutPort16()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon_16.png"));
}

QIcon
gt::gui::icon::bleedOutPort24()
{
    return gt::gui::getIcon(QStringLiteral("bleedOutPort3Icon_24.png"));
}

QIcon
gt::gui::icon::bleedPortGroup()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon.png"));
}

QIcon
gt::gui::icon::bleedPortGroup16()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon_16.png"));
}

QIcon
gt::gui::icon::bleedPortGroup24()
{
    return gt::gui::getIcon(QStringLiteral("bleedPortGroupIcon_24.png"));
}

QIcon
gt::gui::icon::engine()
{
    return gt::gui::getIcon(QStringLiteral("engineIcon.png"));
}

QIcon
gt::gui::icon::engine16()
{
    return gt::gui::getIcon(QStringLiteral("engineIcon_16.png"));
}

QIcon
gt::gui::icon::engineInstallation16()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_16.png"));
}

QIcon
gt::gui::icon::engineInstallation24()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_24.png"));
}

QIcon
gt::gui::icon::engineInstallation32()
{
    return gt::gui::getIcon(QStringLiteral("engine_installation_32.png"));
}

QIcon
gt::gui::icon::performanceDesign()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator.png"));
}

QIcon
gt::gui::icon::performanceDesign16()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator_16.png"));
}

QIcon
gt::gui::icon::performanceDesign24()
{
    return gt::gui::getIcon(QStringLiteral("DPcalculator_24.png"));
}

QIcon
gt::gui::icon::performanceOffDesign()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint.png"));
}

QIcon
gt::gui::icon::performanceOffDesign16()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint_16.png"));
}

QIcon
gt::gui::icon::performanceOffDesign24()
{
    return gt::gui::getIcon(QStringLiteral("operatingPoint_24.png"));
}

QIcon
gt::gui::icon::fluidPort()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon.png"));
}

QIcon
gt::gui::icon::fluidPort16()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon_16.png"));
}

QIcon
gt::gui::icon::fluidPort24()
{
    return gt::gui::getIcon(QStringLiteral("fluidPort3Icon_24.png"));
}

QIcon
gt::gui::icon::fluidPortGroup()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon.png"));
}

QIcon
gt::gui::icon::fluidPortGroup16()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon_16.png"));
}

QIcon
gt::gui::icon::fluidPortGroup24()
{
    return gt::gui::getIcon(QStringLiteral("fluidPortGroupIcon_24.png"));
}

QIcon
gt::gui::icon::shaftPort()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon.png"));
}

QIcon
gt::gui::icon::shaftPort16()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon_16.png"));
}

QIcon
gt::gui::icon::shaftPort24()
{
    return gt::gui::getIcon(QStringLiteral("shaftPort3Icon_24.png"));
}

QIcon
gt::gui::icon::speedLine()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_32.png"));
}

QIcon
gt::gui::icon::speedLine16()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_16.png"));
}

QIcon
gt::gui::icon::speedLine24()
{
    return gt::gui::getIcon(QStringLiteral("speedLineIcon_24.png"));
}

QIcon
gt::gui::icon::compressor()
{
    return gt::gui::getIcon(QStringLiteral("compressorIcon.png"));
}

QIcon
gt::gui::icon::compressor16()
{
    return gt::gui::getIcon(QStringLiteral("compressorIcon_16.png"));
}

QIcon
gt::gui::icon::turbine16()
{
    return gt::gui::getIcon(QStringLiteral("turbineIcon_16.png"));
}

QIcon
gt::gui::icon::combustor()
{
    return gt::gui::getIcon(QStringLiteral("combustorIcon.png"));
}

QIcon
gt::gui::icon::combustor16()
{
    return gt::gui::getIcon(QStringLiteral("combustorIcon_16.png"));
}

QIcon
gt::gui::icon::paramStudy()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon.png"));
}

QIcon
gt::gui::icon::paramStudy16()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon_16.png"));
}

QIcon
gt::gui::icon::paramStudy24()
{
    return gt::gui::getIcon(QStringLiteral("paramStudyIcon_24.png"));
}


GT_DEF_ICON_LETTER(a, "aLetterIcon.png")
GT_DEF_ICON_LETTER(aSmall, "aLetterIconSmall.png")
GT_DEF_ICON_LETTER(b, "bLetterIcon.png")
GT_DEF_ICON_LETTER(bSmall, "bLetterIconSmall.png")
GT_DEF_ICON_LETTER(c, "cLetterIcon.png")
GT_DEF_ICON_LETTER(cSmall, "cLetterIconSmall.png")
GT_DEF_ICON_LETTER(d, "dLetterIcon.png")
GT_DEF_ICON_LETTER(dSmall, "dLetterIconSmall.png")
GT_DEF_ICON_LETTER(e, "eLetterIconBlue.png")
GT_DEF_ICON_LETTER(eSmall, "eLetterIconSmallBlue.png")
GT_DEF_ICON_LETTER(eBrown, "eLetterIcon.png")
GT_DEF_ICON_LETTER(eBrownSmall, "e_LetterIconSmall.png")
GT_DEF_ICON_LETTER(f, "fLetterIcon.png")
GT_DEF_ICON_LETTER(fSmall, "fLetterIconSmall.png")
GT_DEF_ICON_LETTER(g, "gLetterIcon.png")
GT_DEF_ICON_LETTER(gSmall, "gLetterIconSmall.png")
GT_DEF_ICON_LETTER(h, "hLetterIcon.png")
GT_DEF_ICON_LETTER(hSmall, "hLetterIconSmall.png")
GT_DEF_ICON_LETTER(i, "iLetterIcon.png")
GT_DEF_ICON_LETTER(iSmall, "iLetterIconSmall.png")
GT_DEF_ICON_LETTER(j, "jLetterIcon.png")
GT_DEF_ICON_LETTER(jSmall, "jLetterIconSmall.png")
GT_DEF_ICON_LETTER(k, "kLetterIcon.png")
GT_DEF_ICON_LETTER(kSmall, "kLetterIconSmall.png")
GT_DEF_ICON_LETTER(l, "lLetterIcon.png")
GT_DEF_ICON_LETTER(lSmall, "lLetterIconSmall.png")
GT_DEF_ICON_LETTER(m, "mLetterIcon.png")
GT_DEF_ICON_LETTER(mSmall, "mLetterIconSmall.png")
GT_DEF_ICON_LETTER(n, "nLetterIcon.png")
GT_DEF_ICON_LETTER(nSmall, "nLetterIconSmall.png")
GT_DEF_ICON_LETTER(o, "oLetterIcon.png")
GT_DEF_ICON_LETTER(oSmall, "oLetterIconSmall.png")
GT_DEF_ICON_LETTER(p, "pLetterIcon.png")
GT_DEF_ICON_LETTER(pSmall, "pLetterIconSmall.png")
GT_DEF_ICON_LETTER(q, "qLetterIcon.png")
GT_DEF_ICON_LETTER(qSmall, "qLetterIconSmall.png")
GT_DEF_ICON_LETTER(r, "rLetterIcon.png")
GT_DEF_ICON_LETTER(rSmall, "rLetterIconSmall.png")
GT_DEF_ICON_LETTER(s, "sLetterIcon.png")
GT_DEF_ICON_LETTER(sSmall, "sLetterIconSmall.png")
GT_DEF_ICON_LETTER(t, "tLetterIcon.png")
GT_DEF_ICON_LETTER(tSmall, "tLetterIconSmall.png")
GT_DEF_ICON_LETTER(u, "uLetterIcon.png")
GT_DEF_ICON_LETTER(uSmall, "uLetterIconSmall.png")
GT_DEF_ICON_LETTER(v, "vLetterIcon.png")
GT_DEF_ICON_LETTER(vSmall, "vLetterIconSmall.png")
GT_DEF_ICON_LETTER(w, "wLetterIcon.png")
GT_DEF_ICON_LETTER(wSmall, "wLetterIconSmall.png")
GT_DEF_ICON_LETTER(x, "xLetterIcon.png")
GT_DEF_ICON_LETTER(xSmall, "xLetterIconSmall.png")
GT_DEF_ICON_LETTER(y, "yLetterIcon.png")
GT_DEF_ICON_LETTER(ySmall, "yLetterIconSmall.png")
GT_DEF_ICON_LETTER(z, "zLetterIcon.png")
GT_DEF_ICON_LETTER(zSmall, "zLetterIconSmall.png")


QIcon
gt::gui::icon::processRunningIcon(int progress)
{
    if (progress == 0)
    {
        return gt::gui::icon::timerSand();
    }

    QRgb progressDone = qRgb(0, 255, 0);
    QRgb progressToDo = qRgb(255, 255, 255);

    int w = 100;
    int h = 100;
    QImage img(w, h, QImage::Format_Indexed8);

    img.setColorCount(2);
    img.setColor(0, progressDone);
    img.setColor(1, progressToDo);
    img.fill(1);

    double currentFillWidth = 1.0 * w * progress / 100.0;

    for (int i = 0; i < currentFillWidth; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            img.setPixel(i, j, 0);
        }
    }

    /// Note: as the current overlay does not improve the design it is
    /// removed but the code should remain to enable the usage of another
    /// overly easily
    QIcon retValIcon;
    QPixmap comboPixmap(100, 100);
    //QPixmap overlayImage(gt::gui::icon::inProgress16().pixmap(16, 16));
    QPixmap backGroundImage;
    backGroundImage.convertFromImage(img, Qt::ColorOnly);

    QPainter painter(&comboPixmap);
    painter.drawPixmap(0, 0, backGroundImage);
    //painter.drawPixmap(0, 0, overlayImage);

    retValIcon.addPixmap(comboPixmap);

    return retValIcon;
}


QString
gt::gui::pixmap::logoString()
{
    return {":/pixmaps/gt-logo.png"};
}

GT_DEF_PIXMAP(splash, "splash.png");

GT_DEF_PIXMAP(accessLogo, "accessLogo.png");

GT_DEF_PIXMAP(updateLogo, "updateLogo.png");

GT_DEF_PIXMAP(changeLogLogo, "changelogLogo.png");

GT_DEF_PIXMAP(printHeadline, "print-headline.png");

GT_DEF_PIXMAP(downloaderLogo, "print-downloaderLogo.png");

QPixmap
gt::gui::pixmap::question24()
{
    return icon::questionmark().pixmap(24, 24);
}

QPixmap
gt::gui::pixmap::jumpTo()
{
    return icon::jumpTo().pixmap(32, 32);
}

QPixmap
gt::gui::pixmap::logo()
{
    if (gtApp->inDarkMode())
    {
        return {":/pixmaps/gt-logo-dark.png"};
    }

    return {":/pixmaps/gt-logo.png"};
}
