.. _label_chapter_python_console:

The Python Console
------------------

GTlab provides an integrated Python console that allows controlling GTlab and directly managing project data using Python code.  
The main features of the Python console include:

- Switching between sessions
- Opening projects
- Navigating project and process data
- Modifying project data

In addition, the Python console enables direct interaction with the selected Python environment, making it possible, for example, to import Python modules or extend the ``sys.path`` list.

The Python console is located in the Output dock widget (``Window → Dock Widgets → Output``) under the *Python Console* tab.


Usage
^^^^^

The console displays the prompt ``GTlab>``, which is ready to accept Python code input.  
Pressing ``Enter`` executes the code. ``print()`` outputs, return values, and error messages appear directly below the input.  
Using the shortcut ``Shift+Enter`` also allows entering multi-line Python code, for example to define functions.

The auto-completion feature assists with writing Python code and includes all objects, functions, classes, and modules available in the Python environment.

.. |clear| image:: ../static/icons/clear.svg
   :alt: clear
   :height: 16px
   :width: 16px

Console content can be cleared by clicking the |clear| button in the lower-right corner.

.. image:: images/python_console.gif
   :alt: Demo of the Python console
   :width: 400px


Controlling GTlab
^^^^^^^^^^^^^^^^^

The Python console allows direct calling of core GTlab functions.  
This includes switching between sessions, opening projects, and accessing the currently opened project.

.. code-block:: python

    GTlab> switchSession('my session')
    GTlab> openProject('my project')
    GTlab> project = currentProject()
    GTlab> project.objectName
    'my project'


Managing Project Data
^^^^^^^^^^^^^^^^^^^^^

The project object returned by :py:func:`currentProject` provides access to the objects within the hierarchical data tree of the project. 
Any object from the data tree can be stored in a variable, which creates a direct reference to the object.
Accessing these objects allows reading or modifying their property values.

.. image:: images/python_console_data_management.gif
   :alt: Demo of managing project data via Python console
