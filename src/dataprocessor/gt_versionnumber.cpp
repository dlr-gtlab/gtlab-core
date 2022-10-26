/* GTlab - Gas Turbine laboratory
 * Source File: gt_versionnumber.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 05.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <regex>
#include <string>
#include <cctype>
#include <sstream>

#include <QVersionNumber>

#include "gt_regexp.h"

#include "gt_versionnumber.h"

namespace {

bool isNumber(const std::string& str)
{
    auto iter = str.begin();
    while (iter != str.end() && std::isdigit(*iter)) ++iter;
    return !str.empty() && iter == str.end();
}

std::vector<std::string> split(const std::string& s,
                               char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

template<class InputIt1, class InputIt2>
bool lexicographicalCompare(InputIt1 first1, InputIt1 last1,
                            InputIt2 first2, InputIt2 last2)
{
    for ( ; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2 )
    {
        if (*first1 < *first2)
        {
            return true;
        }
        if (*first2 < *first1)
        {
            return false;
        }
    }
    return (first1 == last1) && (first2 != last2);
}

bool comparePreleaseField(const std::string& pr1,
                          const std::string& pr2)
{
    if (QString::fromStdString(pr1).startsWith("dp", Qt::CaseInsensitive))
    {
        if (!QString::fromStdString(pr2).startsWith("dp", Qt::CaseInsensitive))
        {
            return true;
        }
    }
    else
    {
        if (QString::fromStdString(pr2).startsWith("dp", Qt::CaseInsensitive))
        {
            return false;
        }
    }

    if (isNumber(pr1) && isNumber(pr2))
    {
        return std::stoi(pr1) < std::stoi(pr2);
    }
    else if (isNumber(pr1) && !isNumber(pr2))
    {
        return true;
    }
    else
    {
        return lexicographicalCompare(pr1.begin(), pr1.end(),
                                      pr2.begin(), pr2.end());
    }

    return false;
}

// Returns true, if pr1 < pr2
bool comparePreleases(const std::string& pr1,
                      const std::string& pr2)
{
    // pre-release version is always lower than release version
    if (!pr1.empty() && pr2.empty())
    {
        return true;
    }
    else if (pr1.empty() && !pr2.empty())
    {
        return false;
    }

    auto pr1fields = split(pr1, '.');
    auto pr2fields = split(pr2, '.');

    auto nSameFields = std::min(pr1fields.size(), pr2fields.size());

    for (unsigned int i = 0; i < nSameFields; ++i)
    {
        if (comparePreleaseField(pr1fields[i], pr2fields[i]))
        {
            return true;
        }
        else if (comparePreleaseField(pr2fields[i], pr1fields[i]))
        {
            return false;
        }
    }

    // all until nSameField are same, only field size matterss
    return pr1fields.size() < pr2fields.size();
}

}

class GtVersionNumber::Impl
{
public:
    /// version information
    QVersionNumber m_vers;

    /// additional version information (e.g. alpha, rc, etc.)
    QString m_pre_release;

    /// addition build information
    QString m_build;
};

GtVersionNumber::GtVersionNumber() :
    m_pimpl{std::make_unique<Impl>()}
{
}

GtVersionNumber::GtVersionNumber(int major, int minor, int patch,
                                 const QString& pre_release,
                                 const QString& build) :
    GtVersionNumber()
{
    m_pimpl->m_vers = QVersionNumber(major, minor, patch);
    m_pimpl->m_pre_release = pre_release;
    m_pimpl->m_build = build;
}

GtVersionNumber::GtVersionNumber(int major, int minor,
                                 const QString& pre_release,
                                 const QString& build) :
    GtVersionNumber()
{
    m_pimpl->m_vers = QVersionNumber(major, minor);
    m_pimpl->m_pre_release = pre_release;
    m_pimpl->m_build = build;
}

GtVersionNumber::GtVersionNumber(int major, const QString& pre_release,
                                 const QString& build) :
    GtVersionNumber()
{
    m_pimpl->m_vers = QVersionNumber(major);
    m_pimpl->m_pre_release = pre_release;
    m_pimpl->m_build = build;
}


GtVersionNumber::GtVersionNumber(const QString& versStr) :
    GtVersionNumber()
{
    const auto expr = std::regex(GtRegExp::forSemVers().pattern().toStdString());
    auto results = std::match_results<std::string::const_iterator>();


    std::string str = versStr.toStdString();

    if (std::regex_search(str, results, expr)) {
        int major = (int)atof(results[1].str().c_str());

        if (results[3].length() != 0)
        {
            int minor = (int)atof(results[3].str().c_str());

            if (results[5].length() != 0)
            {
                int patch = (int)atof(results[5].str().c_str());
                m_pimpl->m_vers = QVersionNumber(major, minor, patch);
            }
            else
            {
                m_pimpl->m_vers = QVersionNumber(major, minor);
            }
        }
        else
        {
            m_pimpl->m_vers = QVersionNumber(major);
        }

        m_pimpl->m_pre_release = QString::fromStdString(results[6]);
        m_pimpl->m_build = QString::fromStdString(results[7]);
    }
}

GtVersionNumber::GtVersionNumber(const GtVersionNumber& v2) :
    m_pimpl{std::make_unique<Impl>(*v2.m_pimpl)}
{
}

GtVersionNumber&
GtVersionNumber::operator =(const GtVersionNumber& v2)
{
    *m_pimpl = *v2.m_pimpl;

    return *this;
}

GtVersionNumber::~GtVersionNumber() = default;

QString
GtVersionNumber::toString() const
{
    QString retval = m_pimpl->m_vers.toString();

    if (!m_pimpl->m_pre_release.isEmpty())
    {
        retval = retval + QStringLiteral("-") + m_pimpl->m_pre_release;
    }

    if (!m_pimpl->m_build.isEmpty())
    {
        retval = retval + QStringLiteral("+") + m_pimpl->m_build;
    }

    return retval;
}

int
GtVersionNumber::major() const
{
    return m_pimpl->m_vers.majorVersion();
}

int
GtVersionNumber::minor() const
{
    return m_pimpl->m_vers.minorVersion();
}

int
GtVersionNumber::patch() const
{
    return m_pimpl->m_vers.microVersion();
}

QString
GtVersionNumber::preRelease() const
{
    return m_pimpl->m_pre_release;
}

QString
GtVersionNumber::build() const
{
    return m_pimpl->m_build;
}

bool
GtVersionNumber::isNull() const
{
    return m_pimpl->m_vers.isNull();
}

int
GtVersionNumber::compare(const GtVersionNumber& v1,
                         const GtVersionNumber& v2)
{
    int retval = QVersionNumber::compare(v1.m_pimpl->m_vers,
                                         v2.m_pimpl->m_vers);

    if (retval != 0)
    {
        return retval;
    }

    if (comparePreleases(v1.m_pimpl->m_pre_release.toStdString(),
                         v2.m_pimpl->m_pre_release.toStdString()))
    {
        return -1;
    }

    if (comparePreleases(v2.m_pimpl->m_pre_release.toStdString(),
                         v1.m_pimpl->m_pre_release.toStdString()))
    {
        return 1;
    }

    return 0;
}

bool
operator==(const GtVersionNumber&v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) == 0);
}

bool
operator!=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) != 0);
}

bool
operator>(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) > 0);
}

bool
operator<(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) < 0);
}

bool
operator>=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) >= 0);
}

bool
operator<=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) <= 0);
}
