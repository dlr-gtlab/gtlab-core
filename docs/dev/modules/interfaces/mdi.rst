.. _mdiinterface:

User-interface contributions
============================

Use :cpp:class:`GtMdiInterface` when a module contributes GUI classes. Despite
its historical name, the interface covers several distinct extension points:

.. list-table:: GUI registration functions
   :header-rows: 1
   :widths: 25 75

   * - Function
     - Contribution
   * - ``mdiItems()``
     - object-oriented viewers and editors opened in GTlab's central workspace
   * - ``dockWidgets()``
     - persistent tool widgets available from the window menu
   * - ``uiItems()``
     - mappings from data-model classes to their ``GtObjectUI`` classes
   * - ``postItems()``
     - post-processing template classes
   * - ``postPlots()``
     - plot classes available to post-processing

Only override the functions the module needs; all registration functions
default to empty collections.

.. code-block:: cpp

  #include "gt_mdiinterface.h"
  #include "gt_moduleinterface.h"

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtMdiInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtMdiInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QList<QMetaObject> mdiItems() override;
      QList<QMetaObject> dockWidgets() override;
      QMap<const char*, QMetaObject> uiItems() override;
  };

Register viewers and dock widgets
---------------------------------

Return the metadata of each concrete GUI class:

.. code-block:: cpp

  QList<QMetaObject> MyModule::mdiItems()
  {
      return {GT_METADATA(MyObjectViewer)};
  }

  QList<QMetaObject> MyModule::dockWidgets()
  {
      return {GT_METADATA(MyToolDockWidget)};
  }

An MDI item is associated with a compatible data-model object and opens in the
central workspace. A dock widget provides a module-wide tool and should not be
used as a substitute for an editor tied to one object.

Map objects to UI classes
-------------------------

``uiItems()`` maps the registered name of a data-model class to the metadata
of its UI class:

.. code-block:: cpp

  QMap<const char*, QMetaObject> MyModule::uiItems()
  {
      return {{
          GT_CLASSNAME(MyPackage),
          GT_METADATA(MyPackageUI)
      }};
  }

The mapping lets GTlab present object-specific actions and appearance without
placing widget code in the persistent data class. A module can provide UI for
classes owned by another module, but that creates a runtime relationship: test
behavior when the providing module is unavailable or incompatible.

Post-processing classes
-----------------------

``postItems()`` registers post-processing template items, while
``postPlots()`` registers plot implementations used by those views. Return Qt
metadata for the concrete classes, just as for MDI items. Modules that do not
extend post-processing should leave both functions untouched.

GUI code requires the GUI application and should link ``GTlab::Gui``. Keep
domain state and calculations outside widgets so the same data and process
features remain usable from the batch application.
