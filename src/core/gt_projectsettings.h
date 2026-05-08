/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#ifndef GT_PROJECTSETTINGS_H
#define GT_PROJECTSETTINGS_H

#include <gt_core_exports.h>

#include <QMap>
#include <QString>
#include <QVariant>
#include <QJsonValue>

class GT_CORE_EXPORT GtProjectSettings : public QObject
{
    Q_OBJECT
public:
    using MapType = QMap<QString, QVariant>;
    using iterator = MapType::iterator;
    using const_iterator = MapType::const_iterator;
    using key_type = MapType::key_type;
    using mapped_type = MapType::mapped_type;

    GtProjectSettings();
    ~GtProjectSettings() override = default;

    // specific project settings

    /**
     * @brief Returns whether project irregularity warnings are ignored for this
     * project.
     * @return True if irregularity warning are ignored.
     */
    bool ignoringIrregularities() const;

    /**
     * @brief Sets whether project irregularities should be ignored for this
     * project.
     * @param ignore
     */
    void setIgnoreIrregularities(bool ignore);

    /**
     * @brief Returns, whether objects are allowed to serialize into own files
     */
    bool ownObjectFileSerializationEnabled() const;

    /**
     * @brief Sets, whether certain GtObjects can be serialized into own
     * *.gtobj.xml files instead of writing everything into the main module
     * files.
     */
    void setOwnObjectFileSerializationEnabled(bool enable);

    // --- Basic API ---

    /// Set a value for a key
    void setValue(const QString& key, const QVariant& value);

    /// Convenience operator[] (non-const): creates an entry if it doesn't exist
    QVariant& operator[](const QString& key);

    /// Get a value; returns defaultValue if not found
    QVariant value(const QString& key,
                   const QVariant& defaultValue = QVariant()) const;

    /// Check if key exists
    bool contains(const QString& key) const;

    /// Remove a key; returns number of removed entries (0 or 1)
    int remove(const QString& key);

    /// Clear all settings
    void clear();

    /// Number of entries
    int size() const;

    bool isEmpty() const;

    // --- Iterators ---

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    // --- JSON support --- //

    /// Serialize to a JSON element (a QJsonValue containing an object)
    QJsonValue toJson() const;

    /// Deserialize from a JSON element (expects an object)
    void fromJson(const QJsonValue& json);

signals:
    void changed();

private:
    void initDefaultValues();

    MapType m_settings;
};


#endif // GT_PROJECTSETTINGS_H
