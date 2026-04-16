Calculators
^^^^^^^^^^^

A calculator, in general, is a workflow element that performs a specific, 
predefined operation, such as a mathematical computation or a transformation of 
data. The Python Script Calculator extends this concept by providing a flexible
calculator whose behavior is not fixed. Instead, it executes a user-defined
Python script.

Within the script, all child objects of the current project can be accessed 
directly by their names, allowing their properties to be read, modified, and 
used in custom calculations. This capability makes the Python Script Calculator 
ideal for prototyping new calculation methods, experimenting with alternative 
approaches, or integrating custom logic directly at runtime.

The Python Script Calculator provides a configuration dialog with an integrated
script editor, where the Python script executed by the calculator can be 
created and edited.

.. image:: /scripting/images/python-calculator-example-light.png
   :alt: Python Script Calculator configuration dialog
   :align: center
   :class: only-light

.. image:: /scripting/images/python-calculator-example-dark.png
   :alt: Python Script Calculator configuration dialog
   :align: center
   :class: only-dark


To check the script for errors, it can be evaluated using ``Ctrl+E``. 
Evaluation also registers any newly defined variables, classes, or functions, 
making them immediately available via the editor’s auto-completion.  
Output messages from evaluation are displayed directly below the editor in the 
output console

It is important to note that changes made during evaluation are not applied to
the data model immediately. They only take effect when the Python Script 
Calculator is executed as part of a workflow.

During execution as part of a workflow, output messages appear in the :ref:`Python Console <label_python_console>`.
