

Feature: Test specific features of the process dock



    Scenario: Adding a Task

        Given GTlab is running in DEV mode
          And the dock widgets 'Explorer, Processes/Calculators' are visible
         When the standard testing project is opened
          And the Task Wizard is opened
         Then the 'Parameter Loop [PROTOTYPE]' is an addable task in the category 'Uncategorized'
         When the entry is selected
          And the Task Wizard proceeds
          And the value of the property 'start' is set to '1'
         Then the value is set correctly
         When the value of the property 'end' is set to '20'
         Then the value is set correctly
         When the value of the property 'steps' is set to '5'
         Then the value is set correctly
         When the Task Wizard finishes