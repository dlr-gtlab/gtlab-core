Executable operations and the one-shot worker slice
====================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issue `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_
:Related: `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_, `#1527 <https://github.com/dlr-gtlab/gtlab-core/issues/1527>`_, and `PR #1520 <https://github.com/dlr-gtlab/gtlab-core/pull/1520>`_

Context and decision
--------------------

This ADR materializes #1526 for Worker Slice 1. It is not a new architecture:
follow-up issues resolve implementation details, but must not add competing
execution, serialization, registration, result-application, or project
lifecycle models. Executable domain logic is independent of location and
transport. An executable operation is a normal ``GtObject``:

.. code-block:: cpp

   class GtExecutableOperation : public GtObject
   {
   public:
       virtual bool requiresProject() const = 0;
       virtual std::unique_ptr<GtObject>
       createData(const GtExecutionContext& context) const = 0;
       virtual std::unique_ptr<GtObject>
       execute(GtOperationExecutionContext& context) = 0;
       virtual GtOperationApplyResult
       applyResult(/* result */, GtExecutionContext& context) const = 0;
   };

``GtOperationApplyResult`` is a working name for a structured success/failure
outcome; its concrete Core type follows existing conventions. The semantics are
fixed:

.. code-block:: text

   client/originating side                         execution side
   -----------------------                         --------------
   createData() -- detached data --> serialize --> execute() -- detached result -->
          ^                                                               |
          +------------------------------ applyResult() <----------------+

``createData()`` is lightweight, runs on the originating project/application
thread, may read the authoritative project, and returns detached data or
``nullptr``. An execution target never calls it again. ``execute()`` is a
synchronous execution-side contract containing expensive work. It uses only
execution-local state and explicit services, may mutate detached input or an
execution-local project, and never mutates the authoritative project.
``applyResult()`` is a lightweight originating-side commit and is the only step
that makes a result authoritative. It reports structured success/failure: a
failed authoritative commit is a failed client invocation with retained error;
a log-only or implicit ``void`` failure path is forbidden.

Project state is not operation data. ``requiresProject()`` asks whether the
execution location needs a provisioned project; it does not limit client-side
preparation. ``GtExecutionContext`` remains the project context, scoped through
``GtExecutionContextScope`` for project-bound execution.

Serialization, type registration, and ownership
------------------------------------------------

Operations, detached data, and results reuse GTlab Memento/XML and
``GtObjectFactory``. No operation-specific serializer, factory, or worker-side
registry is introduced. A worker reconstructs its boundary object normally and
rejects it before execution if it is not a ``GtExecutableOperation``.

``GtOperationInterface`` is the authoritative optional module declaration of
operation types. The module loader validates declarations and makes types
reconstructable through existing object/type factories. There is no second
factory, global ``GtOperationRegistry``, or public discovery API initially.

Worker Slice 1 follows the existing GTlab module/type-registration convention:

.. code-block:: cpp

   class GtOperationInterface
   {
   public:
       virtual QList<QMetaObject> operations() const { return {}; }
   };

The module loader validates these ``QMetaObject`` declarations and registers
them through the existing ``GtObjectFactory`` and normal GTlab type
infrastructure. Module authors perform no second operation-factory step. This
slice introduces no ``GtTypeId``, ``GtTypeDescriptor``, operation-specific
metadata wrapper, or placeholder ``GtMetaObject``. The future, richer
``GtMetaObject`` type system belongs to separate GTlab 3/type-system work;
operations migrate with the other module interfaces when that work is ready.

Three lifetimes remain separate:

.. code-block:: text

   originating operation       execution-local operation       detached data/result
   ---------------------       -------------------------       --------------------
   client-owned through        runtime-owned through            independently owned
   applyResult()               terminal state/result capture     GtObject trees

An execution-local operation in another process is reconstructed from its
Memento; it is not the originating C++ object. Detached data/results contain no
borrowed authoritative-project pointers and cross boundaries through normal
GTlab serialization.

Minimal operation execution context
-----------------------------------

Each submission creates one ``GtOperationExecutionContext`` valid for the
complete ``execute()`` call. Its minimum contract is:

* ``data()`` returns a non-owning pointer to optional detached invocation data.
  The runtime has sole ownership for the invocation; the operation may mutate it
  during execution but must not retain its pointer afterward.
* ``executionId()`` returns immutable, opaque, globally unique invocation
  identity. The execution handle and every event use this same identity.
* ``events()`` returns the invocation event sink. Publishing enters the ordered
  execution event stream; runtime serialization occurs before local observers
  or a transport adapter see it.
* ``cancellation()`` returns the invocation cancellation token. Its request
  state is safe to observe while another thread requests cancellation.

The context never owns project state. The runtime separately establishes
``GtExecutionContextScope`` before ``execute()`` for project-bound operations.

Runtime and asynchronous execution
----------------------------------

``GtHeadlessProjectRuntime`` is the execution-side single-project application
boundary. It owns one project lifecycle and runtime-owned operation state, but
is broader than an executor: short project reads/mutations need not all become
asynchronous operations.

.. code-block:: text

   one-shot worker / future resident session
                     |
                     v
       GtHeadlessProjectRuntime (one project)
          | project lifecycle, context, mutation coordination
          | async operation submission, handles, cancellation, events
          v
       GtExecutableOperation::execute()

``submitOperation(...)`` returns a generic asynchronous execution handle before
expensive work blocks the submitting path. This is invalid::

   operation->execute(context);
   return alreadyFinishedHandle;

