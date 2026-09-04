#include "gt_executioneventstream.h"
#include "gt_object.h"
#include "gt_objectmemento.h"
#include <stdexcept>
#include <utility>
GtExecutionEventStream::GtExecutionEventStream(GtExecutionId id,QObject* p):QObject(p),m_executionId(std::move(id)){qRegisterMetaType<GtExecutionEvent>();} GtExecutionId const& GtExecutionEventStream::executionId() const noexcept{return m_executionId;} void GtExecutionEventStream::publish(QString t,QJsonValue p){publish(GtExecutionEvent(m_executionId,0,std::move(t),std::move(p)));} void GtExecutionEventStream::publish(QString t,GtObject const& p){publish(GtExecutionEvent(m_executionId,0,std::move(t),p.toMemento().toByteArray()));} void GtExecutionEventStream::publish(GtExecutionEvent event){if(event.eventType().isEmpty())throw std::invalid_argument("Execution event type must not be empty");std::lock_guard<std::recursive_mutex> lock(m_mutex);event=event.payloadEncoding()==GtExecutionEvent::PayloadEncoding::Json?GtExecutionEvent(m_executionId,m_nextSequence++,event.eventType(),event.jsonPayload()):GtExecutionEvent(m_executionId,m_nextSequence++,event.eventType(),event.mementoXml());emit eventPublished(event);}

