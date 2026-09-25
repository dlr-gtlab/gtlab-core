.. `toctree`

.. _parametric-api:

*************
API Reference
*************

This reference lists the public GTlab Core types intended for module authors
and Core contributors. For architectural guidance and usage patterns, see the
:doc:`executable operation guide <architecture/executable_operations>` and the
other developer documentation.

.. _core-api:

Data Processor API
==================


.. doxygenclass:: GtObject
   :members:

.. doxygenclass:: GtAbstractProperty
   :members:

.. doxygenclass:: GtObjectLinkProperty
   :members:

.. doxygenclass:: GtObjectPathProperty
   :members:


Execution API
=============

The execution API provides the common Core contracts for placement-neutral
operation execution. The same operation model is used for in-process,
threaded, isolated-process, and future remote execution.

Executable operations
---------------------

.. doxygenclass:: GtExecutableOperation
   :members:

.. doxygenclass:: GtExecutionEnvironment
   :members:

.. doxygenstruct:: GtOperationExecutionResult
   :members:

.. doxygenclass:: GtExecutionResult
   :members:

Execution context
-----------------

.. doxygenclass:: GtOperationExecutionContext
   :members:

.. doxygenclass:: GtCancellationToken
   :members:

.. doxygenclass:: GtExecutionId
   :members:

.. doxygenclass:: GtExecutionContext
   :members:

.. doxygenclass:: GtExecutionContextScope
   :members:

Results and events
------------------

.. doxygenclass:: GtOperationApplyStatus
   :members:

.. doxygenclass:: GtExecutionEventStream
   :members:

.. doxygenclass:: GtExecutionEvent
   :members:

Module Interfaces
=================

.. doxygenclass:: GtModuleInterface
   :members:

.. doxygenclass:: GtDatamodelInterface
   :members:

.. doxygenclass:: GtProcessInterface
   :members:

.. doxygenclass:: GtMdiInterface
   :members:

.. doxygenclass:: GtPropertyInterface
   :members:

.. doxygenclass:: GtImporterInterface
   :members:

.. doxygenclass:: GtExporterInterface
   :members:

.. doxygenclass:: GtCollectionInterface
   :members:

.. doxygenclass:: GtNetworkInterface
   :members:

.. doxygenclass:: GtCalculatorExecInterface
   :members:

.. doxygenclass:: GtOperationInterface
   :members: