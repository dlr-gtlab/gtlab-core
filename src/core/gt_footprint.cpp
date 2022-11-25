/* GTlab - Gas Turbine laboratory
 * Source File: gt_footprint.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 28.04.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_footprint.h"

#include "gt_coreapplication.h"
#include "gt_versionnumber.h"
#include <gt_logging.h>

#include "gt_algorithms.h"

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QCryptographicHash>

#include <memory>

class GtFootprintImpl
{
public:
    /// Core framework version
    GtVersionNumber m_version;

    /// Module ids and versions
    QMap<QString, GtVersionNumber> m_modules;

    /**
     * @brief readData
     * @param data
     * @return
     */
    bool readData(const QString& data);

    GtVersionNumber readOldVersionNumber(
            const QDomElement& coreVersionElement,
            const QDomElement& coreMajorVersionElement);

};

GtFootprint::GtFootprint() :
    m_pimpl{std::make_unique<GtFootprintImpl>()}
{
    m_pimpl->m_version = gtApp->version();

    QStringList mids = gtApp->moduleIds();
    mids.sort();

    foreach (const QString& mid, mids)
    {
        m_pimpl->m_modules.insert(mid, gtApp->moduleVersion(mid));
    }
}

GtFootprint::GtFootprint(const GtFootprint& other) :
    m_pimpl{std::make_unique<GtFootprintImpl>(*other.m_pimpl)}
{
}

GtFootprint::GtFootprint(const QString& data) :
    m_pimpl{std::make_unique<GtFootprintImpl>()}
{
    m_pimpl->readData(data);
}

GtFootprint::~GtFootprint() = default;

bool
GtFootprint::isValid() const
{
    return !m_pimpl->m_version.isNull();
}

QByteArray
GtFootprint::generateHash() const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(m_pimpl->m_version.toString().toUtf8());

    QStringList mids = m_pimpl->m_modules.keys();
    mids.sort();

    foreach (const QString& mid, mids)
    {
        hash.addData(mid.toUtf8());
        hash.addData(m_pimpl->m_modules.value(mid).toString().toUtf8());
    }

    return hash.result().toHex();
}

QString
GtFootprint::exportToXML() const
{
    QString retval;

    QXmlStreamWriter stream(&retval);

    stream.setAutoFormatting(true);

    stream.writeStartElement(QStringLiteral("env-footprint"));

    stream.writeTextElement(QStringLiteral("hash"),
                            QString::fromStdString(
                                generateHash().toStdString()));

    stream.writeTextElement(QStringLiteral("core-ver"),
                            m_pimpl->m_version.toString());

    stream.writeStartElement(QStringLiteral("modules"));

    QStringList mids = m_pimpl->m_modules.keys();
    mids.sort();

    foreach (const QString& mid, mids)
    {
        stream.writeStartElement(QStringLiteral("module"));

        stream.writeTextElement(QStringLiteral("id"), mid);
        stream.writeTextElement(QStringLiteral("ver"),
                                m_pimpl->m_modules.value(mid).toString());

        stream.writeEndElement(); // module
    }

    stream.writeEndElement(); // modules

    stream.writeEndElement(); // footprint

    return retval;
}

bool
GtFootprint::isCompatible() const
{
    GtFootprint envFootPrint;

    if (generateHash() == envFootPrint.generateHash())
    {
        return true;
    }

    if (isNewerRelease())
    {
        return false;
    }

    if (!unknownModules().isEmpty())
    {
        return false;
    }

    if (!incompatibleModules().isEmpty())
    {
        return false;
    }

    return true;
}

bool
GtFootprint::isOlderRelease() const
{
    GtFootprint envFootPrint;

    return (envFootPrint.m_pimpl->m_version > m_pimpl->m_version);
}

bool
GtFootprint::isNewerRelease() const
{
    GtFootprint envFootPrint;

    return (envFootPrint.m_pimpl->m_version < m_pimpl->m_version);
}

GtVersionNumber
GtFootprint::frameworkVersion() const
{
    return m_pimpl->m_version;
}

