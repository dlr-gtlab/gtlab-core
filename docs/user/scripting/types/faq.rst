FAQ
---

How to import external Python scripts into the integrated environment?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, only modules available in the current Python environment can be imported.  
To make additional scripts accessible, the directory containing them must be added to Python's search path. 
This can be done by importing the ``sys`` module and appending the desired path to ``sys.path``:

.. code-block:: python

   import sys
   sys.path.append("C:/path/to/your/scripts")

   import my_utils

Once the path has been added, scripts located in that directory can be imported and used as regular Python modules.


What is the difference between the the Python Script Calculator and the Python Task?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Python Script Calculator is a single calculator element that executes a user-defined Python script.
It is primarily used for prototyping new calculation methods or testing alternative approaches within a workflow.

The Python Task, on the other hand, is a workflow element that controls the execution of child tasks and calculators.
It allows building flexible and dynamic workflows by defining the workflow logic directly in Python, including loops,
conditions, and calculations, which can dynamically determine the order and repetition of its child elements.


Why can I not see my plot in the node?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To display a plot in the node, there are a few things to consider:
 - The script in the Python node has an interface for matplotlib. Only plots created with it can be displayed. The scripts must end with the show() function of the plot object.
 - The node has a "Plot enabled" property. This must be set to true.
 - The plot can only be displayed if the node has a certain size. If in doubt, you can check this by dragging the node larger (in the bottom right-hand corner of the node).

.. image:: images/PythonNodes_FAQ1.png
  :align: center
  :alt: Plotting with nodes


How are python objects exchanged between the nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Connections of the nodes' ports can be used for data exchange between the nodes. Only ports of the same type can be connected to each other.
The Python node can process all available types on the input side.
Many provided data types have a value() function or similar to make the data of the connection accessible in the Python script. This is also demonstrated in the attached examples.
A few generic data types are enabled on the output side. 
However, the intelli::ByteArrayData data type can be used to pass Python objects directly. 
In this case, the Python functionality "pickl" is used. This method is therefore restricted to data that [is pickl-capable.](https://docs.python.org/3/library/pickle.html#what-can-be-pickled-and-unpickled)


How to access the ports in the script?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The data that is transmitted via the connections or ports can be called up in the script via their "Caption". This is set as a property in the corresponding window. 
This data object itself is not yet the value. This is usually obtained via a value() function or similar functions that can be implemented for the more specific data types.
Output parameters can also be used directly and also have the caption set in the property system as the name in the script.
