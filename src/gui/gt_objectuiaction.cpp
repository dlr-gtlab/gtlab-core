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

#include <QKeySequence>

struct GtObjectUIAction::Impl
{
    explicit
    Impl(QString name_ = {}, InvokableActionMethod method_ = {}) :
        name(std::move(name_)),
        method(std::move(method_))
    { }

    /// Action text
    QString name{};

    /// Action icon
    QIcon icon{};

    /// Invokable method
    InvokableActionMethod method{};

    /// Verification method
    InvokableVerificationMethod verification{};

    /// Visibility method
    InvokableVisibilityMethod visibility{};

    /// Shortcut
    QKeySequence shortCut{};

    /// order priority
    int priority{gt::gui::OrderPriority::Default};

    /**
     * @brief helper function to set action method suing the name of a
     * invokable method
     * @param methodName
     */
    void setActionMethod(const QString& methodName)
    {
        method = fromMethodName(methodName);
    }
};

GtObjectUIAction::InvokableActionMethod
GtObjectUIAction::fromMethodName(const QString& methodName)
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

GtObjectUIAction::GtObjectUIAction() noexcept :
    pimpl(std::make_unique<Impl>())
{ }

GtObjectUIAction::GtObjectUIAction(QString name,
                                   ActionMethod method) :
    GtObjectUIAction(std::move(name),
                     [m = std::move(method)](QObject* parent, GtObject* target){
        Q_UNUSED(parent)
        if (m) m(target);
    })
{ }

GtObjectUIAction::GtObjectUIAction(QString name,
                                   InvokableActionMethod method) :
    pimpl(std::make_unique<Impl>(std::move(name), std::move(method)))
{ }

GtObjectUIAction::GtObjectUIAction(GtObjectUIAction const& o) noexcept :
    pimpl(std::make_unique<Impl>(*o.pimpl))
{ }

GtObjectUIAction::GtObjectUIAction(GtObjectUIAction&& o) noexcept :
    pimpl(std::make_unique<Impl>(std::move(*o.pimpl)))
{ }

GtObjectUIAction&
GtObjectUIAction::operator=(GtObjectUIAction const& o) noexcept
{
    GtObjectUIAction tmp{o};
    swap(tmp);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::operator=(GtObjectUIAction&& o) noexcept
{
    GtObjectUIAction tmp{std::move(o)};
    swap(tmp);
    return *this;
}

GtObjectUIAction::~GtObjectUIAction() noexcept = default;

bool
GtObjectUIAction::isEmpty() const
{
    return pimpl->name.isEmpty();
}

bool
GtObjectUIAction::isSeparator() const
{
    return isEmpty();
}

const QString&
GtObjectUIAction::name() const
{
    return pimpl->name;
}

const QIcon&
GtObjectUIAction::icon() const
{
    return pimpl->icon;
}

const GtObjectUIAction::InvokableActionMethod&
GtObjectUIAction::method() const
{
    return pimpl->method;
}

const GtObjectUIAction::InvokableVerificationMethod&
GtObjectUIAction::verificationMethod() const
{
    return pimpl->verification;
}

const GtObjectUIAction::InvokableVisibilityMethod&
GtObjectUIAction::visibilityMethod() const
{
    return pimpl->visibility;
}

const QKeySequence&
GtObjectUIAction::shortCut() const
{
    return pimpl->shortCut;
}

int
GtObjectUIAction::orderPriority() const
{
    return pimpl->priority;
}

GtObjectUIAction&
GtObjectUIAction::setIcon(const QIcon& icon)
{
    pimpl->icon = icon;
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
    pimpl->verification = std::move(method);
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
        pimpl->verification = nullptr;
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
    pimpl->visibility = std::move(method);
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
        pimpl->visibility = nullptr;
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
    pimpl->shortCut = shortCut;
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::registerShortCut(const QString& id,
                                   const QString& cat,
                                   const QKeySequence& k,
                                   bool readOnly)
{
    gtApp->extendShortCuts({id, cat, k.toString(), readOnly});
    pimpl->shortCut = gtApp->getShortCutSequence(id, cat);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setOrderPriority(int priority)
{
    pimpl->priority = priority;
    return *this;
}

void
GtObjectUIAction::swap(GtObjectUIAction& other) noexcept
{
    std::swap(pimpl->name, other.pimpl->name);
    std::swap(pimpl->icon, other.pimpl->icon);
    std::swap(pimpl->method, other.pimpl->method);
    std::swap(pimpl->verification, other.pimpl->verification);
    std::swap(pimpl->visibility, other.pimpl->visibility);
    std::swap(pimpl->shortCut, other.pimpl->shortCut);
    std::swap(pimpl->priority, other.pimpl->priority);
}
