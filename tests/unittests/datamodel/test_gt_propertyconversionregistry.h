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
    GtTestRegistryPropA() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropB() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropC() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropD() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropE() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropF() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropG() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropH() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
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
    GtTestRegistryPropI() : GtAbstractProperty()
    {
    }

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override
    {
        Q_UNUSED(unit)
        if (success) *success = true;
        return QVariant();
    }

    GT_NO_DISCARD
    bool setValueFromVariant(const QVariant& val, const QString& unit) override
    {
        Q_UNUSED(val)
        Q_UNUSED(unit)
        return true;
    }
};

#endif // TEST_GT_PROPERTYCONVERSIONREGISTRY_H
