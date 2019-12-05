#ifndef GTPROPERTYNOZZLEMAPEDITOR_H
#define GTPROPERTYNOZZLEMAPEDITOR_H

#include <QWidget>
#include <QPointer>

class QLineEdit;
class GtNozzleMapProperty;

/**
 * @brief The GtPropertyNozzleMapEditor class
 */
class GtPropertyNozzleMapEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyNozzleMapEditor
     * @param parent
     */
    GtPropertyNozzleMapEditor(QWidget* parent = Q_NULLPTR);

    /**
     * @brief setTurboMapProperty
     * @param prop
     */
    void setNozzleMapProperty(GtNozzleMapProperty* prop);

private:
    ///
    QLineEdit* m_mapId;

    ///
    QPointer<GtNozzleMapProperty> m_prop;

    /// Current file name
    QString m_filename;

    /**
     * @brief updateText
     */
    void updateText();

private slots:
    /**
     * @brief selectMap
     */
    void selectMap();

    /**
     * @brief deleteMap
     */
    void deleteMap();
};

#endif // GTPROPERTYNOZZLEMAPEDITOR_H
