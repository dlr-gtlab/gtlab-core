Executable operations and one-shot workers
==========================================

This guide applies the accepted :doc:`operation ADR
<../adr/0001-executable-operations-one-shot-worker>` for module, runtime, and
worker implementers. It describes the planned contract, not APIs implemented by
this change. #1526 remains the normative architecture source.

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
