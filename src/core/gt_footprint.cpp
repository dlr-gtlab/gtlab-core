/* GTlab - Gas Turbine laboratory
 * Source File: gt_footprint.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 28.04.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QCryptographicHash>

#include "gt_coreapplication.h"
#include "gt_versionnumber.h"
#include <gt_logging.h>

#include "gt_footprint.h"

class GtFootprintImpl
{
public:
    /// Core framework major version
    int m_core_ver_major;

    /// Core framework minor version
    int m_core_ver_minor;

    /// Core framework patch level
    int m_core_ver_patch;

    /// Module ids and versions
    QMap<QString, GtVersionNumber> m_modules;

    /**
     * @brief readData
     * @param data
     * @return
     */
    bool readData(const QString& data);

};

GtFootprint::GtFootprint()
{
    m_pimpl = new GtFootprintImpl;

    m_pimpl->m_core_ver_major = gtApp->majorRelease();
    m_pimpl->m_core_ver_minor = gtApp->minorRelease();
    m_pimpl->m_core_ver_patch = gtApp->patchLevel();

    QStringList mids = gtApp->moduleIds();
    mids.sort();

    foreach (const QString& mid, mids)
    {
        m_pimpl->m_modules.insert(mid, gtApp->moduleVersion(mid));
    }
}

GtFootprint::GtFootprint(const QString& data)
{
    m_pimpl = new GtFootprintImpl;
    m_pimpl->m_core_ver_major = 0;
    m_pimpl->m_core_ver_minor = 0;
    m_pimpl->m_core_ver_patch = 0;

    m_pimpl->readData(data);
}

GtFootprint::~GtFootprint()
{
    delete m_pimpl;
}

bool
GtFootprint::isValid() const
{
    if (m_pimpl->m_core_ver_major == 0 && m_pimpl->m_core_ver_minor ==0 &&
            m_pimpl->m_core_ver_patch == 0)
    {
        return false;
    }

    return true;
}

QByteArray
GtFootprint::generateHash() const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(QString::number(m_pimpl->m_core_ver_major).toUtf8());
    hash.addData(QString::number(m_pimpl->m_core_ver_minor).toUtf8());
    hash.addData(QString::number(m_pimpl->m_core_ver_patch).toUtf8());

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

    stream.writeStartElement(QStringLiteral("core-ver"));

    stream.writeTextElement(QStringLiteral("major"),
                            QString::number(m_pimpl->m_core_ver_major));
    stream.writeTextElement(QStringLiteral("minor"),
                            QString::number(m_pimpl->m_core_ver_minor));
    stream.writeTextElement(QStringLiteral("patch"),
                            QString::number(m_pimpl->m_core_ver_patch));

    stream.writeEndElement(); // core-ver

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

    if (envFootPrint.m_pimpl->m_core_ver_major > m_pimpl->m_core_ver_major)
    {
        return true;
    }
    else if (envFootPrint.m_pimpl->m_core_ver_major < m_pimpl->m_core_ver_major)
    {
        return false;
    }

    if (envFootPrint.m_pimpl->m_core_ver_minor > m_pimpl->m_core_ver_minor)
    {
        return true;
    }
    else if (envFootPrint.m_pimpl->m_core_ver_minor < m_pimpl->m_core_ver_minor)
    {
        return false;
    }

    if (envFootPrint.m_pimpl->m_core_ver_patch > m_pimpl->m_core_ver_patch)
    {
        return true;
    }

    return false;
}

bool
GtFootprint::isNewerRelease() const
{
    GtFootprint envFootPrint;

    if (envFootPrint.m_pimpl->m_core_ver_major < m_pimpl->m_core_ver_major)
    {
        return true;
    }
    else if (envFootPrint.m_pimpl->m_core_ver_major > m_pimpl->m_core_ver_major)
    {
        return false;
    }

    if (envFootPrint.m_pimpl->m_core_ver_minor < m_pimpl->m_core_ver_minor)
    {
        return true;
    }
    else if (envFootPrint.m_pimpl->m_core_ver_minor > m_pimpl->m_core_ver_minor)
    {
        return false;
    }

    if (envFootPrint.m_pimpl->m_core_ver_patch < m_pimpl->m_core_ver_patch)
    {
        return true;
    }

    return false;
}

QString
GtFootprint::versionToString() const
{
    return QString::number(m_pimpl->m_core_ver_major) + QStringLiteral(".") +
            QString::number(m_pimpl->m_core_ver_minor) + QStringLiteral(".") +
            QString::number(m_pimpl->m_core_ver_patch);
}

