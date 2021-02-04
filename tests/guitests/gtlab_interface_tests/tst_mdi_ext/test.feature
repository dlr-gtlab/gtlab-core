
Feature: Test the mdi interface by extending a module

    Scenario: Setup

        Given GTlab is running
          And only the modules 'Test Mdi Interface, Test Mdi Interface Ext' are included at startup


    Scenario: The module package contains the extended UI items

        Given GTlab is running
          And the dock widgets 'Explorer, Output' are visible
         # load the project
         When the project 'mdi_test' is loaded from '../tests/modules/resources/projects' relative to the AUT
         Then the project exists
         # check that only the base Package is listed
          And the project entry 'Test MDI EXT Package' does not exists
          But the project entry 'Test MDI Package' exists
         # check that the icon is still valid
         When the entry is selected
         Then the entry has a valid icon
         # check number and content of context menu entries
         When the context menu of the entry is opened
         Then the menu contains 6 entries
          And the menu contains the actions 'Open, Open With, Test Action Group, Test Action Group 2, Test Action, Test Action 2' in order
         # check listed actions and check the output dock
         When the action 'Test Action Group 2 > Test Group Action' is activated
         Then the last output line reads 'TEST MDI INTERFACE EXT - TEST GROUP ACTION'
          And the last output line is of type 'INFO'
         When the action 'Test Action 2' in the context menu of the entry is activated
         Then the last output line reads 'TEST MDI INTERFACE EXT - TEST ACTION'
          And the last output line is of type 'INFO'


    Scenario: The implemented Mdi Items work properly

        Given GTlab is running
         When the project 'mdi_test' is opened - expecting missing modules
          And the entry 'Test MDI Package' is selected
         # check that the base mdi module doubleClicked-function has been called
          And double clicked
         Then the last output line reads 'TEST MDI INTERFACE - DOUBLE CLICKED'
         # double click should open the first item in the 'Open With' menu
          And the mdi tab 'Test Mdi Viewer' exists
         When the mdi tab is closed
         Then the mdi tab does not exists
         # 'Open' should open the first item in the 'Open With' menu
         When the action 'Open' in the context menu of the entry is activated
         Then the mdi tab exists once
         When the mdi tab is closed
         # check the order and content of the open with actions
         When the action 'Open With' in the context menu of the entry is activated
         Then the submenu contains the actions 'TestMdiViewer, TestMdiExtViewer' in order
         When the action 'TestMdiViewer' is activated
         Then the mdi tab exists
         # check that multiple instances are allowed (eg. open two times)
         When the action 'Open With > TestMdiExtViewer' in the context menu of the entry is activated
          And the same action in the context menu of the entry is activated
         Then the mdi tab 'Test Mdi Ext Viewer' exists 2 times
         When the mdi tab is closed
         Then the mdi tab exists once
         When the mdi tab is closed
         Then the mdi tab does not exists


    Scenario: Remove the corresponding project from the active session

        Given GTlab is running
          And the dock widget 'Explorer' is visible
         When the project 'mdi_test' is removed from the session
         Then the project does not exists

