.. _calcexecinterface:

CalculatorExec Interface
========================

The CalculatorExec interface enables the module to define separate options to execute calculators not in the basic execution system.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_calculatorexecinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtCalculatorExecInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtCalculatorExecInterface)
    
        [...]
    
		/**
		* @brief Returns executor identification string .
		* @return Executor identification string.
		*/
		virtual QString executorId() = 0;

		/**
		* @brief Returns executor description.
		* @return Executor description.
		*/
		virtual QString executorDescription() = 0;

		/**
		* @brief Returns meta object of executor.
		* @return Meta object of executor.
		*/
		virtual QMetaObject executor() = 0;

		/**
		* @brief Returns list of executor settings.
		* @return Executor settings.
		*/
		virtual QList<GtAbstractProperty*> executorSettings() {
			return {};
		};
    
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