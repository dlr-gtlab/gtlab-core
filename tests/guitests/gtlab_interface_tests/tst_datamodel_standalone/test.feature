
Feature: Test the Datamodel Interface

    Scenario: Setup

        Given GTlab is running
          And only the module 'Test Datamodel Interface' is included at startup


    Scenario: Create a test project and check the implemented package

        # enable the required plugins in case they caused a crash  
        Given the plugins 'DatamodelInterface' are marked as reenabled at startup
        Given GTlab is running
          And the dock widget 'Explorer' is visible
         Then the project 'dm_test' does not exists
         # create a project
         When the 'Create New Project' toolbutton is pressed
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
         # set the name and path of the project
          And the input mask is filled out accordingly:
              | label_name | widget_type | input   |
              | Name:      | QLineEdit   | dm_test |
              | Directory: | QLineEdit   | $AUT/../tests/modules/resources/projects/dm_test |
         When the wizard proceeds
         # check that the Module exists as an entry
         Then the wizard page title reads 'Project Modules'
          And the wizard should contain a view of type 'QListView'
         Then the entry 'Test Datamodel Interface' exists
          And the entry is checked
         # finish the wizard
         When the wizard proceeds
          And the wizard finishes
         # test whether the project contains the correct package
         Then the project 'dm_test' exists
          And the project entry 'Test DMI Package' exists
         # save and reopen the project
         When the 'Save Project' toolbutton is pressed
          And the project is closed
          And the project is opened
         Then the entry exists
         # close and remove the project from the seesion
         When the project is closed
          And the project is removed from the session
         Then the project does not exists


    Scenario: Reload and delete the newly created project

        Given GTlab is running
          And the dock widget 'Explorer' is visible
         # load the project
         When the project 'dm_test' is loaded from '../tests/modules/resources/projects' relative to the AUT
         Then the project exists
          And the project entry 'Test DMI Package' exists
         # delete project permanently
         When the project is closed
          And the project is deleted permanently
         Then the project does not exists
          And the last output line reads '"Project has been deleted from hard-disk successfully"'

