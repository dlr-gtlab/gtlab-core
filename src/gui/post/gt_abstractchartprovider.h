#ifndef GTABSTRACTCHARTPROVIDER_H
#define GTABSTRACTCHARTPROVIDER_H

#include "gt_gui_exports.h"

#include "gt_posttemplateitem.h"

class GtObjectGroup;

typedef QList<QColor> colorList;




/**
 * @brief The GtAbstractChartProvider class
 */
class GT_GUI_EXPORT GtAbstractChartProvider : public GtPostTemplateItem
{
    Q_OBJECT

    Q_PROPERTY(QStringList varColorsUseR READ colorsUseR WRITE setColorsUseR)
    Q_PROPERTY(QStringList varColorsUseL READ colorsUseL WRITE setColorsUseL)
    Q_PROPERTY(bool showmarkers READ showmarkers WRITE setShowmarkers)
    Q_PROPERTY(bool showwidgets READ showwidgets WRITE setShowwidgets)

public:

    Q_INVOKABLE GtAbstractChartProvider();

    GtAbstractPostWidget* createUI(GtPostTemplateItem* /*dm*/,
                                   QWidget* /*parent*/) override
    { return nullptr; }

    QStringList uuids() const;

    void setUUIDs(const QStringList& uuids);

    void addUUID(const QString& uuid);

    QColor colorUseRAt(int i) const;

    QList<QString> colorsUseR() const;

    void setColorsUseR(const QList<QString>& colorsUse);

    void addColorUseR(const QColor& col);

    QColor colorUseLAt(int i);

    void fitColorsL(int params);

    void fitColorsR(int params);

    QList<QString> colorsUseL() const;

    void setColorsUseL(const QList<QString>& colorsUse);

    void addColorUseL(const QColor &col);

    void clearColorsUseL();

    void clearColorsUseR();

    void setSingleColorsUseR(int index, const QColor& col);

    void setSingleColorsUseL(int index, const QColor& col);

    void fixMainValues(const QMap<QString, QString> &fixedMain,
                       QString& fixXmain, QString& fixYmain, QString& fixZmain);

    QStringList prefixToStringList(const QStringList& list,
                                   const QString& prefix) const;

    bool showmarkers() const;

    void setShowmarkers(bool mod);

    bool showwidgets() const;

    void setShowwidgets(bool showwidgets);

    GtObjectGroup* UUIDsContainer() const;

    /**
     * @brief objectNamesFromContainer: returns object names of all
     * uuids saved in the string containers
     * @return
     */
    QStringList objectNamesFromContainer() const;

    /**
     * @brief clearUUIDsContainer: deletes all containers of uuids
     */
    void clearUUIDsContainer();

protected:

    QStringList m_dztUUIDs;

    QList<QString> m_colorsR;

    QList<QString> m_colorsL;

    GtObjectGroup* m_dztUUIDsContainer;

    bool m_showmarkers;

    bool m_showwidgets;
};

#endif // GTABSTRACTCHARTPROVIDER_H