QMap<QString, GtVersionNumber>
GtFootprint::unknownModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    foreach (const QString& mid, m_pimpl->m_modules.keys())
    {
        if (!envFootPrint.m_pimpl->m_modules.contains(mid))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    }

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::incompatibleModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    foreach (const QString& mid, m_pimpl->m_modules.keys())
    {
        if ((envFootPrint.m_pimpl->m_modules.contains(mid)) &&
                (envFootPrint.m_pimpl->m_modules.value(mid) <
                 m_pimpl->m_modules.value(mid)))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    }

    return retval;
}

QMap<QString, GtVersionNumber>
GtFootprint::updatedModules() const
{
    QMap<QString, GtVersionNumber> retval;

    GtFootprint envFootPrint;

    foreach (const QString& mid, m_pimpl->m_modules.keys())
    {
        if ((envFootPrint.m_pimpl->m_modules.contains(mid)) &&
                (envFootPrint.m_pimpl->m_modules.value(mid) >
                 m_pimpl->m_modules.value(mid)))
        {
            retval.insert(mid, m_pimpl->m_modules.value(mid));
        }
    }

    return retval;
}

bool
GtFootprintImpl::readData(const QString& data)
{
    // temporary variables
    int tmp_core_ver_major = 0;
    int tmp_core_ver_minor = 0;
    int tmp_core_ver_patch = 0;
    QMap<QString, GtVersionNumber> temp_modules;

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument document;
    if (!document.setContent(data, true, &errorStr, &errorLine, &errorColumn))
    {
        gtError() << "footprint data error!";
        gtError() << "  |-> " << errorLine << ";" << errorColumn << " - " <<
                     errorStr;

        return false;
    }

    QDomElement elm_root = document.documentElement();

    if (elm_root.isNull() || (elm_root.tagName() !=
                              QLatin1String("env-footprint")))
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    // read core version
    QDomElement elm_core_ver =
            elm_root.firstChildElement(QStringLiteral("core-ver"));

    if (elm_core_ver.isNull())
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    QDomElement elm_core_ver_major =
            elm_core_ver.firstChildElement(QStringLiteral("major"));

    if (elm_core_ver_major.isNull())
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    QDomElement elm_core_ver_minor =
            elm_core_ver.firstChildElement(QStringLiteral("minor"));

    if (elm_core_ver_minor.isNull())
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    QDomElement elm_core_ver_patch =
            elm_core_ver.firstChildElement(QStringLiteral("patch"));

    if (elm_core_ver_patch.isNull())
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    bool conv_success = false;

    tmp_core_ver_major = elm_core_ver_major.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    tmp_core_ver_minor = elm_core_ver_minor.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    tmp_core_ver_patch = elm_core_ver_patch.text().toInt(&conv_success);

    if (!conv_success)
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    // read modules
    QDomElement elm_modules =
            elm_root.firstChildElement(QStringLiteral("modules"));

    if (elm_modules.isNull())
    {
        gtError() << "footprint data corrupted!";
        return false;
    }

    QDomElement elm_module_it =
            elm_modules.firstChildElement(QStringLiteral("module"));

    while (!elm_module_it.isNull())
    {
        // temporary module variables
        QString tmp_mod_id;
        int tmp_mod_ver = 0;

        QDomElement elm_module_it_id =
                elm_module_it.firstChildElement(QStringLiteral("id"));

        if (elm_module_it_id.isNull())
        {
            gtError() << "footprint data corrupted!";
            return false;
        }

        QDomElement elm_module_it_ver =
                elm_module_it.firstChildElement(QStringLiteral("ver"));

        if (elm_module_it_ver.isNull())
        {
            gtError() << "footprint data corrupted!";
            return false;
        }

        tmp_mod_id = elm_module_it_id.text();

        tmp_mod_ver = elm_module_it_ver.text().toInt(&conv_success);

        if (!conv_success)
        {
            gtError() << "footprint data corrupted!";
            return false;
        }

        if (temp_modules.contains(tmp_mod_id))
        {
            gtError() << "footprint data corrupted!";
            return false;
        }

        temp_modules.insert(tmp_mod_id, GtVersionNumber(tmp_mod_ver));

        elm_module_it =
                elm_module_it.nextSiblingElement(QStringLiteral("module"));
    }

    m_core_ver_major = tmp_core_ver_major;
    m_core_ver_minor = tmp_core_ver_minor;
    m_core_ver_patch = tmp_core_ver_patch;
    m_modules = temp_modules;

    return true;
}
