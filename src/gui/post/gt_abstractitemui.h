#ifndef GTABSTRACTITEMUI_H
#define GTABSTRACTITEMUI_H

#include "gt_gui_exports.h"

#include "gt_posttemplateitem.h"
#include "QWidget"
#include "gt_templateviewer.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QHBoxLayout;
class GtAbstractPostWidget;
class GtPostAction;
class QPrinter;

/**
 * @brief The GtAbstractItemUI class
 */
class GT_GUI_EXPORT GtAbstractItemUI : public QWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GtAbstractItemUI(GtAbstractPostWidget* widget);

//    void refresh();

    void resize();

    virtual void updatePlot() {}

    virtual void checkDataConnections() {}

    void setWidget(GtAbstractPostWidget* widget);

    void setHasConfigMenu(bool show);

    void setIsPrintable(bool show);

    GtAbstractPostWidget* widget();

    void createConfigMenu(const QList<GtPostAction*>& list);

    void adjustToolBar();

    void printEmbedded(QPrinter* printer, QPainter*painter, QRectF rect);

    GtPostTemplateItem* data();

protected:

    QLineEdit* m_titleLineEdit;

    QPushButton* m_closeButton;

    QPushButton* m_copyButton;

    QPushButton* m_printButton;

    QPushButton* m_configButton;

    QLabel* m_iconLabel;

    QHBoxLayout* m_mainLayout;

    QHBoxLayout* m_tollBarLayout;

    GtAbstractPostWidget* m_widget;

signals:

    void deleteUI(GtAbstractItemUI* ai);

    void copyUI(GtPostTemplateItem* ti);

    void moveSignal(GtTemplateViewer::MoveDirection d);

private slots:

    void updatePlots();

    void exportPlot();

    void copyPlot();

    void deletePlot();

    void configurePlot();

    void plotNameChanged(QString s);

    void moveUp();

    void moveDown();

    /**
     * @brief onProvidernameChanged -
     * reaction on a changed provider Name: reset the plot title to QString s
     * @param s - new name
     */
    void onProvidernameChanged(QString s);
};

#endif // GTABSTRACTITEMUI_H
