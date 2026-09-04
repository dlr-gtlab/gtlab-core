#ifndef GTSTDIOEXECUTIONEVENTENCODER_H
#define GTSTDIOEXECUTIONEVENTENCODER_H
#include "gt_core_exports.h"
#include "gt_executionevent.h"
#include <mutex>
#include <optional>
#include <QJsonObject>
#include <QObject>
class GtObject; class QIODevice;
class GT_CORE_EXPORT GtStdioExecutionEventEncoder : public QObject
{
    Q_OBJECT public: explicit GtStdioExecutionEventEncoder(GtExecutionId id,QIODevice& output,QObject* parent=nullptr); bool isTerminal() const noexcept; public slots: void encodeEvent(GtExecutionEvent event); public: bool encodeResult(GtObject const* result); bool encodeFailure(QString errorCode,QString message,std::optional<QJsonValue> details=std::nullopt); private: bool writeRecord(QJsonObject record,bool terminal); GtExecutionId m_executionId; QIODevice& m_output; bool m_terminal{false}; mutable std::mutex m_mutex; static std::mutex s_outputMutex; };
#endif
