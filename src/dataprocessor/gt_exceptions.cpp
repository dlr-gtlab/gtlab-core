/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_exceptions.h
 *
 *  Created on: 28.10.2015
 *  Author: Richard Becker (AT-TWK)
 *  Tel.: +49 2203 601 3493
 */

#include <QTextStream>
#include <QObject>

#include "gt_exceptions.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace Qt
{
    static auto endl = ::endl;
}
#endif

GTlabException::GTlabException(const QString& where, const QString& what) :
    m_where(where.toStdString()),
    m_what(what.toStdString())
{
}

QString
GTlabException::errMsg() const
{
    QString msg;
    QTextStream out(&msg);

    out << Qt::endl
        << QObject::tr("GTlab - EXCEPTION EMERGED") << Qt::endl
        << QStringLiteral("---------------------") << Qt::endl
        << QStringLiteral("IN:")  << QStringLiteral("\t") << where() << Qt::endl
        << QStringLiteral("MSG:") << QStringLiteral("\t") << what() << Qt::endl;

    return msg;
}

void
GTlabException::pushWhere(QString const& function, QString const& name)
{
    if (name.isEmpty())
    {
        m_where = QString("%1-->%2").arg(function, where()).toStdString();
    }
    else
    {
        m_where = QString("%1@%2-->%3").arg(function, name, where())
                      .toStdString();
    }
}

void
GTlabException::setWhere(QString const& where) noexcept
{
    m_where = where.toStdString();
}

QString
GTlabException::where() const noexcept
{
    return m_where.c_str();
}

const char *
GTlabException::what() const noexcept
{
    return m_what.c_str();
}
