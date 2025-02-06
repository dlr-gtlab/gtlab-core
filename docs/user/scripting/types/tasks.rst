Tasks
^^^^^

A task, in general, is the part of a workflow that controls the execution of
its child elements. Typical tasks execute their child elements in a predefined
manner, for example sequentially or within loops. The Python Task extends this
concept by enabling entire workflows to be defined dynamically through Python
code. It allows tasks and calculators to be executed conditionally, 
iteratively, or in any user-defined sequence.

Within the script, tasks and calculators can be instantiated and executed as 
Python objects. Additionally, all data model packages of the current project
are accessible by their package name. This provides the ability to read and
modify the data objects’ properties, as well as using the data when building 
the workflow. By combining these capabilities with Python features such as
loops, conditions, and calculations, tasks and calculators can be scripted to
form fully dynamic workflows.

The Python Task provides a configuration dialog with an integrated
script editor, where the Python script executed by the Python Task can be
created and edited.

.. image:: images/python-task-light.png
   :align: center
   :alt: Python Tasks configuration dialog
   :class: only-light


.. image:: images/python-task-dark.png
   :align: center
   :alt: Python Tasks configuration dialog
   :class: only-dark

The editor offers the same features as in the 
:ref:`Python Script Calculator <label_scripting_calculators>`, including 
auto-completion and script evaluation. In addition, the Python Task editor
supports creating, configuring, and executing calculators, and allows
loading existing hub-spoke workflows from the project data into the script.


Configuring Calculators
"""""""""""""""""""""""

Within a Python Task dialog, calculators can be configured in three different
ways:


**Using the user interface**

Calculators can be configured directly through their user interface. Any 
settings applied in this way are automatically translated into Python code
and inserted into the script.

To configure a specific calculator, click the *Add…* button in the Python 
Task dialog. After selecting the desired calculator, its configuration dialog
opens for setting it up. Once the configuration is completed by clicking
*Finish*, the corresponding Python code is automatically inserted into the
script. All calculator properties that differ from their default values are
set accordingly. The calculator created in this way is then available
as a Python object within the script.

.. image:: images/python-task-create-calc-ui-light.gif
   :alt: Demo of configuring a calculator using the user interface
   :width: 100%
   :class: only-light


.. image:: images/python-task-create-calc-ui-dark.gif
   :alt: Demo of configuring a calculator using the user interface
   :width: 100%
   :class: only-dark

**Using drag and drop**

Calculators that are part of a workflow and listed in the
:ref:`Processes/Calculators Dock <label_section_processdock>` can be inserted
directly into the script using drag & drop. The corresponding Python code is
automatically generated, reflecting the calculator’s current configuration.

To insert a calculator this way, simply drag it from the Processes/Calculators
Dock into the editor.

.. note::

   Drag & drop only copies the configuration of the selected calculator into
   the Python script. The original calculator in the workflow remains unchanged.


**In the script**

Calculators can be created directly within the script by instantiating their
corresponding class. The script editor’s auto-completion assists in finding
and inserting the instantiation call for a calculator. It appears in the
auto-completion list under either the calculator’s class name or the name
under which the calculator appears in GTlab.

When a calculator is instantiated in this way, all of its properties are
initially set to their default values. The properties can then be adjusted
directly in the script, making the calculator fully configurable. 
Auto-completion helps with this by listing all available properties of the
calculator object, making it easy to read and set their values.

All calculators instantiated in the script are based on the 
:py:class:`GtCalculator` type, which provides methods for reading and 
writing properties. Properties can also be accessed directly using Python
attribute access, though this only works for properties whose internal names
do not contain spaces or special characters.

Once a calculator has been configured, it can be executed directly from the
script using its :py:meth:`GtCalculator.run` method.

.. note:: 
   
   When evaluating the script in the Python Task configuration dialog, 
   calls to :py:meth:`GtTask.run` and :py:meth:`GtCalculator.run` 
   do not trigger execution. They always return ``True`` as a way 
   to validate the script without running the workflow.


Loading Hub-Spoke Workflows into Python Tasks
"""""""""""""""""""""""""""""""""""""""""""""

Within a Python Task, it is possible to load entire hub-spoke workflows that
are already part of the GTlab project into the Python script.
This allows existing project workflows to be controlled and interconnected in
a flexible way, enabling the creation of complex, dynamic workflows using Python code.

To load an existing workflow from the project data into the script, 
the Python Task provides the function :py:func:`findGtTask`.  
It takes the name of the desired workflow and returns a Python object representing that workflow.  
All hub-spoke workflows of the project are listed in the :ref:`Processes/Calculators Dock <label_section_processdock>`, 
making it easy to identify the name of the workflow to be loaded.

The Python object returned by :py:func:`findGtTask` is a clone of the workflow
from the project data.  
It contains all configuration settings as well as all subordinate tasks and 
calculators of the original workflow, which can be accessed through the hierarchical object tree.

This allows the workflow to be modified and executed within the Python Task without affecting the original workflow in the project data.

A cloned workflow can be started within the Python script using the :py:meth:`GtTask.run` method. 
This executes the entire workflow, including all subordinate tasks and calculators, 
as defined in the workflow.  

It is also possible to execute individual subordinate tasks or calculators separately.  
Using dot notation, child objects of the workflow can be directly accessed and configured.  
Tasks can be executed by calling :py:meth:`GtTask.run`, and calculators by calling :py:meth:`GtCalculator.run`.  

.. important::

   In Python, objects are automatically deleted when they are no longer referenced, i.e., when they go "out of scope".  
   This also applies to task objects created with :py:func:`findGtTask`.  
   Since a task object acts as the parent of a hierarchical object tree, 
   all its subordinate tasks and calculators become invalid once the task object is deleted.  

   Therefore, the task object should remain referenced in the script as long as its child objects are accessed or executed.  

   Bad Practice:

   .. code-block:: python

      def get_my_calculator():
          workflow = findGtTask('My Workflow')
          return workflow.MyCalculator

      my_calc = get_my_calculator()
      # my_calc is now invalid because the workflow object was deleted
      my_calc.run()


   To avoid this, the task object should be actively kept referenced while working with its subordinate elements:  

   .. code-block:: python

      workflow = findGtTask('My Workflow')
      my_calc = workflow.MyCalculator
      # the workflow remains referenced, my_calc is valid
      my_calc.run()
