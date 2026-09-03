Executable operations and the one-shot worker slice
====================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issue `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_
:Related: `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_, `#1527 <https://github.com/dlr-gtlab/gtlab-core/issues/1527>`_, and `PR #1520 <https://github.com/dlr-gtlab/gtlab-core/pull/1520>`_

Context and decision
--------------------

This ADR materializes the decisions in #1526 for Worker Slice 1. It is not a
new architecture: later issues may resolve only details explicitly left open in
#1526 and must not introduce competing execution, serialization, registration,
result-application, or project-lifecycle models.

Executable domain logic is independent of location and transport. An executable
operation is a normal ``GtObject``:

.. code-block:: cpp

   class GtExecutableOperation : public GtObject
   {
   public:
       virtual bool requiresProject() const = 0;
       virtual std::unique_ptr<GtObject>
       createData(const GtExecutionContext& context) const = 0;
       virtual std::unique_ptr<GtObject>
       execute(GtOperationExecutionContext& context) = 0;
       virtual void applyResult(/* result */, GtExecutionContext& context) const = 0;
   };

The final ``applyResult()`` spelling is open, but its responsibilities are not:

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
that makes a result authoritative in that project.

Project state is not operation data. ``requiresProject()`` asks whether the
execution location needs a provisioned project; it does not limit client-side
preparation. ``GtExecutionContext`` remains the project context, with a
``GtExecutionContextScope`` for project-bound execution.
``GtOperationExecutionContext`` is invocation-local: detached data, event sink,
cancellation, and execution identity. It does not own, load, or synchronize a
project.

Serialization, registration, and ownership
-------------------------------------------

Operations, detached data, and results reuse GTlab Memento/XML and
``GtObjectFactory``. No operation-specific serializer, factory, or worker-side
registry is introduced. A worker reconstructs its boundary object normally and
rejects it before execution if it is not a ``GtExecutableOperation``.

``GtOperationInterface`` is the authoritative optional module declaration of
operation types. The module loader validates declarations and makes types
reconstructable through existing object/type factories. There is no second
operation factory, global ``GtOperationRegistry``, or public discovery API
initially. New extension APIs expose stable GTlab ``GtTypeId``, not
``QMetaObject``; its internal representation may use existing Qt/factory
mechanics without becoming public API.

Three lifetimes remain separate:

.. code-block:: text

   originating operation       execution-local operation       detached data/result
   ---------------------       -------------------------       --------------------
   client-owned through        runtime-owned through            independently owned
   applyResult()               terminal state/result capture     GtObject trees

An execution-local operation in another process is reconstructed from its
Memento; it is not the originating C++ object. Detached data/results contain no
borrowed authoritative-project pointers and cross boundaries via normal GTlab
serialization.

Runtime and asynchronous execution
----------------------------------

``GtHeadlessProjectRuntime`` is the execution-side, single-project application
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

Submission returns a generic value-based execution handle before expensive work
blocks the caller. This is invalid::

   operation->execute(context);
   return alreadyFinishedHandle;

``execute()`` remains synchronous on the chosen execution context. The runtime
owns scheduling, state transitions, cancellation, and completion. It may use
owner-thread execution, non-blocking machinery, safe worker threads, or
marshalling; it must not blindly move Qt/GTlab project objects to arbitrary
threads. The invariant is that a long-running ``execute()`` does not make
status, cancellation, or event observation unusable before returning.

The generic handle carries identity, status, events, cancellation, optional
waiting, and stable terminal result/error snapshots. Its final names, states,
ownership signature, retention, and cancellation mechanics are follow-up
details. It exposes no public ``GtTask*``, ``GtProject*``, or other Qt/GTlab
object pointer. Close/destruction resolve active states deterministically with
bounded shutdown; project mutation uses established context and guard/
serialization mechanisms.

Completion, events, and task integration
-----------------------------------------

.. code-block:: text

   runtime completion                     future client completion
   ------------------                     ------------------------
   execute() finished                     runtime completion
   + detached result/failure available    + transport/reconstruction
                                          + applyResult() completed

Worker Slice 1 implements only runtime completion. A future client-side
``GtOperationExecutor`` owns preparation, provisioning/transport,
reconstruction, and ``applyResult()``. The runtime never applies a result to
the originating project.

Operations publish observations through the execution context, not a transport
or GUI:

.. code-block:: text

   operation -> GtOperationExecutionContext -> event sink / local stream
                                                        |             |
                                                Qt signals/slots   transport adapter

Qt signals/slots are preferred local observation. Events are ordered per
execution, connected before ``execute()``, and are observations, not GUI
instructions.

``GtCoreProcessExecutor`` remains task-specific. Existing code enters through
``GtTask / Calculator -> ProcessTaskOperation -> generic runtime``. The adapter
may reuse the executor, but generic runtime state must not depend on it,
``GtTask*``, task lookup, task groups, or task-progress states.

Worker boundary and stdio V1
----------------------------

The batch command is a boundary adapter around ``GtHeadlessProjectRuntime``: it
reconstructs serialized project/operation/optional data and configures stdio.
It does not own a second lifecycle or architecturally call ``execute()``
directly.

V1 records are one UTF-8 stdout line beginning at byte zero with
``@gtlab-operation-v1 `` followed by compact JSON; newline is the boundary. JSON
contains ``kind`` and ``executionId``. ``kind`` is ``event``, ``result``, or
``failure``. An event carries an ordered domain observation. Result/failure are
terminal and carry detached-result or structured-failure metadata/payload.
#1529 owns field names and Memento payload encoding, but encoders emit at most
one terminal record. Non-prefixed stdout is ordinary output; decoders never
assume every line is protocol data and ignore or forward it separately. V1 does
not define replay, reconnect, broker, queue, cluster, or a general remote
protocol.

Migration and open details
--------------------------

Generalize PR #1520's lifecycle, runtime-owned state, value handles,
status/cancel/wait, terminal snapshots, bounded shutdown, owner-thread/event
loop integration, explicit context, and mutation coordination. Replace task
handles/statuses and ``submitTask()`` with generic equivalents; remove
``GtTask*`` and ``GtCoreProcessExecutor`` as generic dependencies. Task
addressing can later be task-specific convenience API.

Final public signatures, handle retention, cancellation threading, detailed
event payloads, runtime marshalling, GUI integration, capability matching,
remote replay/reconnect, and resident-session synchronization remain open and
must be resolved without changing these boundaries.
