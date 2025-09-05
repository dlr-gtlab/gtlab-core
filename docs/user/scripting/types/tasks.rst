Tasks
^^^^^

A task, in general, is the part of a workflow that controls the execution of its child elements.
Typical tasks execute their child elements in a predefined manner, for example sequentially or within loops. 
The Python Task extends this concept by enabling entire workflows to be defined dynamically through Python code. 
It allows tasks and calculators to be executed conditionally, iteratively, or in any user-defined sequence.
By combining these capabilities with Python features such as loops, conditions, and calculations, tasks and calculators can be scripted to form fully dynamic workflows.

Within the script, tasks and calculators can be instantiated and executed as Python objects.
Additionally, all data model packages of the current project are accessible by their package name.
This provides the ability to read and modify the data objects’ properties, as well as using the data when building the workflow.
By combining these capabilities with Python features such as loops, conditions, and calculations, tasks and calculators can be scripted into a dynamic workflow.

The Python Task provides a configuration dialog with an integrated script editor, where the Python script that is executed when the task runs can be created and edited.

.. image:: images/script-task.png
   :align: center
   :alt: Python Tasks access data model packages
   :class: only-light


.. image:: images/script-task-dark.png
   :align: center
   :alt: Python Tasks access data model packages
   :class: only-dark


There are four ways to create calculators...
