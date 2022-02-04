/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessdatadialog.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QNetworkReply>
#include <QThread>
#include <QSpinBox>

#include "gt_application.h"
#include "gt_accessmanager.h"
#include "gt_abstractaccessdataconnection.h"

#include "gt_accessdatadialog.h"

GtAccessDataDialog::GtAccessDataDialog(GtAccessData& data,
                                       const QMetaObject& connection,
                                       QWidget* parent) :
    QDialog(parent), m_data(data), m_portLine(Q_NULLPTR),
    m_connectionMetaData(connection)
{
    setWindowTitle(tr("Add Access Data"));
    setWindowIcon(gtApp->icon(QStringLiteral("loginIcon_16.png")));
    setMaximumSize(QSize(350, 300));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    QLabel* pixLabel = new QLabel;
    QPixmap logo(QStringLiteral(":/pixmaps/accessLogo.png"));
    pixLabel->setPixmap(logo);

    lay->addWidget(pixLabel);

    QFormLayout* formLay = new QFormLayout;
    formLay->setContentsMargins(9, 9, 9, 9);

    m_hostLine = new QLineEdit;
    m_hostLine->setText(data.host());

    formLay->addRow(QStringLiteral("Host:"), m_hostLine);

    m_portLine = new QSpinBox;
    m_portLine->setRange(-1, 65535);
    m_portLine->setValue(data.port());

    GtAbstractAccessDataConnection* con = newConnectionObject();

    if (con != Q_NULLPTR && con->requestPort())
    {
        m_portLine = new QSpinBox;
        m_portLine->setRange(-1, 65535);
        m_portLine->setValue(data.port());

        formLay->addRow(QStringLiteral("Port:"), m_portLine);

        delete con;
    }

    m_userDataCheck = new QCheckBox(QStringLiteral("User Data"));
    m_userDataCheck->setLayoutDirection(Qt::RightToLeft);
    m_userDataCheck->setChecked(!data.user().isEmpty());

    formLay->addRow(QString(), m_userDataCheck);

    m_userLine = new QLineEdit;
    m_userLine->setEnabled(!data.user().isEmpty());
    m_userLine->setText(data.user());

    formLay->addRow(QStringLiteral("User:"), m_userLine);

    m_pwLine = new QLineEdit;
    m_pwLine->setEnabled(!data.user().isEmpty());
    m_pwLine->setEchoMode(QLineEdit::Password);
    m_pwLine->setText(data.password());

    formLay->addRow(QStringLiteral("Password:"), m_pwLine);

    m_outputLabel = new QLabel;
    m_outputLabel->setAlignment(Qt::AlignRight);
    formLay->addRow(QString(), m_outputLabel);

    lay->addLayout(formLay);

    QHBoxLayout* btnLay = new QHBoxLayout;
    btnLay->setContentsMargins(9, 9, 9, 9);

    m_testConnectionBtn = new QPushButton(tr("Test Connection"));
    m_testConnectionBtn->setIcon(
                gtApp->icon(QStringLiteral("connectionIcon_16.png")));

    btnLay->addWidget(m_testConnectionBtn);

    QSpacerItem* spacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum);
    btnLay->addSpacerItem(spacer);

    m_saveBtn = new QPushButton(tr("Save"));
    m_saveBtn->setIcon(gtApp->icon(QStringLiteral("checkIcon_16.png")));

    btnLay->addWidget(m_saveBtn);

    m_cancelBtn = new QPushButton(tr("Cancel"));
    m_cancelBtn->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

    btnLay->addWidget(m_cancelBtn);

    lay->addLayout(btnLay);

    setLayout(lay);

    connect(m_testConnectionBtn, SIGNAL(clicked(bool)), SLOT(testConnection()));
    connect(m_saveBtn, SIGNAL(clicked(bool)), SLOT(saveAccessData()));
    connect(m_cancelBtn, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(m_hostLine, SIGNAL(textEdited(QString)), SLOT(clearOutput()));
    connect(m_userLine, SIGNAL(textEdited(QString)), SLOT(clearOutput()));
    connect(m_pwLine, SIGNAL(textEdited(QString)), SLOT(clearOutput()));
    connect(m_userDataCheck, SIGNAL(toggled(bool)), SLOT(clearOutput()));
    connect(m_userDataCheck, SIGNAL(toggled(bool)), m_userLine,
            SLOT(setEnabled(bool)));
    connect(m_userDataCheck, SIGNAL(toggled(bool)), m_pwLine,
            SLOT(setEnabled(bool)));
}

void
GtAccessDataDialog::enableButtons(bool val)
{
    m_saveBtn->setEnabled(val);
    m_cancelBtn->setEnabled(val);
    m_testConnectionBtn->setEnabled(val);
}

GtAbstractAccessDataConnection*
GtAccessDataDialog::newConnectionObject()
{
    QObject* obj = m_connectionMetaData.newInstance();

    if (obj != Q_NULLPTR)
    {
        GtAbstractAccessDataConnection* retval =
                qobject_cast<GtAbstractAccessDataConnection*>(obj);

        if (retval == Q_NULLPTR)
        {
            delete obj;
        }
        else
        {
            return retval;
        }
    }

    return Q_NULLPTR;
}

void
GtAccessDataDialog::saveAccessData()
{
    int port = -1;

    if (m_portLine != Q_NULLPTR)
    {
        port = m_portLine->value();
    }

    if (m_userDataCheck)
    {
        m_data = GtAccessData(m_hostLine->text(),
                              port,
                              m_userLine->text(),
                              m_pwLine->text());
    }
    else
    {
        m_data = GtAccessData(m_hostLine->text(),
                              port);
    }

    accept();
}

void
GtAccessDataDialog::testConnection()
{
    enableButtons(false);

    int port = -1;

    if (m_portLine != Q_NULLPTR)
    {
        port = m_portLine->value();
    }

    GtAccessData accessData(m_hostLine->text(), port);

    if (m_userDataCheck->isChecked())
    {
        accessData = GtAccessData(m_hostLine->text(),
                                  port,
                                  m_userLine->text(),
                                  m_pwLine->text());
    }

    GtAbstractAccessDataConnection* con = newConnectionObject();

    if (con == Q_NULLPTR)
    {
        return;
    }

    con->setAccessData(accessData);

    QThread* thread = new QThread;
    con->moveToThread(thread);

    connect(thread, SIGNAL(started()), con, SLOT(run()));
    connect(con, SIGNAL(connectionSuccessful()), thread, SLOT(quit()));
    connect(con, SIGNAL(connectionSuccessful()),
            con, SLOT(deleteLater()));
    connect(con, SIGNAL(connectionSuccessful()),
            SLOT(onTestSuccessful()));
    connect(con, SIGNAL(connectionFailed()), thread, SLOT(quit()));
    connect(con, SIGNAL(connectionFailed()),
            con, SLOT(deleteLater()));
    connect(con, SIGNAL(connectionFailed()), SLOT(onTestFailed()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void
GtAccessDataDialog::onTestSuccessful()
{
    m_outputLabel->setText(
                "<font color='darkgreen'>connection successful!</font>");

    enableButtons(true);
}

void
GtAccessDataDialog::onTestFailed()
{
    m_outputLabel->setText(
                "<font color='red'>connection failed!</font>");

    enableButtons(true);
}

void
GtAccessDataDialog::clearOutput()
{
    m_outputLabel->clear();
}
