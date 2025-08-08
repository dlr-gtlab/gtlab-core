Process elements
================

The gtlab process management is based on two types of process elements: tasks and calculators

Calculators
-----------

Calculators are the basic process elements for the execution of tools, data model modifications ...

The main function is :cpp:func:`bool run()`. This is executed in the process evaluation and returns a bool value to indicate the success of the run.
A definition of a calculator always needs the Q_OBJECT in the definition and constructor using the Q_INVOKABLE macro.

A defined caclculator needs to be registered in :cpp:func:`GtProcessInterface::calculators()`. There also meta infomration about the caclulator like version number, author, ... can be defined.

An example for a calculator implementation in the test module about the process interface.


.. code-block:: cpp
    
	#include "gt_calculator.h"
	
	class TestCalculator : public GtCalculator
	{
		Q_OBJECT
	public:
		Q_INVOKABLE TestCalculator();
	
		bool run() override;
	
		[. . .]
	
	};

Tasks
-----

Tasks are higher-level process elements in GTlab. A process chain always has a task as its top-level object. 
Tasks have the function of organizing the process chain. In the simplest case, all subordinate calculators are simply executed. 
However, a task can also define a loop or other iteration or control very complex coordination of calculators, such as an optimization.

The base class GtTask offers virtual functions which might be overloaded to define specific behaviour for a new task.

.. code-block:: cpp

	class GT_CORE_EXPORT GtTask : public GtProcessComponent
	{
		Q_OBJECT
	
	public:
		[. . .]
	
		/**
		* @brief GtTask
		*/
		Q_INVOKABLE GtTask();
		~GtTask() override;
	
		[. . .]
	
		/**
		* @brief Virtual method to implement an initial setup for the iteration
		* process.
		* @return Returns true if setup was successful, otherwise returns false
		*/
		virtual bool setUp();
	
		/**
		* @brief Virtual method to implement an evaluation method for each
		* iteration step.
		* @return Evaluation state for current iteration step
		*/
		virtual GtTask::EVALUATION evaluate();
	
		/**
		* @brief Virtual method to implement an iteration procedure.
		* Default implementation iterates up to the defined maximum iteration
		* number.
		*/
		virtual bool runIteration();
	
		[. . .]
		
		/**
		* @brief Starts the execution process of all child process elements.
		* The current iteration number is automatically increased and all
		* monitoring properties transferred. After the successfull run of the
		* elements the evaluation method is called.
		*/
		virtual bool runChildElements();
		
		[. . .]
	};

Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtTask::setup`

Text about setup

.. code-block:: cpp
    
    [...]
	/// example from a loop 
    gtDebug() << "initial run with value:" << value(m_currentIter);
    m_currentVal.setVal(value(m_currentIter));
    m_maxIter.setVal(m_steps + 1);
    return true;
    [...]
	
* :cpp:func:`GtTask::evaluate`

Text about evaluate

.. code-block:: cpp
    
    [...]
	/// example from a loop 
    gtDebug() << "evaluate for step:" << m_currentIter;

    if (m_currentIter < m_maxIter)
    {
        gtDebug() << "current Value:" << value(m_currentIter);
        m_currentVal.setVal(value(m_currentIter));
    }
    else
    {
        return GtTask::EVAL_FINISHED;
    }

    return GtTask::EVAL_OK;
    [...]	
	
* :cpp:func:`GtTask::runIteration`

Text about runIteration

.. code-block:: cpp
    
    [...]
	/// add a good example here
    [...]
		
* :cpp:func:`GtTask::runChildElements`

Text about runChildElements

.. code-block:: cpp
    
    [...]
	/// example from a loop 
    QList<GtProcessComponent*> childs = processComponents();

    // increment current iteration step and continue iteration
    m_currentIter.setVal(m_currentIter.getVal() + 1);

    // trigger transfer of monitoring properties before running calculators
    emit transferMonitoringProperties();

    // reset state of child prcess elements
    foreach (GtProcessComponent* comp, childs)
    {
        comp->setStateRecursively(GtProcessComponent::QUEUED);
    }

    // run calculators
    foreach (GtProcessComponent* comp, childs)
    {
        GtCalculator* calc = qobject_cast<GtCalculator*>(comp);

        if (!comp->exec())
        {
            if (calc)
            {
            // calculator run failed
            //setState(GtProcessComponent::FAILED);
            //
            //return false;
            gtWarning() << "Current Step failed";
            break;
            }
        }

        if (isInterruptionRequested())
        {
            gtWarning() << "task terminated!";
            setState(GtProcessComponent::TERMINATED);
            return false;
        }

        //GtCalculator* calc = qobject_cast<GtCalculator*>(comp);

        if (calc && calc->runFailsOnWarning())
        {
            if (calc->currentState() == GtProcessComponent::WARN_FINISHED)
            {
                calc->setState(FAILED);
                setState(GtProcessComponent::FAILED);
                return false;
            }
        }
    }

    // evaluate current iteration step
    m_lastEval = evaluate();

    // trigger transfer of monitoring properties after evaluation
    emit transferMonitoringProperties();

    // collect monitoring data for entire task
    GtMonitoringDataSet monData = collectMonitoringData();

    // check whether monitoring data has entries
    if (!monData.isEmpty())
    {
        // monitoring data available - emit signal
        emit monitoringDataTransfer(m_currentIter, monData);
    }

    return true;
    [...]	