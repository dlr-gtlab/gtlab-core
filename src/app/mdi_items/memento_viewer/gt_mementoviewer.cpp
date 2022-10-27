/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>

#include "gt_mementoviewer.h"
#include "gt_icons.h"
#include "gt_mementoeditor.h"
#include "gt_xmlhighlighter.h"
#include "gt_logging.h"

GtMementoViewer::GtMementoViewer()
{
    setObjectName("Memento Viewer");

    GtMementoEditor* editor = new GtMementoEditor(widget());
    editor->setReadOnly(true);

    m_highlighter = new GtXmlHighlighter(editor->document());

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    widget()->setLayout(lay);

    lay->addWidget(editor);
}

GtMementoViewer::~GtMementoViewer()
{
}

QIcon
GtMementoViewer::icon() const
{
    return gt::gui::icon::clone16();
}

bool
GtMementoViewer::allowsMultipleInstances() const
{
    return true;
}

void
GtMementoViewer::onThemeChanged()
{
    m_highlighter->onThemeChanged();
}
