:tocdepth: 3

.. _label_function_reference:

Function Reference
==================

.. If you like to  cross-ref a function, do it like this :py:func:`footprint` :

.. _label_python_scope:

Scopes
------

GTlab integrates a Python interpreter that can be accessed in different ways, 
such as through the :ref:`Python Console <label_python_console>`, :ref:`Python Tasks <label_scripting_tasks>`, 
:ref:`Python Script Calculators <label_scripting_calculators>`, or :ref:`Nodes <label_scripting_nodes>`.
Depending on how Python is accessed, different predefined functions and classes are available.

GTlab organizes this concept by introducing scopes.
A scope corresponds to a separate execution environment with its own predefined functions, classes, and variables.
Variables or functions defined in one scope (e.g., the Console) are not accessible in another scope (e.g., a Task).
Each instance of a Python Task, Script Calculator, or Node has its own scope, meaning that variables or functions defined in one 
instance are isolated and cannot be accessed from another instance.

GTlab defines the following scopes:

.. _label-console-scope:

``console``
^^^^^^^^^^^

Provides functions for interacting with and controlling the application itself
(e.g., switching sessions, opening projects).

.. _label-task-scope:

``task``
^^^^^^^^

Provides functions for building and controlling workflows dynamically inside
Python Tasks.

.. _label-calculator-scope:

``calculator``
^^^^^^^^^^^^^^

Provides functions for performing calculations within Python Script Calculators,
including access to project data models.

.. _label-node-scope:

``node``
^^^^^^^^

Provides functions that are available when scripting inside GTlab nodes.

All documented Python functions indicate the scopes in which they are available.

.. |ref_console_scope| replace:: ``console``
.. |ref_calculator_scope| replace:: ``calculator``
.. |ref_task_scope| replace:: ``task``
.. |ref_node_scope| replace:: ``node``


Logging
---------

To interact with GTlab’s integrated logging system via Python, the ``GtLogging`` module is provided.
It allows logging messages directly from Python scripts and displays them in the Application Console as well as in the Python Console.

Compared to using plain print() statements, GtLogging messages are automatically categorized by log level and integrated into GTlab’s logging infrastructure.
For more information on the different log levels, see the :ref:`logging <label_section_logging>` section.

The logging functions of ``GtLogging`` are import into each :ref:`scope <label_python_scope>` by default.

The following logging functions are provided:

.. py:function:: gtDebug(msg: str)

   Logs a debug-level message.

   :param msg: The message to log.
   :type msg: str
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|

.. py:function:: gtInfo(msg: str)

   Logs an info-level message.

   :param msg: The message to log.
   :type msg: str
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|

.. py:function:: gtWarning(msg: str)

   Logs a warning-level message.

   :param msg: The message to log.
   :type msg: str
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|

.. py:function:: gtError(msg: str)

   Logs an error-level message.

   :param msg: The message to log.
   :type msg: str
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|

.. py:function:: gtFatal(msg: str)

   Logs a fatal-level message.

   :param msg: The message to log.
   :type msg: str
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|


Introspecting GTlab
-------------------

.. py:function:: projectPath() -> str

   Returns the filesystem path of the currently opened project.

   :returns: The path of the current project.
   :rtype: str
   :raises: Warning if no project is currently open.
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|


.. py:function:: envVars() -> dict

   Returns a dictionary of GTlab environment variables.

   :returns: A dictionary mapping environment variable names to their values.
   :rtype: dict
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|


.. py:function:: footprint(only_active: bool = True) -> dict

   Returns a dictionary of module names and their versions.
   By default, it returns the application footprint, which includes all
   available modules in GTlab. If `only_active` is True, only the modules
   that are part of the data model of the current project are included.

   :param only_active: If True, only the currently active modules in the project are included.
                       Otherwise, all available modules in GTlab are returned.
   :type only_active: bool
   :returns: A dictionary mapping module names to their version strings.
   :rtype: dict
   :scope: |ref_console_scope|, 
           |ref_calculator_scope|, 
           |ref_task_scope|, 
           |ref_node_scope|


