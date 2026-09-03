.. _operationinterface:

Operation interface
===================

Use :cpp:class:`GtOperationInterface` when a module contributes
transport-neutral :cpp:class:`GtExecutableOperation` types. It declares the
operation classes that a module makes reconstructable for execution; it does
not select an execution location or transport.

.. code-block:: cpp

  #include "gt_executableoperation.h"
  #include "gt_moduleinterface.h"
  #include "gt_operationinterface.h"

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtOperationInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtOperationInterface)

  public:
      QList<QMetaObject> operations() const override
      {
          return {MyOperation::staticMetaObject};
      }
  };

Register operation types
------------------------

``operations()`` returns the ``staticMetaObject`` of every executable
operation class contributed by the module. Each declared type must derive from
``GtExecutableOperation`` and provide an invokable constructor so normal
``GtObjectFactory``/Memento reconstruction can create it.

The module loader validates and registers these types in ``GtObjectFactory``.
Do not add a second operation factory or a separate registration call. The
module declaration only makes the operation type available; it neither creates
an operation instance nor starts execution.

Implement the operation contract
--------------------------------

Operations prepare optional detached input through ``createData()``, perform
their potentially expensive synchronous compute step through ``execute()``, and
make results authoritative only through ``applyResult()``. Keep preparation and
result application lightweight; execution infrastructure owns scheduling,
location, and transport. See :doc:`../../architecture/executable_operations`
for the complete operation lifecycle and ownership contract.
