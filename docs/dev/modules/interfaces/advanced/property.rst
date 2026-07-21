.. _propertyinterface:

Property editor interface
=========================

Use :cpp:class:`GtPropertyInterface` only when a custom property type needs a
specialized representation or editor in GTlab's property view. Creating a new
property type and registering it in a data object does not by itself require
this GUI interface.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtPropertyInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtPropertyInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;
      QMap<const char*, QMetaObject> propertyItems() override;
  };

Map the custom property class name to the metadata of its property-item class:

.. code-block:: cpp

  QMap<const char*, QMetaObject> MyModule::propertyItems()
  {
      return {{
          GT_CLASSNAME(MyProperty),
          GT_METADATA(MyPropertyItem)
      }};
  }

The property item must derive from the appropriate GTlab property-item base and
have a ``Q_INVOKABLE`` default constructor. GTlab validates and instantiates it
through Qt metadata.

Keep validation and serialization rules in the property type. The property
item should translate between that API and the editor widget, including units,
read-only state, and failed validation. Parent temporary widgets and properties
according to Qt ownership rules; the factory creates UI instances as needed.
