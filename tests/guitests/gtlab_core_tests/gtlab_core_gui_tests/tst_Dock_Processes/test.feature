
Feature: Test specific features of the process dock


    Scenario: Adding a Task using the wizard

        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
         # add an process element and set some properties
          And the Process Task Wizard is opened
         Then the process element 'Parameter Loop [RELEASE]' is listed within the category 'Uncategorized'
         When the entry is selected
          And the wizard proceeds
         When the value of the property 'start' is set to '1'
         Then the value is set correctly
         When the value of the property 'end' is set to '20'
         Then the value is set correctly
         When the value of the property 'steps' is set to '5'
         Then the value is set correctly
         When the wizard finishes
         # process element should be added and properties should be set correctly
         Then the process element 'Parameter Loop' exists
          And the property 'start' has the value '1'
          And the property 'end' has the value '20'
          And the property 'steps' has the value '5'
		 # check that the stop action is disabled
		 When the context menu of the process element is opened
         Then the action 'Run Task' exists
		  But the action 'Stop Task' does not exist


    Scenario: Adding an empty task

        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
		 # add empty task using the add button
          And the 'Add*' button is pressed
         Then a context menu is visible
         When the action 'Empty Root Task' is activated
         Then the process element 'New Task' exists
         When the process element is selected
		 # add a second empty task as a second element
		  And the context menu of the process element is opened
          And the action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task[2]' exists


	Scenario: Skipping a task
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
		  And the context menu of the process view is opened
		 # add empty task
		  And the action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task' exists
		 # test skip and unskip
         When the context menu of the process element is opened
          And the action 'Skip' is activated
         Then the text of the process entry is crossed out
         When the context menu of the process element is opened
          And the action 'Unskip' is activated
         Then the text of the process entry is not crossed out


	Scenario: Copy and pasting a task
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
		  And the context menu of the process view is opened
		 # check default context menu
		 Then the action 'Paste' is disabled
		  And the action 'Import' is enabled
		 # add empty task
		 When the action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task' exists
		 # copy and paste
         When the context menu of the process element is opened
          And the action 'Copy' is activated
		  And the context menu of the process view is opened
		 Then the action 'Paste' is enabled
		 When the action 'Paste' is activated
         Then the process element 'New Task[2]' exists


	Scenario: Running a task
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
         Then the 'Run*' button is greyed out
         # add process element
		 When the context menu of the process view is opened
		  And the action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task' exists
		 # run task using context menu
         When the process element is selected
         Then the 'Run*' button is enabled
         When the button is pressed
         # give it time to execute
         Then wait 1 seconds
         # check success
         Then the process element state is 'Finished'


    Scenario: Pasting copied tasks into a task using the menu and Ctrl+V
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
          And the context menu of the process view is opened
          And the process action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task' exists
         When the process element is selected
          And the selected process is remembered as the paste target
          And the context menu of the selected process is opened
          And the process action 'Copy' is activated
          And the context menu of the selected process is opened
         Then the process action 'Paste' is enabled
         When the process action 'Paste' is activated
         Then the paste target has 1 additional children
          And the pasted component is a 'task' named 'New Task'
         # Copy the selected child task and paste it back as a sibling subtask.
         When the context menu of the selected process is opened
          And the process action 'Copy' is activated
          And the remembered paste target is selected
          And the context menu of the selected process is opened
         Then the process action 'Paste' is enabled
         When the process action 'Paste' is activated
         Then the paste target has 2 additional children
          And the pasted component is a 'task' named 'New Task[2]'
         When the remembered paste target is selected
          And Ctrl+V is pressed in the process view
         Then the paste target has 3 additional children
          And the pasted component is a 'task' named 'New Task[3]'


    Scenario: Pasting calculators preserves destination restrictions
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
          And the context menu of the process view is opened
          And the process action 'Add... > Empty Root Task' is activated
         Then the process element 'New Task' exists
         When the process element is selected
          And the selected process is remembered as the paste target
          And the context menu of the selected process is opened
          And the process action 'Add... > New Calculator...' is activated
          And the Sleep calculator is added using the process wizard
         Then the process element 'Sleep' exists
         When the process element is selected
          And the context menu of the selected process is opened
          And the process action 'Copy' is activated
          And the context menu of the process view is opened
         Then the process action 'Paste' is disabled
         When the process context menu is dismissed
          And the remembered paste target is selected
          And the context menu of the selected process is opened
         Then the process action 'Paste' is enabled
         When the process action 'Paste' is activated
         Then the paste target has 2 additional children
          And the pasted component is a 'calculator' named 'Sleep[2]'
         When the remembered paste target is selected
          And Ctrl+V is pressed in the process view
         Then the paste target has 3 additional children
          And the pasted component is a 'calculator' named 'Sleep[3]'
         # The pasted calculator is selected; calculators cannot contain children.
         When the selected process is remembered as the paste target
          And the context menu of the selected process is opened
         Then the process action 'Paste' does not exist
         When the process context menu is dismissed
          And Ctrl+V is pressed in the process view
         Then the paste target has 0 additional children
         # Copied tasks must not be offered to a running destination.
         Then the process element 'New Task' exists
         When the process element is selected
          And the selected process is remembered as the paste target
          And the context menu of the selected process is opened
          And the process action 'Copy' is activated
          And the context menu of the selected process is opened
          And the process action 'Run Task' is activated
         Then the remembered paste target is running
         When the context menu of the selected process is opened
         Then the process action 'Paste' is disabled
         When the process context menu is dismissed
          And Ctrl+V is pressed in the process view
         Then the paste target has 0 additional children
         When the context menu of the selected process is opened
          And the process action 'Stop Task' is activated
