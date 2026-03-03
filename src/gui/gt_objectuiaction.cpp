/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectuiaction.h"

#include "gt_application.h"
#include "gt_icons.h"
#include "gt_logging.h"
#include "gt_utilities.h"

GtObjectUIAction::GtObjectUIAction() = default;

GtObjectUIAction::InvokableActionMethod
GtObjectUIAction::fromMethodName(const QString &methodName)
{
    if (methodName.isEmpty())
    {
        return nullptr;
    }

    // wrap meta method call in lambda
    return [=](QObject* parent, GtObject* target) {
        if (!QMetaObject::invokeMethod(parent, methodName.toLatin1(),
                                       Q_ARG(GtObject*, target)))
        {
            gtWarning().nospace()
            << QObject::tr("Could not invoke method!")
            << " (" << methodName << ")";
        }
    };
}

GtObjectUIAction::GtObjectUIAction(const QString& text,
                                   const QString& method,
                                   const QString& icon,
                                   const QString& verification,
                                   const QString& visibility,
                                   const QKeySequence& shortcut) :
    m_text(text),
    m_icon(gt::gui::getIcon(icon)),
    m_shortCut(shortcut)
{
    setActionMethod(method);
    setVerificationMethod(verification);
    setVisibilityMethod(visibility);
}

GtObjectUIAction::GtObjectUIAction(const QString& text,
                                   ActionMethod method) :
    GtObjectUIAction(text,
                     [m = std::move(method)](QObject* parent, GtObject* target){
        Q_UNUSED(parent)
        if (m) m(target);
    })
{ }

GtObjectUIAction::GtObjectUIAction(const QString& text,
                                   InvokableActionMethod method) :
    m_text(text),
    m_method(std::move(method))
{

}

bool
GtObjectUIAction::isEmpty() const
{
    return m_text.isEmpty();
}

const QString&
GtObjectUIAction::text() const
{
    return m_text;
}

const QIcon&
GtObjectUIAction::icon() const
{
    return m_icon;
}

const GtObjectUIAction::InvokableActionMethod&
GtObjectUIAction::method() const
{
    return m_method;
}

const GtObjectUIAction::InvokableVerificationMethod&
GtObjectUIAction::verificationMethod() const
{
    return m_verification;
}

const GtObjectUIAction::InvokableVisibilityMethod&
GtObjectUIAction::visibilityMethod() const
{
    return m_visibility;
}

const QKeySequence&
GtObjectUIAction::shortCut() const
{
    return m_shortCut;
}

GtObjectUIAction&
GtObjectUIAction::setIcon(const QIcon& icon)
{
    m_icon = icon;
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setIcon(const QString& icon)
{
    return setIcon(gt::gui::getIcon(icon));
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(InvokableVerificationMethod method)
{
    m_verification = std::move(method);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(VerificationMethod method)
{
    // parent object not needed here
    return setVerificationMethod([m = std::move(method)]
                                 (QObject* /*parent*/, GtObject* target){
        return m && m(target);
    });
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(const QString& methodName)
{
    if (methodName.isEmpty())
    {
        m_verification = nullptr;
        return *this;
    }

    // wrap meta method call in lambda
    return setVerificationMethod([=](QObject* parent, GtObject* target) {
        bool verified = false;
        if (!QMetaObject::invokeMethod(parent, methodName.toLatin1(),
                                       Q_RETURN_ARG(bool, verified),
                                       Q_ARG(GtObject*, target)))
        {
            gtWarning()
                << QObject::tr("Could not invoke verification method!")
                << gt::brackets(methodName);
        }

        return verified;
    });
}

GtObjectUIAction&
GtObjectUIAction::setEnabled(bool enabled)
{
    return setVerificationMethod([=](GtObject*){ return enabled; });
}

GtObjectUIAction&
GtObjectUIAction::setVisibilityMethod(InvokableVisibilityMethod method)
{
    m_visibility = std::move(method);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setVisibilityMethod(VisibilityMethod method)
{
    // parent object not needed here
    return setVisibilityMethod([m = std::move(method)]
                               (QObject* /*parent*/, GtObject* target){
        return m && m(target);
    });
}

GtObjectUIAction&
GtObjectUIAction::setVisibilityMethod(const QString& methodName)
{
    if (methodName.isEmpty())
    {
        m_visibility = nullptr;
        return *this;
    }

    // wrap meta method call in lambda
    return setVisibilityMethod([=](QObject* parent, GtObject* target) {
        bool visible = false;
        if (!QMetaObject::invokeMethod(parent, methodName.toLatin1(),
                                       Q_RETURN_ARG(bool, visible),
                                       Q_ARG(GtObject*, target)))
        {
            gtWarning()
                << QObject::tr("Could not invoke visibility method!")
                << gt::brackets(methodName);
        }
        return visible;
    });
}

GtObjectUIAction&
GtObjectUIAction::setVisible(bool visible)
{
    return setVisibilityMethod([=](GtObject*){ return visible; });
}

GtObjectUIAction&
GtObjectUIAction::setShortCut(const QKeySequence& shortCut)
{
    m_shortCut = shortCut;
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::registerShortCut(const QString& id,
                                   const QString& cat,
                                   const QKeySequence& k,
                                   bool readOnly)
{
    gtApp->extendShortCuts({id, cat, k, readOnly});
    m_shortCut = gtApp->getShortCutSequence(id, cat);
    return *this;
}

void
GtObjectUIAction::setActionMethod(const QString& methodName)
{
    m_method = fromMethodName(methodName);
}

