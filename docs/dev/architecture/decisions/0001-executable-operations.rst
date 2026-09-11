Architecture decision 0001 — Executable operations
===================================================

:Status: Accepted
:Date: 2026-09-03
:Normative source: GitHub issue `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_
:Related: `#1515 <https://github.com/dlr-gtlab/gtlab-core/issues/1515>`_, `#1528 <https://github.com/dlr-gtlab/gtlab-core/issues/1528>`_, `#1529 <https://github.com/dlr-gtlab/gtlab-core/issues/1529>`_, `#1530 <https://github.com/dlr-gtlab/gtlab-core/issues/1530>`_, and `#1531 <https://github.com/dlr-gtlab/gtlab-core/issues/1531>`_

Context
-------

GTlab needs to run computational work in different execution locations without
duplicating domain logic or coupling modules to communication between processes
or machines. The :doc:`../executable_operations` guide introduces the operation
model. This decision records the constraints that implementations must preserve.

Decision
--------

Operation model
~~~~~~~~~~~~~~~

``GtExecutableOperation`` is a normal ``GtObject`` with the lifecycle
``createData()`` → ``execute()`` → ``applyResult()``.

* ``createData()`` and ``applyResult()`` run on the originating side and must
  be fast.
* ``execute()`` is the synchronous, potentially expensive execution-side step.
* Only ``applyResult()`` updates the originating project with the result.
* ``requiresProject()`` states whether the execution location needs a project.

Operations, detached input, and detached results use ``GtObjectFactory`` and
Memento/XML. Modules register operation classes through
``GtOperationInterface``. There is no separate operation factory, registry, or
serializer.

Execution state
~~~~~~~~~~~~~~~

The originating operation, an execution-local reconstructed operation, and the
detached input/result are separate objects with separate lifetimes. Detached
objects must not contain borrowed pointers into the originating project.

``GtOperationExecutionContext`` contains invocation data, execution identity,
cancellation, and event publication. It does not own project state. A project
at the execution location remains available through ``GtExecutionContext``.

Operation submission is asynchronous for the caller, while ``execute()`` stays
synchronous. The runtime owns scheduling, status, cancellation, and completion.
These controls must remain usable while ``execute()`` is running. Project and
Qt thread-affinity rules still apply.

``GtHeadlessProjectRuntime`` is the execution-side boundary for one project. It
owns the execution-local operation and its result. A client-side executor owns
preparation, transfer, reconstruction, ``applyResult()``, and client-visible
completion.

Events
~~~~~~

An operation can report events while ``execute()`` is running. These events tell
the client what is happening before the final result is available. The client
can use them to update its status display or start a follow-up action. Events do
not update the originating project and do not replace the operation result.

The #1528 Core foundation exposes ``GtExecutionEventSink`` through
``GtOperationExecutionContext``. This is only the publication boundary. The
event type, payload, local observation, and transport are defined by #1529.
Operation code does not write directly to a transport or GUI.

Task and worker integration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Existing ``GtTask`` and calculator code remains unchanged. A
``ProcessTaskOperation`` adapter connects it to the operation/runtime model and
may reuse ``GtCoreProcessExecutor`` internally. Generic operation code does not
depend on task lookup or task-specific state.

A worker is an adapter around ``GtHeadlessProjectRuntime``. It reconstructs
GTlab objects through the normal factories, configures boundary adapters, and
submits the operation to the runtime. It does not introduce another execution
lifecycle.

Consequences
------------

* Module code can use one operation implementation in different execution
  locations.
* Operations continue to use the existing GTlab object registration and
  serialization mechanisms.
* Runtime, transport, and GUI integrations can evolve without changing domain
  operations.
* Broker, queue, cluster, replay, reconnect, resident-session synchronization,
  and GUI interaction are separate work.
