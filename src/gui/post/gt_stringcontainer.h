#ifndef GTSTRINGCONTAINER_H
#define GTSTRINGCONTAINER_H

#include "gt_gui_exports.h"

#include "gt_object.h"

#include <QString>

/**
 * @brief The GtStringContainer class
 */
class GT_GUI_EXPORT GtStringContainer : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QString entry READ entry WRITE setEntry)

public:

    Q_INVOKABLE GtStringContainer();

    explicit GtStringContainer(const QString& str);

    QString entry() const;

    void setEntry(const QString& entry);

private:

    QString m_entry;
};

#endif // GTSTRINGCONTAINER_H
