Tasks
^^^^^

A task, in general, is the part of a workflow that controls the execution of its child elements.
Typical tasks execute their child elements in a predefined manner, for example sequentially or within loops. 
The Python Task extends this concept by enabling entire workflows to be defined dynamically through Python code. 
It allows tasks and calculators to be executed conditionally, iteratively, or in any user-defined sequence.

Within the script, tasks and calculators can be instantiated and executed as Python objects.
Additionally, all data model packages of the current project are accessible by their package name.
This provides the ability to read and modify the data objects’ properties, as well as using the data when building the workflow.
By combining these capabilities with Python features such as loops, conditions, and calculations, tasks and calculators can be scripted to form fully dynamic workflows.

The Python Task provides a configuration dialog with an integrated script editor, where the Python script executed by the Python Task can be created, edited, and tested.

.. image:: images/script-task.png
   :align: center
   :alt: Python Tasks access data model packages
   :class: only-light


.. image:: images/script-task-dark.png
   :align: center
   :alt: Python Tasks access data model packages
   :class: only-dark


Configuring Calculators
"""""""""""""""""""""""

Within a Python Task dialog, calculators can be configured in three different ways:

**Via the User Interface**

Calculators can be configured directly through their user interface. Any 
settings applied in this way are automatically translated into Python code and inserted into the script.

To configure a specific calculator, click the *Add…* button in the Python Task dialog.  
After selecting the desired calculator, its configuration dialog opens for setting it up.  
Once the configuration is completed by clicking *Finish*, the corresponding Python code is automatically 
inserted into the script. All calculator properties that differ from their default values are set accordingly.  
The calculator created in this way is then available as a Python object within the script.


**Via the Script**

Calculators can be created directly within the script by instantiating their corresponding class.
The script editor’s autocompletion assists in finding and inserting the instantiation call for a calculator.
It appears in the autocompletion list under either the calculator’s class name or the name under which the calculator appears in GTlab.

When a calculator is instantiated in this way, all of its properties are initially set to their default values.
The properties can then be adjusted directly in the script, making the calculator fully configurable.
Autocompletion helps with this by listing all available properties of the calculator object, making it easy to 
read and set their values.

All calculators instantiated in the script are based on the :py:class:`GtObject` type,  
which provides methods for reading and writing properties. Properties can also be accessed directly using Python attribute access,  
though this only works for properties whose internal names do not contain spaces or special characters.


**Via Drag & Drop**

Calculators that are part of a workflow and listed in the :ref:`Processes/Calculators Dock <label_section_processdock>` can be inserted directly into the script using drag & drop.
The corresponding Python code is automatically generated, reflecting the calculator’s current configuration.

To insert a calculator this way, simply drag it from the Processes/Calculators Dock into the editor.

.. note::

   Drag & drop only copies the configuration of the selected calculator into the Python script. 
   The original calculator in the workflow remains unchanged.



findGtTask()
""""""""""""

Defining  Workflows
"""""""""""""""""""

