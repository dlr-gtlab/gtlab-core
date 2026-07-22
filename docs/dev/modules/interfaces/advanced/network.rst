.. _networkinterface:

Network access interface
========================

Use :cpp:class:`GtNetworkInterface` when a module needs a named set of network
credentials and a connection test integrated with GTlab's access manager. This
interface defines access configuration; it is not a general HTTP or socket
abstraction.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtNetworkInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtNetworkInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QString accessId() override;
      QMetaObject accessConnection() override;
  };

``accessId()`` identifies the credential group. It must be unique among loaded
modules and stable after release. Using the module ID is appropriate when the
module needs exactly one access group:

.. code-block:: cpp

  QString MyModule::accessId()
  {
      return ident();
  }

``accessConnection()`` returns metadata for a concrete
``GtAbstractAccessDataConnection`` class. GTlab creates it through Qt metadata,
so the class needs an invokable default constructor. Override its connection
test and, where required, default host, port, or port-request behavior.

.. code-block:: cpp

  QMetaObject MyModule::accessConnection()
  {
      return GT_METADATA(MyAccessConnection);
  }

Do not log passwords, tokens, or full credential objects. Connection failures
should distinguish invalid configuration from unavailable infrastructure so
users know what they can correct.
