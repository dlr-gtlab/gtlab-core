/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 6.12.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTLOGERRORMESSAGEBOX_H
#define GTLOGERRORMESSAGEBOX_H

#include "gt_dialog.h"
#include "gt_loglevel.h"

#include <QAbstractTableModel>

struct GtLogDetails;
class QLabel;
class QVBoxLayout;
class GtListView;
class GtLogErrorMessageBoxModel;

/**
 * @brief Simple helper object that can be connected to the Signal Slot Logging
 * Destination Functor to log error messages non blockingly.
 */
class GtlogErrorMessageBoxHandler : public QObject
{
    Q_OBJECT

public slots:

    void onLogMessage(QString const& msg, int lvl, GtLogDetails const& details);
};

/**
 * @brief The GtLogErrorMessageBox class
 * A simple dialog that displays accumulating messages into a single message
 * box like widget
 */
class GtLogErrorMessageBox : public GtDialog
{
public:

    /**
     * @brief Displays the message. If a message box already exists the message
     * will be appended. Logs only messages with the Error Level or higher.
     * Must not be called from different threads but should be invoked using a
     * slot function.
     * @param level Level of the message
     * @param message Message to display
     * @param details Message details
     * @param parent Parent object
     */
    static void display(QString const& message,
                        gt::log::Level level,
                        GtLogDetails const& details,
                        QWidget* parent = {});

protected:

    void resizeEvent(QResizeEvent* event);

private:

    /// info label
    QLabel* m_infoLabel{};

    /// central widget containing view
    QWidget* m_centralWidget{};

    /// base layout
    QVBoxLayout* m_lay{};

    /// list view of messages
    GtListView* m_view{};

    /// item model
    GtLogErrorMessageBoxModel* m_model{};

    /// indicates whether we should resize the widget automatically
    bool m_autoResize{true};

    /// ctor
    explicit GtLogErrorMessageBox(QWidget* parent = {});

    /**
     * @brief Appends the message to the message box
     * @param message Message to append
     * @param details Message details
     */
    void appendMessage(gt::log::Level level,
                       QString const& message,
                       GtLogDetails const& details);

    /**
     * @brief Attempts to resize the widget properly
     */
    void autoResize();
};

class GtLogErrorMessageBoxModel : public QAbstractItemModel
{

    Q_OBJECT

public:

    struct Entry
    {
        gt::log::Level level;
        QString message;
    };

    explicit GtLogErrorMessageBoxModel(QObject* parent = {});

    void appendMessage(gt::log::Level level,
                       QString const& message,
                       GtLogDetails const& details);

    int rowCount(QModelIndex const& parent = {}) const override;

    int columnCount(QModelIndex const& parent = {}) const override;

    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, QModelIndex const& parent = {}) const override;

    QModelIndex parent(const QModelIndex& index) const override;

private:

    QVector<Entry> m_entries;
};

#endif // GTLOGERRORMESSAGEBOX_H
