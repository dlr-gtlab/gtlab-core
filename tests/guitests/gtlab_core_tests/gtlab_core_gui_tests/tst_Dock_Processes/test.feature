
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
         When the action 'Empty Task' is activated
         Then the process element 'New Task' exists
         When the process element is selected
		 # add a second empty task as a child
		  And the context menu of the process element is opened
          And the action 'Add... > Empty Task' is activated
         Then the process element 'New Task > New Task' exists


	Scenario: Skipping a task
        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
		  And the context menu of the process view is opened
		 # add empty task
		  And the action 'Add... > Empty Task' is activated
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
		 When the action 'Add... > Empty Task' is activated
         Then the process element 'New Task' exists
		 # copy and paste
         When the context menu of the process element is opened
          And the action 'Copy' is activated
		  And the context menu of the process view is opened
		 Then the action 'Paste' is enabled
		 When the action 'Paste' is activated
         Then the process element 'New Task[2]' exists
