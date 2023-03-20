/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_changelogparser.h"
#include <QStringList>
#include <QRegularExpressionMatch>
#include <QRegularExpression>

#include "gt_regexp.h"

GtChangeLogParser::GtChangeLogParser(const QString& fileContent)
{
    parse(fileContent);
}

void
GtChangeLogParser::parse(const QString& fileContent)
{
    if (fileContent.isEmpty())
    {
        return;
    }

    QList<int> occurances = releaseNumberOccurances(fileContent);

    if (occurances.isEmpty())
    {
        return;
    }

    if (occurances.size() == 1)
    {
        QString content = fileContent.mid(occurances.first());


        m_changeLog.append(GtChangeLogEntry{content});
        return;
    }

    for (int k = 0; k < occurances.size() - 1; ++k)
    {
        QString content = fileContent.mid(occurances[k],
                                          occurances[k + 1] - occurances[k]);
        m_changeLog.append(GtChangeLogEntry{content});
    }

    QString content = fileContent.mid(occurances.last());
    m_changeLog.append(GtChangeLogEntry{content});

    std::sort(m_changeLog.begin(),
              m_changeLog.end(), [] (GtChangeLogEntry const& x,
              GtChangeLogEntry const& y) { return x.version() < y.version(); });
}

GtChangeLogEntry
GtChangeLogParser::currentReleaseMD() const
{
    if (m_changeLog.isEmpty())
    {
        return GtChangeLogEntry{GtVersionNumber{0}, ""};
    }

    if (m_changeLog.size() == 1)
    {
        return m_changeLog.first();
    }

    return m_changeLog.last();
}

QList<GtChangeLogEntry>
GtChangeLogParser::allReleasesDM() const
{
    return m_changeLog;
}

QString
GtChangeLogParser::contentOfVersion(const GtVersionNumber& v) const
{
    auto iter = std::find_if(std::begin(m_changeLog), std::end(m_changeLog),
                             [v](const GtChangeLogEntry& e) {
        return e.version() == v;
    });

    if (iter != std::end(m_changeLog))
    {
        return iter->content();
    }
    else
    {
        return {};
    }
}

QList<GtVersionNumber>
GtChangeLogParser::versions() const
{
    QList<GtVersionNumber> retVal;
    for (GtChangeLogEntry const& e : m_changeLog)
    {
        retVal.prepend(e.version());
    }
    return retVal;
}



QList<int>
GtChangeLogParser::releaseNumberOccurances(const QString& fileContent)
{
    /// finds expression for versionnumber
    /// including its formatting for maarkdown
    QRegExp e("##\\s\\[([0-9].){1,3}");

    QList<int> retVal;
    int i = 0;

    bool first = true;
    while (i >= 0)
    {
        if (first)
        {
            i = fileContent.indexOf(e, 0);
            first = false;
        }
        else
        {
            i = fileContent.indexOf(e, i + 1);
        }

        if (i >= 0)
        {
            retVal.append(i);
        }
    }

    return retVal;
}

GtVersionNumber
GtChangeLogEntry::versionFromMD(const QString& content)
{
    /// to find the version number exactly
    /// to get the GtVersionnumber based on it.
    static QRegularExpression e(QString(R"(\[(%1)\])")
                                    .arg(gt::re::forSemVers().pattern()));

    QRegularExpressionMatch match = e.match(content);
    if (match.hasMatch())
    {
        return GtVersionNumber(match.captured(1));
    }
    return GtVersionNumber{};

}

QDate
GtChangeLogEntry::dateFromMD(const QString &content)
{
    /// to find the version number exactly
    /// to get the GtVersionnumber based on it.
    static QRegularExpression e("[0-9]{4}-[0-9]{1,}-[0-9]{1,}");
    QRegularExpressionMatch match = e.match(content);
    QString v = match.captured();
    QStringList dateStringList = v.split("-");
    if (dateStringList.size() < 3)
    {
        return {};
    }
    int y = dateStringList.at(0).toInt();
    int m = dateStringList.at(1).toInt();
    int d = dateStringList.at(2).toInt();

    QDate date(y, m, d);

    return date;
}

GtChangeLogEntry::GtChangeLogEntry(const GtVersionNumber& v,
                                   const QString& c,
                                   const QDate& d) :
    m_version(v),
    m_content(c),
    m_date(d)
{

}

GtChangeLogEntry::GtChangeLogEntry(const QString& mdContent) :
    m_version(versionFromMD(mdContent)),
    m_date(dateFromMD(mdContent))
{
    int firstContent = mdContent.indexOf("###");

    m_content = mdContent.mid(firstContent);
}

QString
GtChangeLogEntry::toMarkDownText() const
{
    /// start the line as a header line and add version
    QString titleLine = "## Version " + m_version.toString();

    if (!m_date.isNull())
    {
        /// add the date to the header line
        titleLine += " (released on " + m_date.toString(Qt::ISODate) + ")";
    }

    /// add empty line between header and content;
    titleLine += QString("&nbsp;     \r") ;

    /// combine header line and content
    return titleLine + QString("&nbsp;     \r") + m_content;
}

const GtVersionNumber&
GtChangeLogEntry::version() const
{
    return m_version;
}

const QString&
GtChangeLogEntry::content() const
{
    return m_content;
}

const QDate&
GtChangeLogEntry::date() const
{
    return m_date;
}
