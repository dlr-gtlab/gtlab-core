/* GTlab - Gas Turbine laboratory
 * Source File: gt_state.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_variantproperty.h"
#include "gt_coreapplication.h"
#include "gt_object.h"

#include "gt_state.h"

GtState::GtState(const QString& id, const QString& path, const QVariant& initVal,
                 GtObject* guardian, GtObject* parent) :
    GtObject(parent),
    m_guard(guardian),
    m_path(path)
{
    m_property = new GtVariantProperty(id, QString(), GtUnit::Category::None,
                                       initVal);

    registerProperty(*m_property);

    connect(m_property, SIGNAL(changed()), SLOT(onPropertyChange()));
    connect(m_guard.data(), SIGNAL(destroyed(QObject*)),
            SLOT(deleteLater()));
}

void
GtState::onPropertyChange()
{
    emit valueChanged(this);
    emit valueChanged(m_property->get());
}

GtState::~GtState()
{
    delete m_property;
}

void
GtState::setValue(const QVariant& val, bool undoCommand)
{
    if (undoCommand && isValid())
    {
        gtApp->propertyCommand(this, m_property, val);
    }
    else
    {
        m_property->setVal(val);
    }
}

QVariant
GtState::getValue() const
{
    return m_property->getVal();
}

bool
GtState::isValid() const
{
    if (!m_guard)
    {
        return false;
    }

    if (m_property->objectName().isEmpty())
    {
        return false;
    }

    return true;
}

void
GtState::setId(const QString& val)
{
    QString oldId = m_property->objectName();

    if (oldId != val)
    {
        m_property->setObjectName(val);
    }

    setValue(getValue(), false);

    emit idChanged(oldId, val);
}

QString
GtState::id()
{
    return m_property->objectName();
}

const QString&
GtState::path() const
{
    return m_path;
}

GtVariantProperty*
GtState::property()
{
    return m_property;
}
