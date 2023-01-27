#ifndef GTPROPERTYCONTAINERWIDGET_H
#define GTPROPERTYCONTAINERWIDGET_H

#include <QWidget>
#include <QPointer>

class GtObject;
class GtPropertyStructContainer;
class GtPropertyTreeView;

/**
 * @brief The GtPropertyContainerWidget class displays the structure of a given
 * Property-Struct container using a tree view.
 * Additionally the user interface allows to add a new entry to the given
 * container by clicking an add button.
 */
class GtPropertyContainerWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param obj
     * @param container
     * @param parent
     */
    GtPropertyContainerWidget(GtObject* obj,
                              GtPropertyStructContainer& container,
                              QWidget* parent = nullptr);

private:
    /**
     * @brief Pointer to current selected object.
     */
    QPointer<GtObject> m_obj;

    /**
     * @brief Information about allowed types within the property struct
     * container
     */
    QStringList m_allowedTypes;

    /**
     * @brief Property struct container identification string.
     */
    QString m_containerId;

    /**
     * @brief Tree view widget
     */
    GtPropertyTreeView* m_containerTree;

    /**
     * @brief Adds new entry to given property struct container.
     * @param container Container in which the new entry is created-
     * @param entryType Entry type id.
     */
    void addNewEntry(GtPropertyStructContainer& container,
                     const QString& entryType);

private slots:
    /**
     * @brief Triggers the creation of a new entry.
     */
    void addNewEntry();

    /**
     * @brief Called after rows were inserted in the container tree. The new
     * created rows should be expanded.
     * @param parent
     * @param first
     * @param last
     */
    void onRowsInserted(const QModelIndex& parent, int first, int last);

};

#endif // GTPROPERTYCONTAINERWIDGET_H