.. _label_controlling_gtlab:

Controlling GTlab
-----------------

.. py:function:: switchSession(name: str)

   Switches to the session with the specified ``name``.

   :param name: The name of the session to switch to.
   :type name: str
   :scope: |ref_console_scope|


.. py:function:: openProject(projectIdOrPath: str) -> GtProject

   Opens a project either by its filesystem path or by its project ID. 
   Note that opening by project ID only works if the project is already part of the current session.

   :param projectIdOrPath: The filesystem path or the project ID of the project to be opened.
   :type projectIdOrPath: str
   :returns: The opened project, or None if the project could not be opened.
   :rtype: GtProject
   :scope: |ref_console_scope|

.. py:function:: currentProject() -> GtProject

   Returns the currently opened project.

   :returns: The currently opened project, or None if no project is open.
   :rtype: GtProject
   :scope: |ref_console_scope|


Managing Data
-------------

.. py:class:: GtObject

   .. py:method:: className() -> str

      Returns the class name (type name) of the object.

      :returns: The class name of the object.
      :rtype: str

   .. py:method:: calcHash() -> str

      Returns the hash value of the current object.

      The hash value is a fingerprint of the object state,
      including all property values and child objects.
      Thus, if an object changes, the hash will change too.

      :returns: The hash value of the object.
      :rtype: str

   .. py:method:: clone() -> GtObject

      Creates a clone of the object. Note, that the new object
      is a free object and thus does not belong to any parent.

      :returns: A cloned instance of the object.
      :rtype: GtObject

   .. py:method:: findGtChild(childName: str) -> GtObject

      Returns the first child element matching the object name of childName.
      If no element was found, it returns None. 

      :param childName: The object name of the the child to search for.
      :type childrenName: str
      :returns: The first matching child object, or None if not found.
      :rtype: GtObject or None

   .. py:method:: findGtChildren(childrenName: str = "", objectClassName: str = "" ) -> list[GtObject]

      Returns the direct children matching ``childrenName`` and ``objectClassName``.
      An empty string matches everything.

      :param childrenName: The object name of the the children to search for.
      :type childrenName: str
      :param objectClassName: If not empty, only children matching the class name are returned.
      :type objectClassName: str
      :returns: A list of matching child objects.
      :rtype: list[GtObject]

   .. py:method:: findGtChildrenByClass(objectClassName: str = "") -> list[GtObject]

      Returns the direct children matching their class name defined by ``objectClassName``.
      An empty string matches everything.

      :param objectClassName: If not empty, only children matching the class name are returned.
      :type objectClassName: str
      :returns: A list of matching child objects.
      :rtype: list[GtObject]

   .. py:method:: findGtParent() -> GtObject

      Returns the parent object of the object if it exists. 
      Otherwise, ``None`` is returned.
      :returns: The parent object.
      :rtype: GtObject or None

   .. py:method:: findGtProperty(id: str) -> GtAbstractProperty

      Returns the property matching id. Returns ``None``, if not found.

      :note: The actual id might differ from the displayed property name!

      :param id: The property id to search for.
      :type id: str
      :returns: The matching property, or ``None`` if not found.
      :rtype: GtAbstractProperty or None

   .. py:method:: findGtProperties() -> list[GtAbstractProperty]

      Returns all properties of the object.
      
      :returns: A list of all properties of the object.
      :rtype: list[GtAbstractProperty]

   .. py:method:: propertyValue(id: str)

      Returns the value of the property with given ``id``.

      :raises: ``RuntimeError``, if the property does not exist.

      :param id: The property id to search for.
      :type id: str
      :returns: The value of the property. The return type depends on the property.

   .. py:method:: setPropertyValue(id: str, value)

      Sets the value of the property with given ``id``.

      :raises: ``RuntimeError``, if the property does not exist.

      :param id: The property id to search for.
      :type id: str
      :param value: The value to set. The value type depends on the property.

   .. py:method:: getPropertyContainerSize(id: str) -> int

      Returns the size of the property container given by `id`.

      :returns: The size of the container, -1 if *id* is invalid.
      :rtype: int

   .. py:method:: getPropertyContainerVal(containerId: str, index: int, memberId: str)

        Returns the member of the index-th entry in the container, i.e.
        something like ``container[index].memberId`` .

        :example: Accessing the *value* member in the 2. entry of the container *input_args*:
            ``getPropertyContainerVal('input_args', 2, 'value')``

        :param containerId: The identifier of the container.
        :param index: The index of the entry in the container.
        :param memberId: The id of the member to returns.
        :returns: The value in the container entry. Returns None, if not found.

   .. py:method:: setPropertyContainerVal(containerId: str, index: int, memberId: str, value) -> bool

      Sets the member of the index-th entry in the container, i.e.
      something like ``container[index].memberId = value`` .

      :example: Set the *name* member in the 2. entry of the container *input_args* to "myname":
         ``setPropertyContainerVal('input_args', 2, 'name', 'myname')``

      :param containerId: The identifier of the container.
      :param index: The index of the entry in the container.
      :param memberId: The id of the member to set.
      :param value: The value to set.
      :returns: True on success, otherwise False.
      :rtype: bool

   .. py:method:: uuid(): str

      Returns the UUID of the object.
      
      :returns: The UUID of the object.
      :rtype: str

   .. py:method:: objectByUUID(uuid: str) -> GtObject

      Searches a child object, whose UUID is ``uuid``.
        
      :returns: The object matching the UUID. ``None``, if not found.
      :rtype: GtObject or None


