
.. _networkinterface:

Network Interface
=================

The network interface enables the module to define a module related network connection.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_networkinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtNetworkInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtNetworkInterface)
    
        [...]
    
		/**
		* @brief Returns identification string of access point.
		* @return Identification string of access point.
		*/
		QString accessId() override;

		/**
		* @brief Returns meta object of GtAbstractAccessDataConnection class.
		* @return Meta object of GtAbstractAccessDataConnection class.
		*/
		QMetaObject accessConnection() override;
    
        [...]
    }
	
Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtNetworkInterface::accessId`
    Defines the access id of the module. It is recommended to use the module :cpp:func:`GtModuleInterface::ident()`.

    .. code-block:: cpp

	QMap<const char*, QMetaObject>
	MyModule::accessId()
	{
		return ident();
	}
		
* :cpp:func:`GtNetworkInterface::accessConnection`
    Defines the access connection object. For basic applications it is recommended to use GtAccessDataConnection.

    .. code-block:: cpp

	QMap<const char*, QMetaObject>
	MyModule::accessId()
	{
		return GT_METADATA(GtAccessDataConnection);
	}