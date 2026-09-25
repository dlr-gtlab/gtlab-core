Executable operations
=====================

Executable operations describe computational work independently of where it
runs. The same operation can be executed in another thread, in a separate local
process, or on a remote worker. The execution infrastructure chooses the
location; the operation contains only the domain logic. This allows GTlab to keep
expensive work out of the interactive application, isolate computations, or use
remote resources without changing the operation.

Operations are broader than tasks. Existing ``GtTask`` and calculator
implementations remain unchanged and can be exposed as operations through an
adapter. The operation model is not tied to the ``GtTask`` lifecycle or
``GtCoreProcessExecutor`` and can also represent module-specific algorithms or
Intelligraph computations.

Operation lifecycle
-------------------

An operation separates preparation, computation, and project updates:

.. code-block:: text

   originating side       GtOperationExecutor/backend       execution side
   ---------------        --------------------------        --------------
   createData()  -------> provision operation and data --> GtExecutionEnvironment
   applyResult() <------- return full outcome <----------- execute()

``createData()`` prepares optional input from the originating project.
``applyResult()`` receives the full operation outcome and interprets its domain
meaning on that project. Both run on the originating side and must be fast.

The planned client-side ``GtOperationExecutor`` will own asynchronous
submission, backend selection, cancellation, and transport of operation
outcomes. Cancellation remains effective until the originating side starts
``applyResult()``. If requested before then, the executor skips that call but
may still return or expose the detached result payload to the client. Once
``applyResult()`` starts, cancellation does not interrupt that commit step. The
executor does not classify an outcome or decide whether a payload is
semantically applicable; the operation interprets the full outcome in
``applyResult()``.

``execute()`` contains the potentially expensive work. It is synchronous in
the calling thread, while the planned ``GtOperationExecutor`` client-side layer
will manage the caller's asynchronous lifecycle. ``GtExecutionEnvironment``
scopes the execution-local project and calls the operation. The selected
backend provisions the operation, optional data, event stream, and required
project for that thread. This contract works in a dedicated GUI execution
thread, a one-shot worker, a resident project worker, or a future remote worker.
The operation uses only execution-local state and services from
``GtOperationExecutionContext``; it must not access the originating project
directly.

``requiresProject()`` states whether this invocation needs a GTlab project
at the execution location. It does not select the execution location. When it
returns ``true``, ``GtExecutionEnvironment`` requires its borrowed project and
installs it in ``GtExecutionContextScope`` for the call. When it returns
``false``, the environment installs an explicitly empty context, so
``gtApp->currentProject()`` and ``gtDataModel->currentProject()`` return
``nullptr`` even if a GUI session has a selected project.

The project may be used in addition to detached input from ``createData()``.
Reading the originating project in ``createData()`` does not require a complete
project during execution.

There are three common cases:

* ``requiresProject()`` returns ``false`` because the operation state and
  optional detached input contain everything needed;
* ``requiresProject()`` returns ``true`` and the project at the execution
  location contains everything needed, so ``createData()`` returns ``nullptr``;
  or
* ``requiresProject()`` returns ``true`` and ``execute()`` needs both the
  project and additional detached input.

If only selected parts of the originating project are needed, prefer extracting
them in ``createData()`` and return ``false`` from ``requiresProject()``. This
avoids providing a complete project at the execution location.

An operation does not create threads or processes to implement execution
placement or asynchronous submission. Domain-specific internal parallelism
remains the responsibility of the operation or task. Scheduling, data transfer,
and execution lifecycle belong to the execution infrastructure.

Operation outcomes and transport
--------------------------------

An operation outcome is the complete ``GtOperationExecutionResult``; its
result payload is the optional ``GtObject`` it contains. The value type is
transport-neutral and is not a ``GtObject``. It contains a ``Success``,
``Failed``, or ``Cancelled`` status, optional ``code`` and ``message`` values,
and an optional result payload. Any status may carry a payload; there is no
generic ``PartialResult`` status.

``GtExecutionResult`` separately represents the environment boundary. A normal
domain failure is ``Error::None`` with an operation outcome whose status is
``Failed``. ``ProjectRequired``, ``WrongThread``, and ``UnhandledException`` are
boundary failures and have no operation outcome.

Transport adapters encode ``status``, ``code``, and ``message`` directly.
Only the optional ``GtObject`` result payload uses Memento and
``GtObjectFactory`` serialization. ``GtExecutionEnvironment`` performs no
serialization.

GTlab integration
-----------------

``GtExecutableOperation`` is a normal ``GtObject``. Operations, input data,
and result payloads use GTlab properties, ``GtObjectFactory``, and Memento/XML.
No operation-specific object model or serializer is needed.

Modules declare their operation classes through ``GtOperationInterface``. See
the :ref:`operationinterface` documentation for registration details.

Each execution receives a ``GtOperationExecutionContext``. It provides the
detached input, execution identity, cancellation state, and event stream.
Project access continues to use ``GtExecutionContext``; project state is not
part of the operation context.

The environment borrows, but does not open, save, close, or own its
execution-local project. It does not initialize Core or a session, schedule
work, create or move threads, or move execution data between locations. The
backend owns provisioning and placement. Cancellation is cooperative during
``execute()``; the environment preserves the returned
status and reports escaping exceptions as boundary errors.

The accepted :doc:`architecture decision
<decisions/0001-executable-operations>` records the operation lifecycle,
registration, event model, and the responsibility split for the executor,
environment, and worker adapters.