.. py:class:: GtAbstractProperty

   Base class for all properties of GTlab objects.
   Properties are attributes of objects havaing a value.
   Properties can be hierarchical, i.e. they can be nested and thus contain
   child properties.

   .. py:method:: findGtProperties() -> list[GtAbstractProperty]

      Returns all child / sub properties of the property.

      :returns: A list of all child / sub properties.
      :rtype: list[GtAbstractProperty]

   .. py:method:: findGtProperty(id: str) -> GtAbstractProperty

      Returns the child property matching id. Returns ``None``, if not found.

      :note: The actual id might differ from the displayed property name!

      :param id: The property id to search for.
      :type id: str
      :returns: The matching child property, or ``None`` if not found.
      :rtype: GtAbstractProperty or None

   .. py:method:: propertyValue(id: str)

      Returns the value of the sub-property with given ``id``.

      :raises: ``RuntimeError``, if the property does not exist.

      :param id: The property id to search for.
      :type id: str
      :returns: The value of the property. The return type depends on the property.

   .. py:method:: setPropertyValue(id: str, value)

      Sets the value of the sub-property with given ``id``.

      :raises: ``RuntimeError``, if the property does not exist.

      :param id: The property id to search for.
      :type id: str
      :param value: The value to set.

   .. py:method:: isActive() -> bool

      Returns, whether the property is active.

      :returns: True if the property is active, False otherwise.
      :rtype: bool

   .. py:method:: setActive(val: bool)

      Sets the property active / inactive.

      :param val: True to set the property active, False to set it inactive.
      :type val: bool

   .. py:method:: isOptional() -> bool

      Returns, whether the property is optional.

      :returns: True if the property is optional, False otherwise.
      :rtype: bool

   .. py:method:: setOptional(val: bool)

      Sets the property optional / required.

      :param val: True to set the property optional, False to set it required.
      :type val: bool



