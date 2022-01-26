
Feature: Test specific features of the explorer widget

    Beschreibung

	Scenario: No project is opened

        Given GTlab is running
          And the dock widgets 'Explorer, Processes/Calculators' are visible
          And no project is open
         Then no process elements are listed
          And the 'Run' button is greyed out
          And the 'Add...' button is greyed out


    Scenario: Opening a project

        Given GTlab is running
          And the dock widgets 'Explorer, Properties, Processes/Calculators' are visible
         When any project is opened
          And the entry is selected
         Then the property 'Path' exists
          And the 'Add...' button is enabled
          But the 'Run' button is greyed out


	Scenario: Opening and closing a project

        Given GTlab is running
          And the dock widgets 'Explorer, Processes/Calculators' are visible
          And any project is open
         When any open project is closed
         Then no process elements are listed
          And the 'Run' button is greyed out
          And the 'Add...' button is greyed out






