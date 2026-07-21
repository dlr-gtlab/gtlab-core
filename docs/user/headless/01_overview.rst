Overview
========

GTlab offers with the program GTlabConsole.exe the possibility to work with GTlab and on GTlab projects without the user interface.

The modular design of GTlab also comes into play here:
In addition to a series of basic commands, which are explained below, modules can also register supplementary commands for the console application, which are available depending on the local GTlab setup.

A good overview of the use of the console application and the available commands can be found by calling up the help with the command
GTlabConsole.exe --help (Windows) or GTlabConsole --help (Linux)

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
   * - set_variable
     - Sets a global variable that already exists in settings.	 						
   * - switch_session
     - Switches to the given session
   * - upgrade_project
     - Upgrades all modules in the current project	   
   * - load_module	<module_file>
     - Executes a test to load modules of the given arguments.	 						
						
