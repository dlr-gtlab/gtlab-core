#ifndef GTSTRINGCONTAINER_H
#define GTSTRINGCONTAINER_H

#include "gt_mdi_exports.h"

#include "gt_object.h"

#include <QString>

/**
 * @brief The GtStringContainer class
 */
class GT_MDI_EXPORT GtStringContainer : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QString entry READ entry WRITE setEntry)

public:

    Q_INVOKABLE GtStringContainer();

    GtStringContainer(QString str);

    QString entry() const;

    void setEntry(const QString& entry);

private:

    QString m_entry;
};

#endif // GTSTRINGCONTAINER_H