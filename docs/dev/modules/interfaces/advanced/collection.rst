.. _collectioninterface:

Collection Interface
====================

The process interface is needed if the module should offer a collection structure.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_collectioninterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtCollectionInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtCollectionInterface)
    
        [...]
    
		/**
		* @brief Returns collection specific icon.
		* @return Collection spezific icon.
		*/
		QIcon collectionIcon() const override;

		/**
		* @brief Returns identification string of collection. Same id
		* is used to generate access point data within the netowrk interface.
		* @return Identification string of collection.
		*/
		QString collectionId() const override;

		/**
		* @brief Returns meta object of GtAbstractCollectionSettings class.
		* @return Meta object of GtAbstractCollectionSettings class.
		*/
		QMetaObject collectionSettings() const override;

		/**
		* @brief Returns structure of collection items.
		* @return Sturcture of collection items.
		*/
		QMap<QString, QMetaType::Type> collectionStructure() const override;
    
        [...]
    }
	
Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtCollectionInterface::collectionIcon`
    Defines the icon of the collection widget
	
    .. code-block:: cpp

		QIcon
		MyModule::collectionIcon() const
		{
			return gt::gui::icon::server();
		}


* :cpp:func:`GtCollectionInterface::collectionId`
    Defines the id of the collection. It is recommended to use the module :cpp:func:`GtModuleInterface::ident()`.

	.. code-block:: cpp

		QString
		MyModule::collectionId() const
		{
			return ident();
		}


* :cpp:func:`GtCollectionInterface::collectionSettings`
    Defines the icon of the collection widget
	
    .. code-block:: cpp

		QIcon
		MyModule::collectionSettings() const
		{
			return GtExampleCollectionSettings::staticMetaObject;
		}	


* :cpp:func:`GtCollectionInterface::collectionStructure`
    Defines the structure of the collection based on categories to sort the entries. 
	
    .. code-block:: cpp

		QIcon
		MyModule::collectionStructure() const
		{
			QMap<QString, QMetaType::Type> retval;

			retval.insert(QStringLiteral("source"), QMetaType::QString);
			retval.insert(QStringLiteral("type"), QMetaType::QString);
			retval.insert(QStringLiteral("component"), QMetaType::QString);

			return retval;
		}

