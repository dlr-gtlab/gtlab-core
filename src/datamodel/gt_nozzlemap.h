#ifndef GTNOZZLEMAP_H
#define GTNOZZLEMAP_H

#include "gt_datamodel_exports.h"
#include "gt_object.h"

class GtTable;
class QDomElement;

/**
 * @brief The GtNozzleMap class
 */
class GT_DATAMODEL_EXPORT GtNozzleMap : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QString maptype READ type WRITE setType)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(QString id READ id WRITE setId)

public:

    Q_INVOKABLE GtNozzleMap();

    GtTable* map();

    bool readXML(QDomElement &root);

    QString id() const;

    void setId(QString str);

    QString source() const;

    void setSource(const QString &source);

    QString description() const;

    void setDescription(const QString &brief);

    QString type() const;

    void setType(const QString &type);

    double CD(double npr, double petalAngle);

private:

    /// Map type
    QString m_type;

    /// Brief description of the compresor map
   QString m_brief;

   /// Description of the data source
   QString m_source;

   /// name for editor
   QString m_id;

   /// parse nozzle map table
   bool parseTable(QDomElement& root);

   /// find specific table
   bool findTable(const QDomElement& tables, const QString& name,
                  QDomElement& table);
};

#endif // GTNOZZLEMAP_H
