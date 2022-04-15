#include "gt_shortcutsettingsdata.h"

GtShortCutSettingsData::GtShortCutSettingsData()
{

}

GtShortCutSettingsData::GtShortCutSettingsData(
        QString const& identifier, QString const& cat,
        QKeySequence key, bool readOnly) :
    id(identifier), category(cat),
    shortCut(key), isReadOnly(readOnly)
{

}

QVariant
GtShortCutSettingsData::dataToVariant() const
{
    QString readOnly = "false";
    if (isReadOnly)
    {
        readOnly = "true";
    }

    QStringList l {shortCut.toString(), category, readOnly};
    return QVariant(l);
}
