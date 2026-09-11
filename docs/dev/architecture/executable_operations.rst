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

   originating GTlab                         execution location
   -----------------                         ------------------
   createData()  ---- detached input ---->   execute()
   applyResult() <--- detached result -----

``createData()`` prepares optional input from the originating project.
``applyResult()`` applies the result to that project. Both run on the
originating side and must be fast.

``execute()`` contains the potentially expensive work. It is synchronous, but
the surrounding execution infrastructure may schedule it asynchronously. It
works only with execution-local state: the operation itself, detached input, an
optional project at the execution location, and services from
``GtOperationExecutionContext``. It must not access the originating project
directly.

``requiresProject()`` states whether a GTlab project must be available to
``execute()`` at the execution location. This project may be used in addition
to detached input from ``createData()``. Reading the originating project in
``createData()`` does not require a complete project during execution.

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

The operation does not create threads or processes and does not select remote
workers. Scheduling, data transfer, and execution lifecycle belong to the
execution infrastructure.

GTlab integration
-----------------

``GtExecutableOperation`` is a normal ``GtObject``. Operations, input data, and
results therefore use GTlab properties, ``GtObjectFactory``, and Memento/XML.
No operation-specific object model or serializer is needed.

Modules declare their operation classes through ``GtOperationInterface``. See
the :ref:`operationinterface` documentation for registration details.

Each execution receives a ``GtOperationExecutionContext``. It provides the
detached input, execution identity, cancellation state, and event stream.
Project access continues to use ``GtExecutionContext``; project state is not
part of the operation context.

The accepted :doc:`architecture decision
<decisions/0001-executable-operations>` records the technical constraints for
runtime, event, and worker implementations.