The handle is copyable and value-like: copies refer to one runtime-owned state,
not copied project, task, operation, or result objects. It exposes no
``GtObject*``, ``GtProject*``, ``GtTask*``, or other process-local pointer. At
minimum it provides execution identity, status inspection, cancellation request,
optional blocking wait, terminal result access, and structured terminal error
access. Runtime state remains valid while handles refer to it. Close or shutdown
drives every affected handle to a defined terminal state, never a dangling one.

``cancel()`` requests cancellation; acceptance of that request is distinct from
the operation reaching a cancelled/terminated terminal state. Status inspection
and request state are thread-safe and callable independently of the thread
blocked in ``execute()``. The token observed by the operation has the same
cross-boundary safety. A design where cancellation can only be requested after
``execute()`` returns violates the asynchronous contract.

``execute()`` remains synchronous on the chosen context. The runtime owns
scheduling, state transitions, cancellation, and completion. It may use an
owner thread, non-blocking machinery, safe worker threads, or marshalling; it
must not blindly move Qt/GTlab project objects to arbitrary threads. The
invariant is that long-running execution does not make status, cancellation, or
event observation unusable. Close/destruction are bounded and mutation uses the
established execution context and guard/serialization mechanisms.

The detached ``GtObject`` returned by ``execute()`` is the runtime result. It is
available through the handle only in the appropriate successful terminal state;
failure, cancellation, close, or shutdown does not expose a partial result as a
successful completion.

Completion and event model
--------------------------

.. code-block:: text

   runtime completion                     future client completion
   ------------------                     ------------------------
   execute() finished                     runtime completion
   + detached result/failure available    + transport/reconstruction
                                          + applyResult() succeeded

Worker Slice 1 implements only runtime completion. A future client-side
``GtOperationExecutor`` owns preparation, provisioning/transport,
reconstruction, ``applyResult()``, and its failure state. The runtime never
applies a result to the originating project.

Operations publish observations through the execution context, not a transport
or GUI. A transport-neutral logical event envelope has these required fields:

* ``executionId``: invocation identity;
* ``sequence``: zero-based, strictly increasing unsigned per-execution order;
* ``eventType``: non-empty domain event key; and
* ``payload``: absent, a JSON value tree (null, boolean, number, string, list,
  or string-keyed object), or a detached serializable ``GtObject`` Memento with
  an identifying payload encoding.

Payloads contain no process-local pointers. Qt signals/slots are preferred local
observation; stdio is a transport adapter. Events are ordered per execution,
connections are established before ``execute()``, and events are observations,
not GUI instructions.

Task integration and worker boundary
------------------------------------

``GtCoreProcessExecutor`` remains task-specific. Existing code enters through
``GtTask / Calculator -> ProcessTaskOperation -> generic runtime``. The adapter
may reuse the executor, but generic runtime state does not depend on it,
``GtTask*``, task lookup, task groups, or task-progress states.

Worker Slice 1 depends on the generic runtime from #1515. The batch command is
a boundary adapter: it reconstructs serialized project, operation, and optional
data, configures stdio, and delegates lifecycle to
``GtHeadlessProjectRuntime::submitOperation(...)``. It does not construct an
execution context and invoke ``operation.execute()`` as an architectural path.

Stdio V1
--------

V1 records are one UTF-8 stdout line beginning at byte zero with
``@gtlab-operation-v1 `` followed by compact JSON; newline is the boundary. All
records contain ``version`` (integer ``1``), ``kind``, and ``executionId``.
``kind`` is exactly ``event``, ``result``, or ``failure``.

* ``event`` additionally contains ``sequence``, ``eventType``,
  ``payloadEncoding``, and ``payload``. ``payloadEncoding`` is ``json`` for the
  logical JSON value tree or ``memento-xml-base64`` for UTF-8 Memento/XML bytes
  encoded with standard Base64. ``payload`` is respectively the JSON value or a
  Base64 string.
* ``result`` additionally contains ``resultEncoding`` and ``result``.
  ``resultEncoding`` is ``null`` for a valid null result or
  ``memento-xml-base64`` for a detached-result Memento. ``result`` is JSON null
  or the Base64 string respectively.
* ``failure`` additionally contains stable ``errorCode``, human-readable
  ``message``, and optional JSON-value ``details``.

Compact JSON plus Base64 guarantees no unescaped newline in a protocol record.
Each execution emits zero or more events followed by exactly one terminal
``result`` or ``failure`` record; no later event or second terminal record is
valid. Protocol writes are serialized as whole records, so another thread's
ordinary/log output cannot interleave bytes inside a prefixed record.
Non-prefixed stdout is ordinary output; decoders ignore or forward it separately
and never treat every stdout line as protocol data. V1 excludes replay,
reconnect, broker, queue, cluster, and a general remote protocol.

Migration and open details
--------------------------

Generalize PR #1520's lifecycle, runtime-owned state, value handles,
status/cancel/wait, terminal snapshots, bounded shutdown, owner-thread/event
loop integration, explicit context, and mutation coordination. Replace task
handles/statuses and ``submitTask()`` with generic equivalents; remove
``GtTask*`` and ``GtCoreProcessExecutor`` as generic dependencies. Task
addressing can later be a task-specific convenience API.

Internal scheduler classes, concrete structured-result spelling, GUI integration,
capability matching, remote replay/reconnect, and resident-session
synchronization remain open and must not change these boundaries.
