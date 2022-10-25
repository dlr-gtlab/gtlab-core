/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCHANGELOGWIDGET_H
#define GTCHANGELOGWIDGET_H

#include <QWidget>
#include "gt_gui_exports.h"
#include "gt_changelogparser.h"

class GtTextEdit;

class GT_GUI_EXPORT GtChangeLogWidget : public QWidget
{
public:
    explicit GtChangeLogWidget(QString const& mdContent,
                               QWidget* parent = nullptr);
private:
    GtTextEdit* m_textEdit;

    GtChangeLogParser m_parser;
};

#endif // GTCHANGELOGWIDGET_H
