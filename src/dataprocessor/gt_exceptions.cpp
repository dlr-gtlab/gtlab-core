#include <QTextStream>

#include "gt_exceptions.h"


GTlabException::GTlabException(
        const QString& where, const QString& what) :
    m_where(where),
    m_what(what)
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
        << QStringLiteral("MSG:") << QStringLiteral("\t") << m_what << Qt::endl;

    return msg;
}

void
GTlabException::pushWhere(QString const& function, QString const& name)
{
    if (name.isEmpty())
    {
        m_where = function + QStringLiteral("-->") + m_where;
    }
    else
    {
        m_where = function + QStringLiteral("@") +
                name + QStringLiteral("-->") + m_where;
    }
}

void
GTlabException::setWhere(QString const& where)
{
    m_where = where;
}

QString
GTlabException::where() const
{
    return m_where;
}

QString
GTlabException::what()
{
    return m_what;
}



