/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFont>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>

#include "gt_propertyitem.h"
#include "gt_abstractproperty.h"
#include "gt_propertymodel.h"
#include "gt_stringproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_application.h"
#include "gt_propertyvaluedelegate.h"
#include "gt_session.h"
#include "gt_regularexpression.h"
#include "gt_icons.h"
#include "gt_command.h"
#include "gt_colors.h"

GtPropertyItem::GtPropertyItem() :
    m_currentUnit(QString())
{
}

QVariant
GtPropertyItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return {};
    }

    if (!m_property)
    {
        return {};
    }

    switch (role)
    {
        case Qt::DisplayRole:
            if (column == 0)
            {
                return m_property->objectName();
            }
            else if (column == 2)
            {
                return m_property->valueToVariant(m_currentUnit);
            }
            else if (column == 1)
            {
                if (m_currentUnit.isEmpty())
                {
                    QString tmpStr = m_property->siUnit();

                    if (tmpStr.isEmpty())
                    {
                        return QString();
                    }

                    return QStringLiteral("[") + tmpStr +
                           QStringLiteral("]");
                }

                return QStringLiteral("[") + m_currentUnit +
                       QStringLiteral("]");

            }

            break;


        case Qt::EditRole:
            if (!m_property->isReadOnly())
            {
                if (column == 2)
                {
                    return m_property->valueToVariant(m_currentUnit).toString();
                }
                else if (column == 1)
                {
                    if (m_currentUnit.isEmpty())
                    {
                        return m_property->siUnit();
                    }

                    return m_currentUnit;
                }
            }

            break;

        case Qt::ForegroundRole:
            if (!m_property->isActive() || 
                (m_property->isReadOnly() && column == 2))
            {
                return gt::gui::color::disabled();
            }
        break;

        case Qt::DecorationRole:
            if (m_property->isConnected())
            {
                if (column == 0)
                {
                    return gt::gui::icon::chain();
                }
            }

            break;

        case Qt::CheckStateRole:
            if (column == 0)
            {
                if (m_property->isOptional())
                {
                    if (m_property->isActive())
                    {
                        return Qt::Checked;
                    }

                    return Qt::Unchecked;
                }
            }
            else if (column == 2)
            {
                QVariant var = m_property->valueToVariant();

                if (var.type() == QVariant::Bool)
                {
                    if (var.toBool())
                    {
                        return Qt::Checked;
                    }

                    return Qt::Unchecked;
                }
            }

            break;

        case Qt::ToolTipRole:
            if (column == 0)
            {
                return m_property->brief();
            }
            else if (column == 1)
            {
                return m_property->unitCategoryToString();
            }

            break;

        case Qt::FontRole:
            if (column == 1)
            {
                QFont font;
                font.setItalic(true);

                return font;
            }

            break;

        case GtPropertyModel::UnitCategoryRole:
            return QVariant::fromValue(m_property->unitCategory());

        case GtPropertyModel::ReadOnlyRole:
            return m_property->isReadOnly();

        case GtPropertyModel::OptionalRole:
            return m_property->isOptional();

        case GtPropertyModel::ActiveRole:
            return m_property->isActive();
        case GtPropertyModel::DefaultCollapseRole:
            return m_property->collapsedByDefault();
    }

    return {};
}

bool
GtPropertyItem::setData(int column, const QVariant& value, GtObject* obj,
                        int role)
{
    if (!m_property)
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            if (column == 2)
            {
                if (!m_property->isReadOnly())
                {
                    // TODO: check whether property can be changed

                    gt::propertyItemChange(*obj, *m_property, value,
                                           m_currentUnit);

                    return true;
                }
            }
            else if (column == 1)
            {
                m_currentUnit = value.toString();
                return true;
            }

            break;
        }

        case Qt::CheckStateRole:
        {
            if (column == 0)
            {
                if (m_property->isOptional())
                {
                    bool checked = value.toBool();
                    m_property->setActive(checked);
                }
            }
            else if (column == 2)
            {
                QVariant var = m_property->valueToVariant();

                if (var.type() == QVariant::Bool)
                {
                    gt::propertyItemChange(*obj, *m_property, value,
                                           m_currentUnit);

                    return true;
                }
            }

            break;
        }
    }

    return false;
}

QWidget*
GtPropertyItem::editorWidget(QWidget* parent,
                             const GtPropertyValueDelegate* delegate) const
{
    QVariant var = m_property->valueToVariant();

    switch (var.type())
    {
        case QVariant::Double:
        {
            auto* lineEdit = new QLineEdit(parent);
            lineEdit->setValidator(new QRegularExpressionValidator(
                                       gt::rex::forDoubles(), lineEdit));
            return lineEdit;
        }

        case QVariant::Int:
        {
            auto* spinbox = new QSpinBox(parent);
            spinbox->setRange(std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max());
            connect(spinbox, SIGNAL(editingFinished()),
                    delegate, SLOT(onDataChange()));
            return spinbox;
        }

        case QVariant::String:
        {
            auto* lineEdit = new QLineEdit(parent);
            QValidator* validator = nullptr;
            auto* s = qobject_cast<GtStringProperty*>(m_property);
            auto p = qobject_cast<GtObjectPathProperty*>(m_property);

            if (s)
            {
                validator = s->validator();
            }
            else if (p)
            {
                validator = nullptr;
            }
            else
            {
                validator = new QRegularExpressionValidator(
                    gt::rex::forExpressions(), lineEdit);
            }

            lineEdit->setValidator(validator);

            return lineEdit;
        }

        default:
            return GtAbstractPropertyItem::editorWidget(parent, delegate);
    }

    return nullptr;
}

void
GtPropertyItem::setEditorData(QWidget* editor, QVariant& var) const
{
    if (!editor)
    {
        return;
    }

    switch (var.type())
    {
        case QVariant::Int:
        {
            auto* spinbox = static_cast<QSpinBox*>(editor);
            spinbox->setValue(var.toInt());
            return;
        }

        default:
            GtAbstractPropertyItem::setEditorData(editor, var);
            break;
    }
}

void
GtPropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model,
                             const QModelIndex& index) const
{
    QVariant var = m_property->valueToVariant();

    switch (var.type())
    {
        case QVariant::Int:
        {
            auto* spinbox = static_cast<QSpinBox*>(editor);
            model->setData(index, spinbox->value(), Qt::EditRole);
            break;
        }

        default:
            GtAbstractPropertyItem::setModelData(editor, model, index);
            break;
    }
}

void
GtPropertyItem::onPropertyChange()
{
    m_model->updatePropertyData(this);
}

QString
gt::propertyItemCommandString(const QString& objName,
                              const QString& propertyName,
                              const QString& commandStr)
{
    return  {
        objName + QStringLiteral(" - ") + propertyName +
        QStringLiteral(" ") + commandStr
    };
}

bool
gt::propertyItemChange(GtObject& obj,
                       GtAbstractProperty& property,
                       const QVariant& value,
                       const QString& unit)
{
    auto* root = obj.findRoot<GtSession*>();

    if (!root)
    {
        return property.setValueFromVariant(value, unit);
    }

    if (value == property.valueToVariant(unit))
    {
        return false;
    }

    auto cmd = gtApp->makeCommand(&obj,
                                  gt::propertyItemCommandString(
                                      obj.objectName(), property.objectName(),
                                      QObject::tr("changed")));
    Q_UNUSED(cmd)

    return property.setValueFromVariant(value, unit);
}
