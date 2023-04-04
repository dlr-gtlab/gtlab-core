

Feature: GTlab has multiple toolbuttons in the main menu bar to acces certain functions fast

	Toolbuttons should be configured correctly after launch.

    Scenario: 	Certain actions should be disabled if no project is open
		Given 	GTlab is running
          And   no project is open
	     When   the action 'File' in the main menu bar is activated
         Then 	the action 'New Project' is enabled
          And 	the action 'Open Project' is enabled
          And 	the action 'Close Project' is disabled
          And 	the action 'Save Project' is disabled
          And 	the action 'Save As...' is disabled
         When   every visible menu is closed
          And   the action 'Edit' in the main menu bar is activated
         Then   the action 'Undo' in the submenu is disabled
          And   the action 'Redo' is disabled
         When   every visible menu is closed
          # Open a project and test again
          And any project is opened
	      And   the action 'File' in the main menu bar is activated
         Then 	the action 'New Project' is enabled
          And 	the action 'Open Project' is enabled
          And 	the action 'Close Project' is enabled
          And 	the action 'Save Project' is enabled
          And 	the action 'Save As...' is enabled
         When   every visible menu is closed
          And   the action 'Edit' in the main menu bar is activated
         Then   the action 'Undo' in the submenu is disabled
          And   the action 'Redo' is disabled

    Scenario: "Dev" action should be hidden in default mode

        Given GTlab is running
	      Then the action 'Dev' in the main menu bar does not exist

    Scenario: "Dev" action should be visible in default mode

        Given GTlab is running in DEV mode
	     Then the action 'Dev' in the main menu bar exists
	      # Test memento viewer
	     When the action 'Dev > Memento Viewer' in the main menu bar is activated
	      And the same action in the main menu bar is activated
	     Then the mdi tab 'Memento Viewer' exists 2 times
         When the mdi tab is closed
         Then the mdi tab exists once
         When the mdi tab is closed
         Then the mdi tab does not exists
	      # Test state browser
	     When the action 'Dev > State Browser' in the main menu bar is activated
	      And the same action in the main menu bar is activated
	     Then the mdi tab 'State Viewer' exists once
         When the mdi tab is closed
         Then the mdi tab does not exists
