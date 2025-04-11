/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.07.2015
 *  Author: Martin Siggel (AT-TWK) and Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QObject>
#include <QVector>
#include <QList>
#include <QPoint>
#include <QStringList>
#include <QtGlobal>
#include <QDataStream>
#include <QMetaType>

template <typename List>
struct StreamingFunctionBuilder
{

    // create free operator functions to allow qt to stream
    // the list into the data stream
    friend QDataStream& operator>>(QDataStream& s, List& vector)
    {
        //size_t size = 0;
        quint64 size = 0;
        s >> size;

        for (size_t i = 0; i < size; ++i) {
            s >> vector;
        }
        return s;
    }

    friend QDataStream& operator<<(QDataStream& s, const List& vector)
    {
        s << vector.size();

        for(const auto& v : vector) {
            s << v;
        }
        return s;
    }
};

namespace
{
    // we need to be able to serialize these data types
    template<class T>
    void registerBasicContainerType(const char* typeName)
    {
        StreamingFunctionBuilder<T> build_streaming_funcs;
        Q_UNUSED(build_streaming_funcs)
        qRegisterMetaType<T>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        qRegisterMetaTypeStreamOperators<T>(typeName);
#endif
    };
}

// Dynamic type registration to meta object system on library load
static const bool registerTypesOnLibraryLoad = [](){
    // do the registering
    registerBasicContainerType< QVector<double> >("QVector<double>");
    registerBasicContainerType< QVector<int> >("QVector<int>");
    registerBasicContainerType< QVector<bool> >("QVector<bool>");
    registerBasicContainerType< QVector<QPointF> >("QVector<QPointF>");
    registerBasicContainerType< QList<double> >("QList<double>");
    registerBasicContainerType< QList<int> >("QList<int>");
    registerBasicContainerType< QList<bool> > ("QList<bool>");
    registerBasicContainerType< QList<QPointF> >("QList<QPointF>");


#if QT_VERSION >= 0x050900
    registerBasicContainerType< QStringList >("QStringList");
#endif

    return true;
}();
