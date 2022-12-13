
Feature: Test specific features for projects

	Scenario: Opening and closing a project should update the properties and process dock widgets

        Given GTlab is running
          And the dock widgets 'Explorer, Properties, Processes/Calculators' are visible
          # no project is open
          And no project is open
         Then no property categories are listed
          And no process elements are listed
          And the 'Run' button is greyed out
          And the 'Add*' button is greyed out
          # opening a project
         When the standard testing project is opened
          And the entry is selected
         Then a single property category is listed
         Then the property 'Path' exists
          And the 'Add*' button is enabled
          But the 'Run' button is greyed out
          # closing the project
         When the project is closed
         Then no process elements are listed
          And the 'Run' button is greyed out
          # And the 'Add*' button is greyed out
