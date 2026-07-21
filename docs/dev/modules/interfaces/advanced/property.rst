.. _propertyinterface:

Property Interface
==================

The property interface is needed if the module should offer its own property types with their own GUI items. 
This this interface newly defined properties can be extendended to habe their own graphical representation and editors.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_propertyinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtPropertyInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtPropertyInterface)
    
        [...]
    
		/**
		* @brief Returns static meta objects of property item classes.
		* @return list of meta objects of property item classes.
		*/
		QMap<const char*, QMetaObject> propertyItems() override;
    
        [...]
    }
	
Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtPropertyInterface::propertyItems`
    Returns list of registered property items.

    .. code-block:: cpp

	QMap<const char*, QMetaObject>
	MyModule::propertyItems()
	{
		QMap<const char*, QMetaObject> retval;

		retval.insert(GT_CLASSNAME(GtTestProperty),
                 GT_METADATA(GtTestPropertyItem));

		return retval;
       }
