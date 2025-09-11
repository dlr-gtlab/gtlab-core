The Python Console
------------------

GTlab provides an integrated Python Console that allows controlling GTlab and 
directly managing project data using Python code.  The main features of the 
Python Console include:

- Switching between sessions
- Opening projects
- Managing project data and hub-spoke workflows
- Starting hub-spoke workflows

In addition, the Python Console enables direct interaction with the selected
Python environment, making it possible, for example, to import Python modules 
or extend the ``sys.path`` list.

The Python Console is located in the :ref:`Output Dock <label_output_dock>` 
(``Window → Dock Widgets → Output``) under the *Python Console* tab.


Entering and Executing Code
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Python Console displays the prompt ``GTlab>``, which is ready to accept 
Python code input. Pressing ``Enter`` executes the code. ``print()`` outputs,
return values, and error messages appear directly below the input.
Using the shortcut ``Shift+Enter`` also allows entering multi-line Python code,
for example to define functions.

The auto-completion feature assists with writing Python code and includes all 
objects, functions, classes, and modules available in 
the :ref:`console scope <label_python_scope>`.

.. |clear| image:: ../static/icons/clear.svg
   :alt: clear
   :height: 16px
   :width: 16px

Console content can be cleared by clicking the |clear| button in the 
lower-right corner.

.. image:: images/python_console.gif
   :alt: Demo of the Python Console
   :width: 400px


Controlling GTlab
^^^^^^^^^^^^^^^^^

The Python Console allows direct execution of core GTlab functions that control 
the application. Typical use cases include switching between sessions, opening 
projects, and accessing the currently opened project.

.. code-block:: python

   switchSession('My Session')
   openProject('My Project')
   project = currentProject()

These functions provide the entry point for interacting with GTlab from Python.  
A complete list of available functions, including their parameters and return 
types, is documented in :ref:`label_controlling_gtlab`.


Managing Project Data
^^^^^^^^^^^^^^^^^^^^^

A GTlab project organizes all project data in a hierarchical data tree, 
which is visualized in the :ref:`Project Explorer <label_project_explorer>`. 
The Python Console allows exploring this data tree, accessing individual objects,
and modifying their properties using Python code.

To do so, the :py:func:`currentProject` function must be called, which returns 
the currently opened project as a Python object. This object corresponds to the 
project entry shown in the :ref:`Project Explorer <label_project_explorer>`.

All child data objects of the project can be accessed through the project object.
Navigation through the data tree is done via the names of the child objects,
for example:

.. code-block:: python

   proj = currentProject()
   proj.<child_object_name>.<subchild_object_name>

In this way, individual data objects can be referenced and their properties can
be read or modified. Note that :py:class:`GtObject` is the base type of every
data object and provides read and write access to the object's properties.

.. image:: images/python_console_data_management.gif
   :alt: Demo of managing project data via Python Console


Managing and Starting hub-spoke Workflows
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A GTlab project includes not only the project data but also the hub-spoke
workflows defined within the project. They are visualized in the 
:ref:`Process/Calculators Dock <label_section_processdock>` of the GTlab user
interface. These workflows can be accessed and modified via the Python Console. 
In addition, it is possible to start workflows directly from the Python Console.

To access the project-specific workflows, the project object returned by 
:py:func:`currentProject` provides the child object *Process Data*. This subtree
allows navigation through the project's workflows, which consist of tasks and
calculators, enabling their properties to be read and modified.

A specific workflow can be accessed using the following object navigation:

.. code-block:: python

   proj = currentProject()
   my_task = proj.findGtChild('Process Data').<scope>.<workflow_group>.findGtChild('My Workflow')

Here, ``<scope>`` refers to the sub-area under *Process Data* where the workflows
are organized (e.g., ``_user`` or ``_custom``). ``<workflow_group>`` corresponds
to the name of the workflow group, and ``My Workflow`` is the  name of the 
desired workflow. Since each workflow consists of a root task, the workflow 
itself is represented by a task that manages the subordinate tasks and calculators.
Like all data objects in the project's data tree, tasks and calculators are based on the
type :py:class:`GtObject`, which provides read and write access to their properties.

To start workflows directly from the Python Console, the project object provides
the method :py:meth:`GtProject.runProcess`. It expects the name of a task that
serves as a starting point of a workflow. In GTlab, each direct child of a 
``<workflow_group>`` represents such a workflow starting point. The names of 
these workflows can be found in the :ref:`Process/Calculators Dock <label_section_processdock>`, 
where each workflow appears as a top-level item in the tree view.

.. code-block:: python

   proj = currentProject()
   proj.runProcess('My Workflow')

.. note::

   Only workflows located in the workflow group that is currently selected
   and displayed in the Process/Calculators Dock can be started via the 
   Python Console. In a future release, this restriction will be removed, 
   to allow starting workflows from any workflow group.
