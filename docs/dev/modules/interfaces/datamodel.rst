.. _datamodelinterface:

Data-model interface
====================

Use :cpp:class:`GtDatamodelInterface` when a module contributes persistent
domain objects to GTlab projects. It registers one package class and any number
of :cpp:class:`GtObject`-derived data classes. GTlab then uses its object
factories and serialization system to create, copy, save, and restore them.

.. code-block:: cpp

  #include "gt_datamodelinterface.h"
  #include "gt_moduleinterface.h"

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtDatamodelInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtDatamodelInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QMetaObject package() override;
      QList<QMetaObject> data() override;
      bool standAlone() override;
  };

Register the package and objects
--------------------------------

``package()`` returns the metadata of the module's package class. ``data()``
returns the metadata of every other persistent class provided by the module:

.. code-block:: cpp

  QMetaObject MyModule::package()
  {
      return GT_METADATA(MyPackage);
  }

  QList<QMetaObject> MyModule::data()
  {
      return {
          GT_METADATA(MyComponent),
          GT_METADATA(MyOperatingPoint)
      };
  }

Register every concrete class that can appear in a saved project. Otherwise,
GTlab cannot reconstruct that class when opening the project. The registered
class name is therefore part of the persisted data format; renaming classes
requires a deliberate migration strategy.

Choose standalone behavior
--------------------------

``standAlone()`` tells GTlab whether the module owns an independent package in
the project structure:

* return ``true`` for a domain module with its own package and data tree;
* return ``false`` when the module only contributes types or functionality to
  data structures owned elsewhere.

This choice affects how users encounter module data in a project. Decide it
from the module's domain model, not from whether the module can be distributed
independently.

Data-model classes should keep domain state in GTlab properties and follow the
normal ``GtObject`` parent-child ownership model. See
:doc:`../../basics/data_modelling` before defining a large class hierarchy.
