#ifndef GTPROPERTYCONTAINERWIDGET_H
#define GTPROPERTYCONTAINERWIDGET_H

#include <QWidget>
#include <QPointer>

class GtObject;
class GtPropertyStructContainer;
class GtPropertyTreeView;

class GtPropertyContainerWidget : public QWidget
{
    Q_OBJECT

public:
    GtPropertyContainerWidget(GtObject* obj,
                              GtPropertyStructContainer& container,
                              QWidget* parent = nullptr);

private:
    QPointer<GtObject> m_obj;

    QStringList m_allowedTypes;

    QString m_containerId;

    GtPropertyTreeView* m_containerTree;

    void addNewEntry(GtPropertyStructContainer& container,
                     const QString& entryType);

private slots:
    void addNewEntry();

};

#endif // GTPROPERTYCONTAINERWIDGET_H
