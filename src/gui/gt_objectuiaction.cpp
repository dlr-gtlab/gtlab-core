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
    explicit Impl(QString name_ = {}, InvokableActionMethod method_ = {}) :
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

GtObjectUIAction::GtObjectUIAction() :
    pimpl(std::make_unique<Impl>())
{ }

GtObjectUIAction::GtObjectUIAction(QString name, ActionMethod method) :
    GtObjectUIAction(std::move(name), [m = std::move(method)](
                                          QObject* parent, GtObject* target) {
        Q_UNUSED(parent)
        if (m) m(target);
    })
{ }

GtObjectUIAction::GtObjectUIAction(QString name, InvokableActionMethod method) :
    pimpl(std::make_unique<Impl>(std::move(name), std::move(method)))
{ }

GtObjectUIAction::GtObjectUIAction(const GtObjectUIAction& o) :
    pimpl(std::make_unique<Impl>(*o.pimpl))
{ }

GtObjectUIAction::GtObjectUIAction(GtObjectUIAction&& o) :
    pimpl(std::make_unique<Impl>(std::move(*o.pimpl)))
{ }

GtObjectUIAction&
GtObjectUIAction::operator=(const GtObjectUIAction& o)
{
    GtObjectUIAction tmp{o};
    swap(tmp);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::operator=(GtObjectUIAction&& o)
{
    GtObjectUIAction tmp{std::move(o)};
    swap(tmp);
    return *this;
}

GtObjectUIAction::~GtObjectUIAction() = default;

bool
GtObjectUIAction::isEmpty() const noexcept
{
    return pimpl->name.isEmpty();
}

bool
GtObjectUIAction::isSeparator() const noexcept
{
    return isEmpty();
}

const QString&
GtObjectUIAction::name() const noexcept
{
    return pimpl->name;
}

const QIcon&
GtObjectUIAction::icon() const noexcept
{
    return pimpl->icon;
}

const GtObjectUIAction::InvokableActionMethod&
GtObjectUIAction::method() const noexcept
{
    return pimpl->method;
}

const GtObjectUIAction::InvokableVerificationMethod&
GtObjectUIAction::verificationMethod() const noexcept
{
    return pimpl->verification;
}

const GtObjectUIAction::InvokableVisibilityMethod&
GtObjectUIAction::visibilityMethod() const noexcept
{
    return pimpl->visibility;
}

const QKeySequence&
GtObjectUIAction::shortCut() const noexcept
{
    return pimpl->shortCut;
}

int
GtObjectUIAction::orderPriority() const noexcept
{
    return pimpl->priority;
}

GtObjectUIAction&
GtObjectUIAction::setIcon(const QIcon& newIcon)
{
    pimpl->icon = newIcon;
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setIcon(const QString& iconPath)
{
    return setIcon(gt::gui::getIcon(iconPath));
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(InvokableVerificationMethod verfiyMethod)
{
    pimpl->verification = std::move(verfiyMethod);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(VerificationMethod verifyMethod)
{
    // parent object not needed here
    return setVerificationMethod([m = std::move(verifyMethod)]
                                 (QObject* /*parent*/, GtObject* target){
        return m && m(target);
    });
}

GtObjectUIAction&
GtObjectUIAction::setVerificationMethod(const QString& verifyMethodName)
{
    if (verifyMethodName.isEmpty())
    {
        pimpl->verification = nullptr;
        return *this;
    }

    // wrap meta method call in lambda
    return setVerificationMethod([=](QObject* parent, GtObject* target) {
        bool verified = false;
        if (!QMetaObject::invokeMethod(parent, verifyMethodName.toLatin1(),
                                       Q_RETURN_ARG(bool, verified),
                                       Q_ARG(GtObject*, target)))
        {
            gtWarning()
                << QObject::tr("Could not invoke verification method!")
                << gt::brackets(verifyMethodName);
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
GtObjectUIAction::setVisibilityMethod(InvokableVisibilityMethod visMethod)
{
    pimpl->visibility = std::move(visMethod);
    return *this;
}

GtObjectUIAction&
GtObjectUIAction::setVisibilityMethod(VisibilityMethod visMethod)
{
    // parent object not needed here
    return setVisibilityMethod([m = std::move(visMethod)]
                               (QObject* /*parent*/, GtObject* target){
        return m && m(target);
    });
}

GtObjectUIAction&
GtObjectUIAction::setVisibilityMethod(const QString& visMethodName)
{
    if (visMethodName.isEmpty())
    {
        pimpl->visibility = nullptr;
        return *this;
    }

    // wrap meta method call in lambda
    return setVisibilityMethod([=](QObject* parent, GtObject* target) {
        bool visible = false;
        if (!QMetaObject::invokeMethod(parent, visMethodName.toLatin1(),
                                       Q_RETURN_ARG(bool, visible),
                                       Q_ARG(GtObject*, target)))
        {
            gtWarning()
                << QObject::tr("Could not invoke visibility method!")
                << gt::brackets(visMethodName);
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
GtObjectUIAction::setShortCut(const QKeySequence& newShortCut)
{
    pimpl->shortCut = newShortCut;
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
GtObjectUIAction::setOrderPriority(int priority) noexcept
{
    pimpl->priority = priority;
    return *this;
}

void
GtObjectUIAction::swap(GtObjectUIAction& other) noexcept
{
    using std::swap;
    swap(pimpl, other.pimpl);
}
