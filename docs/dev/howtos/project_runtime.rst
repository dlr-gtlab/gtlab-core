Single-project runtime
======================

``GtProjectRuntime`` owns one project at the location where an operation runs.
It can also execute operations that need no project. A hosting application or
backend places the runtime and its GTlab Core services in a compatible Qt
thread; the runtime does not create or select that thread.

Execution is synchronous. The caller constructs the operation and any detached
input before calling ``executeOperation()``. It also supplies an event stream
and cancellation token that remain available during the call. The runtime
constructs ``GtOperationExecutionContext``, installs a project execution
context when required, and returns a detached result or a structured failure.
It neither calls ``createData()`` nor applies the result to the originating
project.

Minimal usage
-------------

.. code-block:: cpp

   GtProjectRuntime runtime;
   auto ready = runtime.initialize();
   if (ready)
   {
       GtExecutionEventStream events{GtExecutionId{}};
       GtCancellationToken cancellation;
       auto outcome = runtime.executeOperation(std::move(operation),
                                               std::move(input), events,
                                               cancellation);
       // executeOperation() has finished; outcome.result is detached.
   }

Connect event observers and make the cancellation token requestable before
calling ``executeOperation()``. An operation must cooperate with cancellation;
the runtime cannot forcibly interrupt it. The host must use a separate thread
or process if the initiating thread must remain responsive. Hard interruption
and result transport belong to that host, not to the runtime.

Lifecycle
---------

The lifecycle is ``Created -> Initialized -> ProjectLoaded -> Closed``. Project-
free operations may run in ``Initialized`` state. A project-required operation
needs ``openProject()`` first. A second project is rejected. ``saveProject()``
and ``closeProject()`` reject a project guarded by active execution. All runtime
methods and destruction must run on its Qt thread; Core application and data
model services must have matching thread affinity.
