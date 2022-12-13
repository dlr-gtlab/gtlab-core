/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 6.12.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_logerrormessagebox.h"
#include "gt_logmodel.h"
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_logging.h"
#include "gt_listview.h"

#include <QMessageBox>
#include <QPointer>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QResizeEvent>

#ifndef NDEBUG
  #include <QThread>
#endif

void
GtLogErrorMessageBox::display(gt::log::Level level,
                         QString const& message,
                         GtLogDetails const& details,
                         QWidget* parent)
{
    // pipe errors only
    if (level < gt::log::levelToInt(gt::log::ErrorLevel))
    {
        return;
    }

#ifndef NDEBUG
    // make sure the thread is the same
    static auto threadId = QThread::currentThreadId();
    assert(QThread::currentThreadId() == threadId);
#endif

    // we want a static instance of the error message box
    static QPointer<GtLogErrorMessageBox> ptr;

    if (!ptr)
    {
        // ptr does not exist -> create message box and execute
        GtLogErrorMessageBox box(parent);
        box.appendMessage(level, message, details);
        ptr = &box;
        // this will block the current function call but keep the event loop
        // running -> new messages may be appended
        box.exec();
        // not strictly neccessary as QPointer does the cleanup
        // but hides dangling pointer warning
        ptr.clear();
    }
    else
    {
        // message box still exists, append message
        ptr->appendMessage(level, message, details);
    }
}

GtLogErrorMessageBox::GtLogErrorMessageBox(QWidget* parent) :
    GtDialog(parent)
{
    setObjectName("LogMessageBox");
    setWindowTitle(tr("Error"));

    QSizePolicy fixedSizePolicy{QSizePolicy::Fixed, QSizePolicy::Fixed};

    m_infoLabel = new QLabel{tr("An Error occured:")};
    m_infoLabel->setSizePolicy(fixedSizePolicy);

    m_view = new GtListView;
    m_model = new GtLogErrorMessageBoxModel;
    m_view->setModel(m_model);

    QPalette p;
    p.setColor(QPalette::Base, p.color(QPalette::Window));
    m_view->setPalette(p);

    // make table items not selectable
    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->setSelectionMode(GtListView::NoSelection);
    m_view->setSizeAdjustPolicy(GtListView::AdjustToContents);
    m_view->setMinimumHeight(32);
    m_view->setFrameStyle(QFrame::NoFrame);

    auto* icon = new QLabel;
    icon->setAlignment(Qt::AlignTop);

    // cheesy way to access message box icons?!
    {
        QMessageBox dummy;
        // set the appropriate icon
        dummy.setIcon(QMessageBox::Critical);
        icon->setPixmap(dummy.iconPixmap());
        // also update window icon
        setWindowIcon(dummy.windowIcon());
    }

    auto* hlay = new QHBoxLayout;
    hlay->addWidget(icon);
    hlay->addWidget(m_view);

    m_centralWidget = new QWidget;
    m_centralWidget->setLayout(hlay);

    auto* btnlay = new QHBoxLayout;
    auto* closeBtn = new QPushButton(tr("Close"));
    btnlay->addStretch();
    btnlay->addWidget(closeBtn);

    m_lay = new QVBoxLayout;
    m_lay->addWidget(m_infoLabel);
    m_lay->addWidget(m_centralWidget);
    m_lay->addLayout(btnlay);
    setLayout(m_lay);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    setMinimumWidth(300);

    resize(width(), minimumSizeHint().height());
}

void
GtLogErrorMessageBox::resizeEvent(QResizeEvent* event)
{
    if (m_autoResize)
    {
        // was the event triggered by the underlying windowing system
        // (i.e. by the user)? -> widget should no longer "auto" resize
        m_autoResize = !event->spontaneous();
    }
    return GtDialog::resizeEvent(event);
}

void
GtLogErrorMessageBox::appendMessage(gt::log::Level level,
                                    QString const& message,
                                    GtLogDetails const& details)
{
    m_model->appendMessage(level, message, details);

    if (m_model->rowCount() == 2)
    {
        m_infoLabel->setText(tr("Multiple Errors accumulated:"));
    }

    // defers the function call until the widgets geometry updated
    QTimer::singleShot(0, this, [this](){
        autoResize();
    });
}

void
GtLogErrorMessageBox::autoResize()
{
    static int S_MAX_AUTO_WIDTH = 1000;

    // auto resizing widgets is apparently sourcery so we do it our self
    int rowCount = m_model->rowCount();

    // it would be annoying if the widget resized itself every time a new
    // message appears eventhough the user already resized the widget himself
    if (!m_autoResize || rowCount < 1)
    {
        return;
    }

    auto margins = m_lay->contentsMargins();

    // width
    int width = m_centralWidget->width();

    // first we substract the actual width of the view and add the preferred contents width
    width = width - m_view->width() + m_view->sizeHintForColumn(0);
    // append margins (+ some for good measure)
    width += margins.left() + margins.right() + 5;

    // height
    int height = minimumSizeHint().height();

    int cellHeight = m_view->sizeHintForRow(0);
    // about 1.5 rows will fit the widget by default
    height += rowCount * cellHeight - cellHeight * 1.5;

    // clamp width (height is limited by model)
    width = std::min(width, S_MAX_AUTO_WIDTH);

    resize(width, height);
}

GtLogErrorMessageBoxModel::GtLogErrorMessageBoxModel(QObject* parent) :
    QAbstractItemModel(parent)
{ }

void
GtLogErrorMessageBoxModel::appendMessage(gt::log::Level level,
                                         QString const& message,
                                         GtLogDetails const& details)
{
    static constexpr int S_MAX_ENTRIES = 25;

    // remove first entries if the model exceeded the max entry count
    int offset = m_entries.size() - S_MAX_ENTRIES + 1;
    if (offset > 0)
    {
        beginRemoveRows(QModelIndex{}, 0, offset - 1);
        m_entries.remove(0, offset);
        endRemoveRows();
    }

    // insert message
    beginInsertRows(QModelIndex{}, m_entries.size(), m_entries.size());
    m_entries.append({
        level, details.id.isEmpty() ?
                         message :
                         QStringLiteral("%1:  %2").arg(details.id, message)
    });
    endInsertRows();
}
int
GtLogErrorMessageBoxModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_entries.size();
}

int
GtLogErrorMessageBoxModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

QVariant
GtLogErrorMessageBoxModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();
    if (m_entries.empty() || row >= m_entries.size())
    {
        return {};
    }

    switch (role)
    {
    case Qt::TextColorRole:
        if (m_entries[row].level == gt::log::FatalLevel)
        {
            return gt::gui::color::fatalText();
        }
        break;
    case Qt::BackgroundColorRole:
        if (m_entries[row].level == gt::log::FatalLevel)
        {
            return gt::gui::color::fatalTextBackground();
        }
        break;
    case Qt::DisplayRole:
        switch (col)
        {
        case 0:
            return m_entries[row].message;
        default:
            break;
        }
    default:
        break;
    }

    return {};
}

QModelIndex
GtLogErrorMessageBoxModel::index(int row, int column, const QModelIndex& /*parent*/) const
{
    return createIndex(row, column);
}

QModelIndex
GtLogErrorMessageBoxModel::parent(const QModelIndex& /*index*/) const
{
    return {};
}
