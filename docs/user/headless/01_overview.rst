Overview
========

GTlab offers with the program GTlabConsole.exe the possibility to work with GTlab and on GTlab projects without the user interface.

The modular design of GTlab also comes into play here:
In addition to a series of basic commands, which are explained below, modules can also register supplementary commands for the console application, which are available depending on the local GTlab setup.

A good overview of the use of the console application and the available commands can be found by calling up the help with the command
GTlabConsole.exe --help (Windows) or GTlabConsole --help (Linux)

Execute a task from Mementos
----------------------------

``run_task_from_memento`` executes one task in an isolated ``GTlabConsole``
process. The project input is the ``GtObjectGroup`` Memento produced by
``GtProject::toProjectDataMemento()``. The output is the existing
``GtObjectMementoDiff`` XML representation without an additional wrapper.

.. code-block:: console

   GTlabConsole run_task_from_memento \
       --project-memento project_memento.xml \
       --task-memento task.xml \
       --output-diff result.diff.xml \
	   [--task-diff task.diff.xml] \
	   [--task-state taskstate.json] \
       [--working-directory working_dir]

The short option names are ``-p``, ``-t``, ``-o``, ``-m``, ``-s`` and ``-w``. If no working
directory is supplied, the directory containing the project Memento is used.
During execution this directory is both the process working directory and the
project path exposed by the execution context. Therefore existing calculators
using ``gtApp->currentProject()`` observe the temporary execution project.

The result file is published atomically only after successful task execution.
Logs and diagnostics remain on standard output and standard error. Direct file
system changes made by the task are not included in the project Memento-Diff.

Override task properties while running a task
---------------------------------------------

The ``run`` command accepts the repeatable option ``--set "<path>=<value>"`` to
overwrite task properties before the task is executed. The selected task is the
root of the path, therefore the task name itself is not part of it. Quote the
whole ``path=value`` argument so that the same command works in Bash,
PowerShell and ``cmd.exe``.

.. code-block:: console

   GTlabConsole run MyProject MyTask \
       --set "iterations=100" \
       --set "Solver.tolerance=1e-6" \
       --set "Solver/My Calculator[1].relaxation=0.5"

The path syntax is intentionally small:

- ``/`` navigates through the child objects, ``.`` switches to property access.
- ``ObjectName`` matches the object name and must resolve unambiguously,
  ``ObjectName[n]`` selects the one-based ``n``\ th child with that name;
  indices start at 1.
- ``{uuid}`` selects the direct child object with the given UUID.
- ``points[2].pressure`` selects the member of the second entry of a sequential
  property container. Numeric selectors are one-based and ``[0]`` is invalid.
  ``boundaries[{inlet}].pressure`` selects an associative entry by id.
- If ``Foo[1].bar`` matches both a child object and a property container on the
  current object, it is ambiguous. Prefix it with a dot (``.Foo[1].bar``) to
  force property-container access on the current object.

The value is converted and validated by the regular GTlab property mechanism,
so numbers are expected in the units of the property and read only and
monitoring properties cannot be changed. Multiple options for one property are
applied in command line order, the last value wins. If an override cannot be
applied, the task is not executed, the error is printed to standard error and
the exit code is not zero. Overrides are only written to the project if the
task is executed successfully and ``--save`` is given.

Overall, the use of the console application is correct in the form GTlabConsole.exe [options] <command>

A number of options and commands are generally available:

Options:
^^^^^^^^
--debug							Enables debug output and higher
--dev							Activate the developer mode
--help							Displays help on commandline options (also -h and -?)
--medium						Enables medium verbose output
--session <session_id>			Defines a session to be used for execution. (also --se)
--trace							Enables trace output and higher
--verbose						Enables very verbose output
--version						Displays the version number of GTlab (also -v)
   
Commands:	
^^^^^^^^^ 
.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Command
     - Description
   * - check_meta <input.xml>
     - Checks given meta process data.
   * - create_session
     - Creates a session if it doesn't exist already
   * - delete_session
     - Deletes the given session	
   * - enable_modules
     - Enables the modules specified. A module is disabled if it caused a crash on a previous application run.	
   * - footprint
     - Displays framework footprint				
   * - import_to_session
     - loads a project to the current session
   * - list
     - Shows list of modules, session, projects and tasks.
   * - list_variables
     - Lists the contents of all variables.
   * - process_runner
     - Starts a TCP server, which handles and executes task requests.
   * - python
     - Executes python
   * - run
     - Executes a process. To define a project name and a process name is the default used option to execute this command. Use --help for more details.	 
   * - run_meta <input.xml> <output.xml>
     - Executes given meta process data. Results are stored in given output file.
   * - run_task_from_memento
     - Executes a task from project and task Mementos and writes the resulting project Memento-Diff.
   * - set_variable
     - Sets a global variable that already exists in settings.	 						
   * - switch_session
     - Switches to the given session
   * - upgrade_project
     - Upgrades all modules in the current project	   
   * - load_module	<module_file>
     - Executes a test to load modules of the given arguments.	 						
						
