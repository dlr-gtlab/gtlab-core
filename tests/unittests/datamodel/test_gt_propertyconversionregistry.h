/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_GT_PROPERTYCONVERSIONREGISTRY_H
#define TEST_GT_PROPERTYCONVERSIONREGISTRY_H

#include "gt_abstractproperty.h"

// Minimal GtAbstractProperty subclasses with unique class names used to
// register isolated type pairs in the global GtPropertyConversionRegistry
// without interfering with the production registrations.

class GtTestRegistryPropA : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropA() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropB : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropB() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropC : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropC() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropD : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropD() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropE : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropE() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropF : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropF() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropG : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropG() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropH : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropH() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

class GtTestRegistryPropI : public GtAbstractProperty
{
    Q_OBJECT

public:
    GtTestRegistryPropI() = default;

    QVariant valueToVariant(const QString& unit, bool* success) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

#endif // TEST_GT_PROPERTYCONVERSIONREGISTRY_H
