/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectuiaction.h"

GtObjectUIAction::GtObjectUIAction()
{

}

GtObjectUIAction::GtObjectUIAction(const QString& txt,
                                   const QString& mthd,
                                   const QString& icn,
                                   const QString& ver,
                                   const QString& visible,
                                   const QKeySequence& shortcut) :
    m_txt(txt),
    m_icon(icn),
    m_method(mthd),
    m_ver(ver),
    m_visible(visible),
    m_shortCut(shortcut)
{

}

const QString&
GtObjectUIAction::text() const
{
    return m_txt;
}

const QString&
GtObjectUIAction::icon() const
{
    return m_icon;
}

const QString&
GtObjectUIAction::method() const
{
    return m_method;
}

const QString&
GtObjectUIAction::verificationMethod() const
{
    return m_ver;
}

const QString&
GtObjectUIAction::visibilityMethod() const
{
    return m_visible;
}

const QKeySequence&
GtObjectUIAction::shortCut() const
{
    return m_shortCut;
}

