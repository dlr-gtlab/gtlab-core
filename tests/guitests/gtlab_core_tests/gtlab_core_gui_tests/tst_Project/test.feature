
Feature: Test specific features for projects

	Scenario: Creating a project without an open project opens it

        Given GTlab is running
          And the dock widget 'Explorer' is visible
          And no project is open
          And the project 'issue1602_no_current' is removed from the session
          And the test project directory 'issue1602_no_current' is removed
         When the action 'File > New Project' in the main menu bar is activated
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input   |
              | Name:      | QLineEdit   | issue1602_no_current |
              | Directory: | QLineEdit   | $AUT/../tests/guitests/issue1602_no_current |
          And the wizard proceeds
          And the wizard proceeds
          And the wizard finishes
          And wait 5 seconds
         Then the project 'issue1602_no_current' exists
          And the project 'issue1602_no_current' is open
         When the project is closed
          And the project is deleted permanently
         Then the project does not exist

	Scenario: Creating a project while another project is open adds it to the session

        Given GTlab is running
          And the dock widget 'Explorer' is visible
          And no project is open
          And the project 'issue1602_current' is removed from the session
          And the project 'issue1602_added' is removed from the session
          And the project 'issue1602_switch_added' is removed from the session
          And the test project directory 'issue1602_current' is removed
         When the action 'File > New Project' in the main menu bar is activated
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input   |
              | Name:      | QLineEdit   | issue1602_current |
              | Directory: | QLineEdit   | $AUT/../tests/guitests/issue1602_current |
          And the wizard proceeds
          And the wizard proceeds
          And the wizard finishes
          And wait 5 seconds
         Then the project 'issue1602_current' exists
          And the project 'issue1602_current' is open
          And the test project directory 'issue1602_added' is removed
         When the action 'File > New Project' in the main menu bar is activated
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input   |
              | Name:      | QLineEdit   | issue1602_added |
              | Directory: | QLineEdit   | $AUT/../tests/guitests/issue1602_added |
          And the wizard proceeds
          And the wizard proceeds
          And the wizard finishes
          And the switch project prompt is answered with Cancel
          And wait 5 seconds
         Then the project 'issue1602_added' exists
          And the project 'issue1602_added' is closed
          And the project 'issue1602_current' is open
          And the test project directory 'issue1602_switch_added' is removed
         When the action 'File > New Project' in the main menu bar is activated
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input   |
              | Name:      | QLineEdit   | issue1602_switch_added |
              | Directory: | QLineEdit   | $AUT/../tests/guitests/issue1602_switch_added |
          And the wizard proceeds
          And the wizard proceeds
          And the wizard finishes
          And the switch project prompt is answered with Yes
          And wait 5 seconds
         Then the project 'issue1602_switch_added' exists
          And the project 'issue1602_switch_added' is open
          And the project 'issue1602_current' is closed
          And the project 'issue1602_added' is closed
         When the project is closed
          And the project 'issue1602_current' is removed from the session
          And the project 'issue1602_added' is removed from the session
          And the project 'issue1602_switch_added' is removed from the session
          And the test project directory 'issue1602_current' is removed
          And the test project directory 'issue1602_added' is removed
          And the test project directory 'issue1602_switch_added' is removed

	Scenario: Opening an existing project while another project is open switches to it

        Given GTlab is running
          And the dock widget 'Explorer' is visible
          And no project is open
          And the project 'TestProject' is removed from the session
         When the standard testing project is opened
         Then the project 'Empty-Test-Project' is open
         When the project 'TestProject' is loaded from '../tests/systemtests/import_to_session' relative to the AUT
          And the switch project prompt is answered with Yes
         Then the project 'TestProject' exists
          And the project 'TestProject' is open
          And the project 'Empty-Test-Project' is closed
         When the project is closed
          And the project 'TestProject' is removed from the session

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
          And the 'Add*' button is greyed out
