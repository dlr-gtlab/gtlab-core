
Feature: Test specific features of the process dock



    Scenario: Adding a Task

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


    Scenario: Adding an empty task

        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
          And the 'Add*' button is pressed
         Then a context menu is visible
         When the action 'Empty Task' is activated
         Then the process element 'New Task' exists
         When the process element is selected
          And the context menu of the process element is opened
          And the action 'Skip' is activated
         Then the text of the process entry is crossed out
         When the context menu of the process element is opened
          And the action 'Unskip' is activated
         Then the text of the process entry is not crossed out