.. py:class:: GtProject(GtObject)

   Inherited from :py:class:`GtObject`.

   This class represents a project. 

   .. py:method:: path() -> str

      Returns the path to the project directory on the hard drive.

      :returns: The filesystem path of the project directory.
      :rtype: str

   .. py:method:: isOpen() -> bool

      Returns whether the project is currently open.

      :returns: True if the project is open, False otherwise.
      :rtype: bool

   .. py:method:: isValid() -> bool

      Returns whether the project data is successfully loaded.

      :returns: True if the project data is valid, False otherwise.
      :rtype: bool

   .. py:method:: runProcess(processId: str, save: bool = False) -> bool

      Starts the process with the given ID.

      :param processId: The ID of the process to start.
      :type processId: str
      :param save: Whether the results of the process should be saved. Default is False.
      :type save: bool
      :returns: True if the process executed successfully, False otherwise.
      :rtype: bool

   .. py:method:: findProcess(processId: str) -> GtTask

      Returns the process with the specified ID.

      :param processId: The ID of the process to retrieve.
      :type processId: str
      :returns: The process object with the given ID, or None if not found.
      :rtype: GtProcess (or the appropriate Python-wrapped object)

   .. py:method:: close(save: bool = False) -> bool

      Closes the project.

      :param save: Whether to save results before closing. Default is False.
      :type save: bool
      :returns: True if the project was closed successfully, False otherwise.
      :rtype: bool


Building and Controlling Workflows
----------------------------------

.. py:function:: findGtTask(name: str) -> GtTask

   Returns a cloned instance of a workflow with the given ``name`` from the project's hub-spoke workflows.

   This function can only find workflows that are part of the same workflow
   group as the Python Task in which it is called.
   If multiple workflows share the same name, the first match is returned.

   The returned workflow object is a clone and exists only within the Python task's :ref:`scope <label_python_scope>`.

   .. important::

      To safely access its subordinate tasks or calculators, a reference to the cloned workflow must be kept;
      otherwise, child objects may become invalid if the workflow object is deleted.

   :raises SystemError: If no workflow with the given name exists.

   :param name: The object name of the workflow to search for.
   :type name: str
   :returns: A cloned instance of the matching workflow.
   :rtype: GtTask
   :scope: |ref_task_scope|


.. py:class:: GtTask(GtObject)

   An executable task of the hub-spoke workflow engine.

   Inherited from :py:class:`GtObject`

   .. py:method:: run() -> bool

      Executes the task.

      :return: ``True`` on success, ``False`` otherwise.
      :rtype: bool

      .. note::
   
         When called while evaluating a script in the
         :ref:`Python Task’s configuration dialog <label_scripting_tasks>`,
         ``run()`` does not execute the task logic. In this case it always 
         returns ``True`` to allow syntax and basic semantic checks without
         running the actual workflow.

   .. py:method:: deleteAllCalculators()

      Deletes all calculator appended to the given task.

   .. py:method:: hasWarnings() -> bool

      Returns state of warning flag.

      :returns: True, if the task has warnings. False otherwise.
      :rtype: bool


.. py:class:: GtPyTask(GtTask)

   This class represents a python task.
   As such, additional settings like input and output 
   args can be defined via methods.

   Inherited from :py:class:`GtTask`

   .. py:method:: inputArgs() -> dict

      Returns all input arguments of the python task as a dict.
      :returns: A dictionary mapping argument names to their values.
      :rtype: dict

   .. py:method:: inputArg(argName: str)

      Returns the value of the input argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist
   
      :param argName: The name of the input argument.
      :type argName: str
      :returns: The value of the input argument.

   .. py:method:: setInputArg(argName: str, value)

      Sets the value of the input argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist
   
      :param argName: The name of the input argument.
      :type argName: str
      :param value: The value to set.

   .. py:method:: outputArgs() -> dict

      Returns all output arguments of the python task as a dict.

      :returns: A dictionary mapping argument names to their values.
      :rtype: dict

   .. py:method:: outputArg(argName: str)

      Returns the value of the output argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist
      
      :param argName: The name of the output argument.
      :type argName: str
      :returns: The value of the output argument.


.. py:class:: GtCalculator(GtObject)

   An executable calculator of the hub-spoke workflow engine.

   Inherited from :py:class:`GtObject`

   .. py:method:: run() -> bool

      Executes the calculator.

      :return: ``True`` on success, ``False`` otherwise.
      :rtype: bool

      .. note::
         When called while evaluating a script in the
         :ref:`Python Task’s configuration dialog <label_scripting_tasks>`, 
         ``run()`` does not perform the actual calculation. In this case it 
         always returns ``True`` to enable syntax and basic semantic checks
         without executing the calculator routine.
