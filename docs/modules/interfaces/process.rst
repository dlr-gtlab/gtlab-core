.. _processinterface:

Process Interface
=================

The process interface is needed if the module should offer process elements as tasks or calculators.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_processinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtProcessInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtProcessInterface)
    
        [...]
    
		/**
		* @brief Returns static meta objects of calculator classes.
		* @return list including meta objects
		*/
		QList<GtCalculatorData> calculators() override;

		/**
		* @brief Returns static meta objects of task classes.
		* @return list including meta objects
		*/
		QList<GtTaskData> tasks() override;
    
        [...]
    }
	
Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtProcessInterface::calculators`
    Returns list of registered calculators.

    To use a calculator in GTlab it has to be registered in its module by adding it to the list returned by :cpp:func:`GtProcessInterface::calculators()`.
	The calculator is added as GtCalculatorData or GtExtendedCalculatorData with several meta information as the author, a category or its version.
	
    .. code-block:: cpp

	QList<GtCalculatorData>
	MyModule::calculators()
	{
		QList<GtCalculatorData> metaData;

		//// simple example calculator
		GtCalculatorData exampleCalc = GT_CALC_DATA(GtExampleCalc)
		exampleCalc->id = QStringLiteral("Example Calculator");
		exampleCalc->version = calculatorVersion(1, 0);
		exampleCalc->author = QStringLiteral("John Doe");
		exampleCalc->category = QStringLiteral("Examples");
		metaData << exampleCalc;
		return metaData;
       }

* :cpp:func:`GtProcessInterface::tasks`
    Returns list of registered tasks.

    To use a tasks in GTlab it has to be registered in its module by adding it to the list returned by :cpp:func:`GtProcessInterface::tasks()`.
	The task is added as GtTaskData or GtExtendedTaskData with several meta information as the author, a category or its version.
	
    .. code-block:: cpp

	QList<GtCalculatorData>
	MyModule::tasks()
	{
		QList<GtCalculatorData> metaData;

		//// simple example task
		GtTaskData exampleTask = GT_TASK_DATA(Task)
		exampleTask->id = QStringLiteral("Example Task");
		exampleTask->version = calculatorVersion(1, 0);
		exampleTask->author = QStringLiteral("John Doe");
		exampleTask->category = QStringLiteral("Examples");
		metaData << exampleTask;
		return metaData;
       }