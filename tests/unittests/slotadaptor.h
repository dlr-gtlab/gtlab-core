/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef SLOTADAPTOR_H
#define SLOTADAPTOR_H

#include <QObject>

class SlotAdaptor : public QObject
{
    Q_OBJECT
public:
    SlotAdaptor();

    template <typename Functor>
    void on(QObject* a, const char* signal, Functor f){
        connect(a, signal, this, SIGNAL(trigger()));
        connect(this, &SlotAdaptor::trigger, this, f);
    }

signals:
    void trigger();
};

#endif // SLOTADAPTOR_H
