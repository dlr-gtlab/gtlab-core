Executable operations and one-shot workers
==========================================

This guide applies the accepted :doc:`operation ADR
<decisions/0001-executable-operations-one-shot-worker>` for module, runtime, and
worker implementers. It describes the implemented Core foundation and the
follow-up runtime and worker contracts. #1528 introduces only the
transport-neutral operation API; execution scheduling, event streaming, and
worker transport remain follow-up work. #1526 remains the normative
architecture source.

Core foundation
---------------

Core provides ``GtExecutableOperation`` together with ``GtOperationExecutionContext``,
``GtExecutionId``, ``GtCancellationToken``, and the structured
``GtOperationApplyResult``. The context carries borrowed detached data, an
invocation identity, direct access to the invocation-local
``GtExecutionEventStream``, and shared cooperative cancellation state; it does
not carry project state.

The event stream is a concrete Qt-based publisher/observable object rather than
an observer/sink interface hierarchy. Operations publish through
``context.events().publish(...)``; local consumers and process-boundary adapters
observe the resulting logical events through Qt signals/slots. The stream itself
has no stdout, file, GUI, or remote-control-plane dependency.

Use ``GtOperationInterface::operations()`` to declaratively contribute operation
``QMetaObject`` values. The normal module loader validates each declaration as an
invokable ``GtExecutableOperation`` and registers it through ``GtObjectFactory``.
Operations, data, and results use existing Memento/XML reconstruction.

Operation authors
-----------------

Implement a ``GtExecutableOperation`` as transport-neutral domain logic:

.. code-block:: text

   createData()   client thread: read authoritative project; make detached input
   execute()      execution side: compute synchronously; return detached result
   applyResult()  client thread: commit to the authoritative project

Preparation and commit must be lightweight; expensive work belongs in
``execute()``. Do not create processes, select remote endpoints, implement
private transport, write GUI state, or mutate the originating project from an
operation. Use detached ``GtObject`` trees with no borrowed project pointers.
``requiresProject()`` concerns project availability at the execution location.
Use ``GtOperationExecutionContext`` for invocation data, events, cancellation,
and identity; project access is separately scoped through ``GtExecutionContext``.

Module authors
--------------

Declare operation types through the optional ``GtOperationInterface`` using the
existing GTlab ``QMetaObject`` module convention:

.. code-block:: cpp

   class MyOperationInterface : public GtOperationInterface
   {
   public:
       QList<QMetaObject> operations() const override
       {
           return {MySimulationOperation::staticMetaObject};
       }
   };

The module loader validates these declarations and registers them through the
existing ``GtObjectFactory`` and normal GTlab type infrastructure. Do not add an
operation registry, second factory, ``GtTypeId``, ``GtTypeDescriptor``,
operation-specific metadata wrapper, or placeholder ``GtMetaObject``. A future
``GtMetaObject`` migration belongs to the broader GTlab type-system work and
will migrate operations together with the other module interfaces.

Runtime implementers
--------------------

``GtHeadlessProjectRuntime`` owns exactly one execution-side project lifecycle
and operation execution state. ``submitOperation(...)`` (working name) returns
a generic value handle before expensive work blocks the caller. ``execute()``
remains synchronous but status, cancellation, and event observation remain
usable while it runs. Respect project/Qt affinity through safe scheduling or
marshalling; do not fake asynchrony or move arbitrary project objects to a
background thread. Use explicit ``GtExecutionContext`` and the existing project
mutation guard. Close/destruction resolves active work deterministically with
bounded shutdown.

Keep lifecycle boundaries separate:

.. code-block:: text

   runtime: execute() + detached result/failure
   client:  runtime completion + transport/reconstruction + applyResult()

The runtime owns only the first. A future client ``GtOperationExecutor`` owns
the second; the runtime never calls ``applyResult()`` on an originating project.

Event observation
-----------------

``GtExecutionEventStream`` is the operation-facing event publisher and local
observable stream for one invocation. It assigns execution identity and strict
per-execution sequence order and emits logical ``GtExecutionEvent`` values via
Qt signals/slots. Writers and adapters connect as ordinary Qt observers; there
is no separate ``GtExecutionEventSink`` interface.

Logical event payloads remain transport-neutral. A detached object payload is
represented logically as Memento/XML; textual adapters such as stdio V1 may
encode those bytes as Base64, but Base64 is not part of the logical event type.

Worker implementers
-------------------

The one-shot worker is an adapter, not a second runtime. Reconstruct project,
operation, and optional detached data through normal Memento/ObjectFactory
mechanisms, validate ``GtExecutableOperation`` before submission, configure the
stdio adapter, and delegate lifecycle to ``GtHeadlessProjectRuntime``.

Emit V1 protocol records as one UTF-8 line starting exactly with
``@gtlab-operation-v1 `` and compact JSON containing ``kind`` and
``executionId``. Kinds are ``event``, ``result``, and ``failure``; result and
failure are terminal, with no second terminal record. Non-prefixed stdout lines
are ordinary output, never protocol. Decoders ignore or forward them separately
and must not assume every stdout line is JSON protocol data.

Existing tasks
--------------

Existing ``GtTask`` and calculator code remains unchanged. It enters through
``ProcessTaskOperation``, which may reuse ``GtCoreProcessExecutor`` and translate
task details into generic events/cancellation. Do not place task identifiers,
lookup, executor-specific state, or task-specific handles in the generic
runtime.

Prototype migration
-------------------

PR #1520 supplies useful runtime mechanics: one project, runtime-owned state,
value handles, polling/cancel/wait, terminal snapshots, bounded shutdown,
owner-thread behavior, explicit context, and mutation coordination. Generalize
these mechanics to executable operations. Its task handles/statuses,
``submitTask()``, ``GtTask*`` identity, and direct dependency on
``GtCoreProcessExecutor`` are not the public model.
