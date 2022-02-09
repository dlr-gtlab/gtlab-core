/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QColor>
#include <QFont>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleValidator>

#include "gt_propertyitem.h"
#include "gt_abstractproperty.h"
#include "gt_propertymodel.h"
#include "gt_groupproperty.h"
#include "gt_modeproperty.h"
#include "gt_stringproperty.h"
#include "gt_propertychangecommand.h"
#include "gt_application.h"
#include "gt_propertyvaluedelegate.h"
#include "gt_session.h"
#include "gt_regexp.h"

GtPropertyItem::GtPropertyItem() :
    m_currentUnit(QString())
{
}

GtPropertyItem::~GtPropertyItem()
{
}

QVariant
GtPropertyItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return QVariant();
    }

    if (m_property == Q_NULLPTR)
    {
        return QVariant();
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
                else
                {
                    return QStringLiteral("[") + m_currentUnit +
                           QStringLiteral("]");
                }
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
                    else
                    {
                        return m_currentUnit;
                    }
                }
            }

            break;

        case Qt::DecorationRole:
            if (m_property->isConnected())
            {
                if (column == 0)
                {
                    return gtApp->icon(QStringLiteral("chainIcon_16.png"));
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
                    else
                    {
                        return Qt::Unchecked;
                    }
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
                    else
                    {
                        return Qt::Unchecked;
                    }
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

//        case Qt::ForegroundRole:
//            if (!m_property->isActive())
//            {
//                return QColor(120, 120, 120);
//            }

//            break;

//        case Qt::BackgroundRole:
//            if (column == 2)
//            {
//                if (!m_property->isActive() || m_property->isReadOnly())
//                {
//                    return QColor(245, 245, 245);
//                }
//                else
//                {
//                    return QColor(255, 255, 255);
//                }
//            }

//            break;

        case Qt::FontRole:
            if (column == 1)
            {
                QFont font;
                font.setItalic(true);

                return font;
            }

            break;

        case GtPropertyModel::UnitCategoryRole:
            return qVariantFromValue(m_property->unitCategory());

        case GtPropertyModel::ReadOnlyRole:
            return m_property->isReadOnly();

        case GtPropertyModel::OptionalRole:
            return m_property->isOptional();

        case GtPropertyModel::ActiveRole:
            return m_property->isActive();
    }

    return QVariant();
}

bool
GtPropertyItem::setData(int column, const QVariant& value, GtObject* obj,
                        int role)
{
    if (m_property == Q_NULLPTR)
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            if (!m_property->isReadOnly())
            {
                if (column == 2)
                {
                    // TODO: check whether property can be changed
                    GtSession* root =  obj->findRoot<GtSession*>();

                    if (root != Q_NULLPTR)
                    {
                        if (value != m_property->valueToVariant(m_currentUnit))
                        {
                            gtApp->propertyCommand(obj, m_property, value,
                                                   m_currentUnit, root);
                        }
                    }
                    else
                    {
                        m_property->setValueFromVariant(value, m_currentUnit);
                    }

                    return true;
                }

                if (column == 1)
                {
                    m_currentUnit = value.toString();
                    return true;
                }
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
                    GtSession* root =  obj->findRoot<GtSession*>();

                    if (root != Q_NULLPTR)
                    {
                        gtApp->propertyCommand(obj, m_property, value,
                                               m_currentUnit, root);
                    }
                    else
                    {
                        m_property->setValueFromVariant(value, m_currentUnit);
                    }

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
            QLineEdit* lineEdit = new QLineEdit(parent);
            QDoubleValidator* validator = new QDoubleValidator(lineEdit);
            lineEdit->setValidator(validator);
            return lineEdit;
        }

        case QVariant::Int:
        {
            QSpinBox* spinbox = new QSpinBox(parent);
            spinbox->setRange(std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max());
            connect(spinbox, SIGNAL(valueChanged(int)),
                    delegate, SLOT(onDataChange()));
            return spinbox;
        }

        case QVariant::String:
        {
            QLineEdit* lineEdit = new QLineEdit(parent);
            QValidator* validator = nullptr;
            GtStringProperty* s = qobject_cast<GtStringProperty*>(m_property);

            if (s != nullptr)
            {
                validator = s->validator();
            }
            else
            {
                validator = new QRegExpValidator(GtRegExp::forExpressions(),
                                                 lineEdit);
            }

            lineEdit->setValidator(validator);

            return lineEdit;
        }

        default:
            return GtAbstractPropertyItem::editorWidget(parent, delegate);
    }

    return Q_NULLPTR;
}

void
GtPropertyItem::setEditorData(QWidget* editor, QVariant& var) const
{
    if (editor == Q_NULLPTR)
    {
        return;
    }

    switch (var.type())
    {
        case QVariant::Int:
        {
            QSpinBox* spinbox = static_cast<QSpinBox*>(editor);
            spinbox->setValue(var.toInt());
            return;
        }

        default:
            GtAbstractPropertyItem::setEditorData(editor, var);
            break;
    }

    return;
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
            QSpinBox* spinbox = static_cast<QSpinBox*>(editor);
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
