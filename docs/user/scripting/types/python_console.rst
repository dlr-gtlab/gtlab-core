The Python Console
------------------

GTlab provides an integrated Python console that allows you to control GTlab and directly manage project data using Python code.
The main features of the Python console include:

- Switching between sessions
- Opening projects
- Navigating project and process data
- Modifying project data
- Executing tasks

In addition, the Python console enables direct interaction with the selected Python environment, making it possible, for example, to import Python modules or extend the ``sys.path`` list.

The Python console is located in the Output dock widget (``Window → Dock Widgets → Output``) under the *Python Console* tab.


Usage
~~~~~

The console displays the prompt ``GTlab>``, which is ready to accept Python code input.
Pressing ``Enter`` executes the code. ``print()`` outputs, return values and error messages appear directly below the input.
By using the shortcut ``Shift+Enter``, you can also enter multi-line Python code, for example to define functions.

The auto-completion feature assists with writing Python code and includes all objects, functions, classes and modules available in the Python environment.

.. |clear| image:: ../static/icons/clear.svg
   :alt: clear
   :height: 16px
   :width: 16px

You can clear the console content by clicking the |clear| button in the lower-right corner.

.. image:: ../static/images/python_console.gif
   :alt: Demo of the Python console
   :width: 400px


Controlling GTlab
~~~~~~~~~~~~~~~~~

The Python console allows you to directly call core functions of GTlab.
This includes switching between sessions, opening projects and accessing the currently opened project.

.. code-block:: python

    GTlab> switchSession('my session')
    GTlab> openProject('my project')
    GTlab> project = currentProject()
    GTlab> project.objectName
    'my project'


