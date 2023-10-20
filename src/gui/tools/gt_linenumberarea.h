/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLINENUMBERAREA_H
#define GTLINENUMBERAREA_H

#include <QWidget>

class GtCodeEditor;

class GtLineNumberArea : public QWidget
{
public:
    explicit GtLineNumberArea(GtCodeEditor* editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GtCodeEditor* m_codeEditor;

};

#endif // GTLINENUMBERAREA_H
