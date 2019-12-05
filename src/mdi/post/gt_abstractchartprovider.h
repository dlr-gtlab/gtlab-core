#ifndef GTABSTRACTCHARTPROVIDER_H
#define GTABSTRACTCHARTPROVIDER_H

#include "gt_mdi_exports.h"

#include "gt_posttemplateitem.h"

class GtDataZoneTableList;
class GtDataZoneTable;
class GtStringContainer;
class GtObjectGroup;

typedef QList<QColor> colorList;




/**
 * @brief The GtAbstractChartProvider class
 */
class GT_MDI_EXPORT GtAbstractChartProvider : public GtPostTemplateItem
{
    Q_OBJECT

    Q_PROPERTY(QStringList varColorsUseR READ colorsUseR WRITE setColorsUseR)
    Q_PROPERTY(QStringList varColorsUseL READ colorsUseL WRITE setColorsUseL)
    Q_PROPERTY(bool showmarkers READ showmarkers WRITE setShowmarkers)
    Q_PROPERTY(bool showwidgets READ showwidgets WRITE setShowwidgets)

public:

    Q_INVOKABLE GtAbstractChartProvider();

    virtual GtAbstractPostWidget* createUI(GtPostTemplateItem* /*dm*/,
                                           QWidget* /*parent*/) Q_DECL_OVERRIDE
    { return Q_NULLPTR; }

    QStringList uuids() const;

    void setUUIDs(const QStringList& uuids);

    void addUUID(const QString& uuid);

    QColor colorUseRAt(int i);

    QList<QString> colorsUseR() const;

    void setColorsUseR(const QList<QString>& colorsUse);

    void addColorUseR(QColor col);

    QColor colorUseLAt(int i);

    void fitColorsL(int params);

    void fitColorsR(int params);

    QList<QString> colorsUseL() const;

    void setColorsUseL(const QList<QString>& colorsUse);

    void addColorUseL(QColor col);

    void clearColorsUseL();

    void clearColorsUseR();

    void setSingleColorsUseR(int index, QColor col);

    void setSingleColorsUseL(int index, QColor col);

    int bottomIterator(GtDataZoneTableList* dztList, QString bottomAxis,
                       QVector<double>& axisTicks);

    // no usage of 'axisTicks'
    int bottomIterator(GtDataZoneTableList* dztList, QString bottomAxis);

    int additionalIterator(GtDataZoneTableList* dztList, QString additionalAxis,
                           QStringList scharParameters,
                           QVector<double>& axisTicks);

    // no usage of 'axisTicks'
    int additionalIterator(GtDataZoneTableList* dztList, QString additionalAxis,
                           QStringList scharParameters);

    void fixMainValues(GtDataZoneTableList* dztList,
                       QMap<QString, QString> fixedMain,
                       int& fixXmain, int& fixYmain, int& fixZmain);

    void fixMainValues(QMap<QString, QString> fixedMain,
                       QString& fixXmain, QString& fixYmain, QString& fixZmain);

    QStringList prefixToStringList(QStringList list, QString prefix);

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
    QStringList objectNamesFromContainer();

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
