/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    GtCodeEditor* m_codeEditor;

};

#endif // GTLINENUMBERAREA_H
