/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 9.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_iconbrowser.h"

#include "gt_icons.h"

#include <QKeyEvent>

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

/// helper macro for creating a new Icon-Widget (use with grid->addWidget(...))
#define MAKE_ICON(NAME) \
    [](){ \
    auto* btn = new QPushButton(QStringLiteral(#NAME)); \
    btn->setIcon(gt::gui::icon::NAME()); \
    return btn; }(), row, incCol()


GtIconBrowser::GtIconBrowser()
{
    constexpr int MAX_COLS = 11;

    auto* grid = new QGridLayout;
    setLayout(grid);

    int row = 0;
    int col = 0;

    // "finishes" the current row
    auto makeSeparator = [&](){
        row += 1;
        col = 0;
    };

    // increments col
    auto incCol = [&](){
        int tmp = col;
        col += 1;
        col %= MAX_COLS;
        if (tmp == 0) row += 1;
        return tmp;
    };

    grid->addWidget(MAKE_ICON(add));
    grid->addWidget(MAKE_ICON(application));
    grid->addWidget(MAKE_ICON(applicationSettings));
    grid->addWidget(MAKE_ICON(applicationVar));
    grid->addWidget(MAKE_ICON(arrowDown));
    grid->addWidget(MAKE_ICON(arrowUp));
    grid->addWidget(MAKE_ICON(arrowRight));
    grid->addWidget(MAKE_ICON(arrowLeft));
    grid->addWidget(MAKE_ICON(backspace));
    grid->addWidget(MAKE_ICON(backspaceFlipped));
    grid->addWidget(MAKE_ICON(binaryTree));
    grid->addWidget(MAKE_ICON(blades));
    grid->addWidget(MAKE_ICON(brush));
    grid->addWidget(MAKE_ICON(bug));
    grid->addWidget(MAKE_ICON(bugPlay));
    grid->addWidget(MAKE_ICON(calculator));
    grid->addWidget(MAKE_ICON(cancel));
    grid->addWidget(MAKE_ICON(carpetPlot));
    grid->addWidget(MAKE_ICON(chain));
    grid->addWidget(MAKE_ICON(chainOff));
    grid->addWidget(MAKE_ICON(chartBar));
    grid->addWidget(MAKE_ICON(chartCurve));
    grid->addWidget(MAKE_ICON(chartLine));
    grid->addWidget(MAKE_ICON(chartScatter));
    grid->addWidget(MAKE_ICON(chartXY));
    grid->addWidget(MAKE_ICON(check));
    grid->addWidget(MAKE_ICON(clear));
    grid->addWidget(MAKE_ICON(clone));
    grid->addWidget(MAKE_ICON(close));
    grid->addWidget(MAKE_ICON(cog));
    grid->addWidget(MAKE_ICON(cogPlus));
    grid->addWidget(MAKE_ICON(collapsed));
    grid->addWidget(MAKE_ICON(uncollapsed));
    grid->addWidget(MAKE_ICON(collection));
    grid->addWidget(MAKE_ICON(comment));
    grid->addWidget(MAKE_ICON(config));
    grid->addWidget(MAKE_ICON(connection));
    grid->addWidget(MAKE_ICON(convergence));
    grid->addWidget(MAKE_ICON(copy));
    grid->addWidget(MAKE_ICON(cross));
    grid->addWidget(MAKE_ICON(cursor));
    grid->addWidget(MAKE_ICON(cursorMove));
    grid->addWidget(MAKE_ICON(cut));
    grid->addWidget(MAKE_ICON(data));
    grid->addWidget(MAKE_ICON(dataArrowUp));
    grid->addWidget(MAKE_ICON(dataEye));
    grid->addWidget(MAKE_ICON(dataLock));
    grid->addWidget(MAKE_ICON(dataSingle));
    grid->addWidget(MAKE_ICON(delete_));
    grid->addWidget(MAKE_ICON(disk));
    grid->addWidget(MAKE_ICON(diskGradient));
    grid->addWidget(MAKE_ICON(dock));
    grid->addWidget(MAKE_ICON(dots));
    grid->addWidget(MAKE_ICON(download));
    grid->addWidget(MAKE_ICON(duplicate));
    grid->addWidget(MAKE_ICON(engineInstallation));
    grid->addWidget(MAKE_ICON(error));
    grid->addWidget(MAKE_ICON(examples));
    grid->addWidget(MAKE_ICON(exclamationmark));
    grid->addWidget(MAKE_ICON(export_));
    grid->addWidget(MAKE_ICON(eye));
    grid->addWidget(MAKE_ICON(eyeOff));
    grid->addWidget(MAKE_ICON(fatal));
    grid->addWidget(MAKE_ICON(file));
    grid->addWidget(MAKE_ICON(fileAdd));
    grid->addWidget(MAKE_ICON(fileClock));
    grid->addWidget(MAKE_ICON(fileCode));
    grid->addWidget(MAKE_ICON(fileDelete));
    grid->addWidget(MAKE_ICON(fileDoc));
    grid->addWidget(MAKE_ICON(fileEdit));
    grid->addWidget(MAKE_ICON(fileEye));
    grid->addWidget(MAKE_ICON(fileImport));
    grid->addWidget(MAKE_ICON(folder));
    grid->addWidget(MAKE_ICON(folderAdd));
    grid->addWidget(MAKE_ICON(folderEdit));
    grid->addWidget(MAKE_ICON(folderEye));
    grid->addWidget(MAKE_ICON(folderOpen));
    grid->addWidget(MAKE_ICON(folderSearch));
    grid->addWidget(MAKE_ICON(font));
    grid->addWidget(MAKE_ICON(gas));
    grid->addWidget(MAKE_ICON(global));
    grid->addWidget(MAKE_ICON(grid));
    grid->addWidget(MAKE_ICON(gridSnap));
    grid->addWidget(MAKE_ICON(help));
    grid->addWidget(MAKE_ICON(hdf5));
    grid->addWidget(MAKE_ICON(histogram));
    grid->addWidget(MAKE_ICON(history));
    grid->addWidget(MAKE_ICON(home));
    grid->addWidget(MAKE_ICON(homeAccount));
    grid->addWidget(MAKE_ICON(import));
    grid->addWidget(MAKE_ICON(info));
    grid->addWidget(MAKE_ICON(info2));
    grid->addWidget(MAKE_ICON(input));
    grid->addWidget(MAKE_ICON(inputForm));
    grid->addWidget(MAKE_ICON(inputSettings));
    grid->addWidget(MAKE_ICON(jumpTo));
    grid->addWidget(MAKE_ICON(json));
    grid->addWidget(MAKE_ICON(label));
    grid->addWidget(MAKE_ICON(language));
    grid->addWidget(MAKE_ICON(layers));
    grid->addWidget(MAKE_ICON(list));
    grid->addWidget(MAKE_ICON(listFormatted));
    grid->addWidget(MAKE_ICON(log));
    grid->addWidget(MAKE_ICON(login));
    grid->addWidget(MAKE_ICON(loop));
    grid->addWidget(MAKE_ICON(magnet));
    grid->addWidget(MAKE_ICON(mass));
    grid->addWidget(MAKE_ICON(mathDivision));
    grid->addWidget(MAKE_ICON(mathExponent));
    grid->addWidget(MAKE_ICON(mathMinus));
    grid->addWidget(MAKE_ICON(mathMultiplication));
    grid->addWidget(MAKE_ICON(mathPlus));
    grid->addWidget(MAKE_ICON(mathRoot));
    grid->addWidget(MAKE_ICON(minimize));
    grid->addWidget(MAKE_ICON(network));
    grid->addWidget(MAKE_ICON(noteEdit));
    grid->addWidget(MAKE_ICON(notificationUnread));
    grid->addWidget(MAKE_ICON(objectCombustor));
    grid->addWidget(MAKE_ICON(objectCompressor));
    grid->addWidget(MAKE_ICON(objectEmpty));
    grid->addWidget(MAKE_ICON(objectEngine));
    grid->addWidget(MAKE_ICON(objectFreestyleComponent));
    grid->addWidget(MAKE_ICON(objectInvalid));
    grid->addWidget(MAKE_ICON(objectUnknown));
    grid->addWidget(MAKE_ICON(objectTurbine));
    grid->addWidget(MAKE_ICON(open));
    grid->addWidget(MAKE_ICON(palette));
    grid->addWidget(MAKE_ICON(paramStudy));
    grid->addWidget(MAKE_ICON(paste));
    grid->addWidget(MAKE_ICON(pause));
    grid->addWidget(MAKE_ICON(pdf));
    grid->addWidget(MAKE_ICON(perfDesignPoint));
    grid->addWidget(MAKE_ICON(perfMap));
    grid->addWidget(MAKE_ICON(perfMapExport));
    grid->addWidget(MAKE_ICON(perfNoMap));
    grid->addWidget(MAKE_ICON(perfModel));
    grid->addWidget(MAKE_ICON(perfOperatingPoint));
    grid->addWidget(MAKE_ICON(perfStageMap));
    grid->addWidget(MAKE_ICON(perfSpeedLine));
    grid->addWidget(MAKE_ICON(perfTsDiagram));
    grid->addWidget(MAKE_ICON(perfWorkingLine));
    grid->addWidget(MAKE_ICON(perspectives));
    grid->addWidget(MAKE_ICON(perspectivesEdit));
    grid->addWidget(MAKE_ICON(play));
    grid->addWidget(MAKE_ICON(plugin));
    grid->addWidget(MAKE_ICON(print));
    grid->addWidget(MAKE_ICON(process));
    grid->addWidget(MAKE_ICON(processAdd));
    grid->addWidget(MAKE_ICON(projectAdd));
    grid->addWidget(MAKE_ICON(projectClose));
    grid->addWidget(MAKE_ICON(projectClosed));
    grid->addWidget(MAKE_ICON(projectDelete));
    grid->addWidget(MAKE_ICON(projectImport));
    grid->addWidget(MAKE_ICON(projectOpen));
    grid->addWidget(MAKE_ICON(projectRemove));
    grid->addWidget(MAKE_ICON(property));
    grid->addWidget(MAKE_ICON(prp));
    grid->addWidget(MAKE_ICON(puzzle));
    grid->addWidget(MAKE_ICON(python));
    grid->addWidget(MAKE_ICON(questionmark));
    grid->addWidget(MAKE_ICON(queue));
    grid->addWidget(MAKE_ICON(queueAdd));
    grid->addWidget(MAKE_ICON(redo));
    grid->addWidget(MAKE_ICON(reload));
    grid->addWidget(MAKE_ICON(remove));
    grid->addWidget(MAKE_ICON(rename));
    grid->addWidget(MAKE_ICON(repeat));
    grid->addWidget(MAKE_ICON(resize));
    grid->addWidget(MAKE_ICON(results));
    grid->addWidget(MAKE_ICON(revert));
    grid->addWidget(MAKE_ICON(ruler));
    grid->addWidget(MAKE_ICON(save));
    grid->addWidget(MAKE_ICON(schedules));
    grid->addWidget(MAKE_ICON(schedules2));
    grid->addWidget(MAKE_ICON(search));
    grid->addWidget(MAKE_ICON(search));
    grid->addWidget(MAKE_ICON(select));
    grid->addWidget(MAKE_ICON(server));
    grid->addWidget(MAKE_ICON(session));
    grid->addWidget(MAKE_ICON(showText));
    grid->addWidget(MAKE_ICON(skip));
    grid->addWidget(MAKE_ICON(unskip));
    grid->addWidget(MAKE_ICON(sleep));
    grid->addWidget(MAKE_ICON(sleepOff));
    grid->addWidget(MAKE_ICON(square));
    grid->addWidget(MAKE_ICON(sort));
    grid->addWidget(MAKE_ICON(sortAsc));
    grid->addWidget(MAKE_ICON(sortDesc));
    grid->addWidget(MAKE_ICON(stop));
    grid->addWidget(MAKE_ICON(stepFile));
    grid->addWidget(MAKE_ICON(stretch));
    grid->addWidget(MAKE_ICON(swap));
    grid->addWidget(MAKE_ICON(sync));
    grid->addWidget(MAKE_ICON(table));
    grid->addWidget(MAKE_ICON(text));
    grid->addWidget(MAKE_ICON(timer));
    grid->addWidget(MAKE_ICON(timerSand));
    grid->addWidget(MAKE_ICON(trace));
    grid->addWidget(MAKE_ICON(tree));
    grid->addWidget(MAKE_ICON(triangleDown));
    grid->addWidget(MAKE_ICON(triangleLeft));
    grid->addWidget(MAKE_ICON(triangleRight));
    grid->addWidget(MAKE_ICON(triangleRightGroup));
    grid->addWidget(MAKE_ICON(triangleUp));
    grid->addWidget(MAKE_ICON(triangleSmallDown));
    grid->addWidget(MAKE_ICON(triangleSmallLeft));
    grid->addWidget(MAKE_ICON(triangleSmallRight));
    grid->addWidget(MAKE_ICON(triangleSmallUp));
    grid->addWidget(MAKE_ICON(undo));
    grid->addWidget(MAKE_ICON(update));
    grid->addWidget(MAKE_ICON(upload));
    grid->addWidget(MAKE_ICON(variable));
    grid->addWidget(MAKE_ICON(vectorBezier));
    grid->addWidget(MAKE_ICON(vectorBezier2));
    grid->addWidget(MAKE_ICON(vectorCurve));
    grid->addWidget(MAKE_ICON(vectorLine));
    grid->addWidget(MAKE_ICON(vectorPoint));
    grid->addWidget(MAKE_ICON(vectorPointEdit));
    grid->addWidget(MAKE_ICON(vectorPoly));
    grid->addWidget(MAKE_ICON(vectorSimple));
    grid->addWidget(MAKE_ICON(vectorSquare));
    grid->addWidget(MAKE_ICON(vectorSquareEdit));
    grid->addWidget(MAKE_ICON(vectorTriangle));
    grid->addWidget(MAKE_ICON(warning));
    grid->addWidget(MAKE_ICON(web));
    grid->addWidget(MAKE_ICON(zoomMinus));
    grid->addWidget(MAKE_ICON(zoomPlus));
    grid->addWidget(MAKE_ICON(bleedInPort));
    grid->addWidget(MAKE_ICON(bleedOutPort));
    grid->addWidget(MAKE_ICON(bleedPortGroup));
    grid->addWidget(MAKE_ICON(fluidPort));
    grid->addWidget(MAKE_ICON(fluidPortGroup));
    grid->addWidget(MAKE_ICON(shaftPort));
    makeSeparator();
    grid->addWidget(new QLabel(tr("Letters:")), row, incCol());
    makeSeparator();
    grid->addWidget(MAKE_ICON(letter::a));
    grid->addWidget(MAKE_ICON(letter::aSmall));
    grid->addWidget(MAKE_ICON(letter::b));
    grid->addWidget(MAKE_ICON(letter::bSmall));
    grid->addWidget(MAKE_ICON(letter::c));
    grid->addWidget(MAKE_ICON(letter::cSmall));
    grid->addWidget(MAKE_ICON(letter::d));
    grid->addWidget(MAKE_ICON(letter::dSmall));
    grid->addWidget(MAKE_ICON(letter::e));
    grid->addWidget(MAKE_ICON(letter::eSmall));
    grid->addWidget(MAKE_ICON(letter::f));
    grid->addWidget(MAKE_ICON(letter::fSmall));
    grid->addWidget(MAKE_ICON(letter::g));
    grid->addWidget(MAKE_ICON(letter::gSmall));
    grid->addWidget(MAKE_ICON(letter::h));
    grid->addWidget(MAKE_ICON(letter::hSmall));
    grid->addWidget(MAKE_ICON(letter::i));
    grid->addWidget(MAKE_ICON(letter::iSmall));
    grid->addWidget(MAKE_ICON(letter::j));
    grid->addWidget(MAKE_ICON(letter::jSmall));
    grid->addWidget(MAKE_ICON(letter::k));
    grid->addWidget(MAKE_ICON(letter::kSmall));
    grid->addWidget(MAKE_ICON(letter::l));
    grid->addWidget(MAKE_ICON(letter::lSmall));
    grid->addWidget(MAKE_ICON(letter::m));
    grid->addWidget(MAKE_ICON(letter::mSmall));
    grid->addWidget(MAKE_ICON(letter::n));
    grid->addWidget(MAKE_ICON(letter::nSmall));
    grid->addWidget(MAKE_ICON(letter::o));
    grid->addWidget(MAKE_ICON(letter::oSmall));
    grid->addWidget(MAKE_ICON(letter::p));
    grid->addWidget(MAKE_ICON(letter::pSmall));
    grid->addWidget(MAKE_ICON(letter::q));
    grid->addWidget(MAKE_ICON(letter::qSmall));
    grid->addWidget(MAKE_ICON(letter::r));
    grid->addWidget(MAKE_ICON(letter::rSmall));
    grid->addWidget(MAKE_ICON(letter::s));
    grid->addWidget(MAKE_ICON(letter::sSmall));
    grid->addWidget(MAKE_ICON(letter::t));
    grid->addWidget(MAKE_ICON(letter::tSmall));
    grid->addWidget(MAKE_ICON(letter::u));
    grid->addWidget(MAKE_ICON(letter::uSmall));
    grid->addWidget(MAKE_ICON(letter::v));
    grid->addWidget(MAKE_ICON(letter::vSmall));
    grid->addWidget(MAKE_ICON(letter::w));
    grid->addWidget(MAKE_ICON(letter::wSmall));
    grid->addWidget(MAKE_ICON(letter::x));
    grid->addWidget(MAKE_ICON(letter::xSmall));
    grid->addWidget(MAKE_ICON(letter::y));
    grid->addWidget(MAKE_ICON(letter::ySmall));
    grid->addWidget(MAKE_ICON(letter::z));
    grid->addWidget(MAKE_ICON(letter::zSmall));
    makeSeparator();
    grid->addWidget(new QLabel(tr("Colorized:")), row, incCol());
    makeSeparator();
    grid->addWidget(MAKE_ICON(traceColorized));
    grid->addWidget(MAKE_ICON(bugColorized));
    grid->addWidget(MAKE_ICON(infoColorized));
    grid->addWidget(MAKE_ICON(warningColorized));
    grid->addWidget(MAKE_ICON(errorColorized));
    grid->addWidget(MAKE_ICON(fatalColorized));
    grid->addWidget(MAKE_ICON(collapsedColorized));
    grid->addWidget(MAKE_ICON(uncollapsedColorized));
    grid->addWidget(MAKE_ICON(pluginSettings));
    grid->addWidget(MAKE_ICON(letter::eColorized));
    grid->addWidget(MAKE_ICON(letter::eSmallColorized));
}

void
GtIconBrowser::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        e->accept();
        accept();
    }
    return GtDialog::keyPressEvent(e);
}
