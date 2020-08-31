

Feature: GTlab has multiple toolbuttons in the main menu bar to acces certain functions fast

	Toolbuttons should be configured correctly after launch.

    Scenario: 	Pressing the New Project toolbutton initializes the New-Project-Wizard
		Given 	GTlab is running
		Then 	the 'Create New Project' toolbutton is enabled
		When 	the toolbutton is pressed
		Then 	the 'New Project' Wizard is enabled

    Scenario: 	If no Project is open, you cannot save a project
		Given   GTlab is running
		And 	no project is open
		Then 	the 'Save Project' toolbutton is greyed out
		When 	any project is opened
		Then 	the toolbutton is enabled

    Scenario: 	At launch some toolbuttons should be enabled or disabled
        Given 	GTlab is running
		Then 	the 'Create New Project' toolbutton is enabled
		And 	the 'Import Project' toolbutton is enabled
		And 	the 'Tile Windows' toolbutton is enabled
		And 	the 'Cascade Windows' toolbutton is enabled
		And 	the 'Print*' toolbutton is enabled
		# no actions to undo or redo
		But 	the 'Undo' toolbutton is greyed out
		And     the 'Redo' toolbutton is greyed out