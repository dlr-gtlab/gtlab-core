Scripting of GTlab
==================

Scripting in GTlab makes it possible to automate tasks and extend the functionality of the application.  
It provides access to the Python Console, supports the creation of dynamic workflows,  
and allows integration of Python code into graph-based workflows.

Typical use cases include inspecting project data, writing custom calculators,  
implementing loops or conditional tasks, and embedding Python logic into graph nodes.

.. _label_python_console:
.. include:: types/python_console.rst

Scripting of workflows
----------------------

GTlab offers several ways to integrate Python code directly into workflows.  
This makes it possible to combine standard workflow elements with custom 
scripts, add dynamic logic, or extend calculations.  
The following scripting options are available:

- :ref:`Calculators <label_scripting_calculators>` – 
  embed custom Python code as a calculator within a workflow.

- :ref:`Tasks <label_scripting_tasks>` – 
  define entire workflows dynamically using Python code.

- :ref:`Nodes <label_scripting_nodes>` – 
  integrate Python logic into graph-based workflows.

Together, these elements allow workflows to be customized, automated, and 
extended beyond predefined functionality.

.. _label_scripting_calculators:
.. include:: types/calculators.rst
.. _label_scripting_tasks:    
.. include:: types/tasks.rst
.. _label_scripting_nodes:
.. include:: types/nodes.rst