QMap<QString, GtVersionNumber>
GtFootprint::fullVersionInfo() const
{
    QMap<QString, GtVersionNumber> retval = modules();
    retval.insert(frameworkIdentificationString(), frameworkVersion());

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::unknownModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    gt::for_each_key (m_pimpl->m_modules, [&](const QString& mid)
    {
        if (!envFootPrint.m_pimpl->m_modules.contains(mid))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    });

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::incompatibleModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    gt::for_each_key (m_pimpl->m_modules, [&](const QString& mid)
    {
        if ((envFootPrint.m_pimpl->m_modules.contains(mid)) &&
                (envFootPrint.m_pimpl->m_modules.value(mid) <
                 m_pimpl->m_modules.value(mid)))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    });

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::updatedModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    gt::for_each_key (m_pimpl->m_modules, [&](const QString& mid)
    {
        if ((envFootPrint.m_pimpl->m_modules.contains(mid)) &&
                (envFootPrint.m_pimpl->m_modules.value(mid) >
                 m_pimpl->m_modules.value(mid)))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    });

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::modules() const
{
    return m_pimpl->m_modules;
}

QString
GtFootprint::frameworkIdentificationString()
{
    return QStringLiteral("_core");
}

bool
GtFootprintImpl::readData(const QString& data)
{
    // temporary variables
    bool old_vers_indicator = false;
    QMap<QString, GtVersionNumber> temp_modules;

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument document;
    if (!document.setContent(data, true, &errorStr, &errorLine, &errorColumn))
    {
        gtError() << QObject::tr("footprint data error!");
        gtError() << "  |-> " << errorLine << ";" << errorColumn << " - " <<
                     errorStr;

        return false;
    }

    QDomElement elm_root = document.documentElement();

    if (elm_root.isNull() || (elm_root.tagName() !=
                              QLatin1String("env-footprint")))
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return false;
    }

    // read core version
    QDomElement elm_core_ver =
            elm_root.firstChildElement(QStringLiteral("core-ver"));

    if (elm_core_ver.isNull())
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return false;
    }

    QDomElement elm_core_ver_major =
            elm_core_ver.firstChildElement(QStringLiteral("major"));

    if (!elm_core_ver_major.isNull())
    {
        old_vers_indicator = true;
    }

    // read modules
    QDomElement elm_modules =
            elm_root.firstChildElement(QStringLiteral("modules"));

    if (elm_modules.isNull())
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return false;
    }

    QDomElement elm_module_it =
            elm_modules.firstChildElement(QStringLiteral("module"));

    while (!elm_module_it.isNull())
    {
        // temporary module variables
        QString tmp_mod_id;
        QString tmp_mod_ver;

        QDomElement elm_module_it_id =
                elm_module_it.firstChildElement(QStringLiteral("id"));

        if (elm_module_it_id.isNull())
        {
            gtError() << QObject::tr("footprint data corrupted!");
            return false;
        }

        QDomElement elm_module_it_ver =
                elm_module_it.firstChildElement(QStringLiteral("ver"));

        if (elm_module_it_ver.isNull())
        {
            gtError() << QObject::tr("footprint data corrupted!");
            return false;
        }

        tmp_mod_id = elm_module_it_id.text();

        tmp_mod_ver = elm_module_it_ver.text();

        if (temp_modules.contains(tmp_mod_id))
        {
            gtError() << QObject::tr("footprint data corrupted!");
            return false;
        }

        temp_modules.insert(tmp_mod_id, GtVersionNumber(tmp_mod_ver));

        elm_module_it =
                elm_module_it.nextSiblingElement(QStringLiteral("module"));
    }

    if (old_vers_indicator)
    {
        // old versioning found. pre-release will be set to zero!
        m_version = readOldVersionNumber(elm_core_ver, elm_core_ver_major);

        if (m_version.isNull())
        {
            return false;
        }
    }
    else
    {
        m_version = GtVersionNumber(elm_core_ver.text());
    }

    m_modules = temp_modules;

    return true;
}

GtVersionNumber
GtFootprintImpl::readOldVersionNumber(
        const QDomElement& coreVersionElement,
        const QDomElement& coreMajorVersionElement)
{
    int tmp_core_ver_major = 0;
    int tmp_core_ver_minor = 0;
    int tmp_core_ver_patch = 0;

    QDomElement elm_core_ver_minor =
            coreVersionElement.firstChildElement(QStringLiteral("minor"));

    if (elm_core_ver_minor.isNull())
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return {};
    }

    QDomElement elm_core_ver_patch =
            coreVersionElement.firstChildElement(QStringLiteral("patch"));

    if (elm_core_ver_patch.isNull())
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return {};
    }

    bool conv_success = false;

    tmp_core_ver_major = coreMajorVersionElement.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return {};
    }

    tmp_core_ver_minor = elm_core_ver_minor.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return {};
    }

    tmp_core_ver_patch = elm_core_ver_patch.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << QObject::tr("footprint data corrupted!");
        return {};
    }

    return {tmp_core_ver_major, tmp_core_ver_minor, tmp_core_ver_patch, "0"};
}
