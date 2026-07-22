.. _collectioninterface:

Collection interface
====================

Use :cpp:class:`GtCollectionInterface` to integrate a browsable collection of
installable module content with GTlab's collection editor. A collection can
load items from the local installation and, together with network access, show
available remote items. It is not the interface for ordinary project object
groups.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtCollectionInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtCollectionInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QIcon collectionIcon() const override;
      QString collectionId() const override;
      QMetaObject collectionSettings() const override;
      QMap<QString, QMetaType::Type> collectionStructure() const override;
  };

``collectionId()`` is the stable identifier used to register and open the
collection. It also connects collection and access-point data, so it should
match the corresponding network access ID when the collection has a remote
source. ``collectionIcon()`` provides the icon shown in the GUI.

``collectionSettings()`` returns metadata for a concrete
``GtCollectionSettings`` implementation. GTlab creates it through Qt metadata;
the class therefore needs an invokable default constructor. It supplies the
local collection widget, remote browser, and the data displayed by both.

.. code-block:: cpp

  QMetaObject MyModule::collectionSettings() const
  {
      return GT_METADATA(MyCollectionSettings);
  }

``collectionStructure()`` optionally declares additional fields stored for
each item as a map from field name to Qt metatype:

.. code-block:: cpp

  QMap<QString, QMetaType::Type> MyModule::collectionStructure() const
  {
      return {
          {QStringLiteral("component"), QMetaType::QString},
          {QStringLiteral("fidelity"), QMetaType::QString}
      };
  }

Keep field names and types stable once collection metadata has been published.
If remote browsing is required, implement :ref:`networkinterface` with the
same access identity and handle unavailable servers without breaking access to
already installed local items.
