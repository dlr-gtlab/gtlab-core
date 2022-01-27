
Feature: Test the mdi interface

    Scenario: Setup

        Given GTlab is running
          And only the module 'Test Mdi Interface' is included at startup


    Scenario: The module package contains the implemented UI items

        # enable the required plugins in case they caused a crash
        Given the plugins 'MdiInterface' are marked as reenabled at startup
        Given GTlab is running
          And the dock widgets 'Explorer, Output' are visible
         # load the project
         When the project 'mdi_test' is loaded from '../tests/modules/resources/projects' relative to the AUT - expecting missing modules
         Then the project exists
         # check the icon
         When the project entry 'Test MDI Package' is selected
         Then the entry has a valid icon
         # check number of context menu entries
         When the context menu of the entry is opened
         Then the menu contains 3 entries
         # Only one mdi item is registered -> no open with menu visible
          And the action 'Open With' does not exists
         # check listed actions and check the output dock
         When the action 'Test Action Group > Test Group Action 1' is activated
         Then the last output line reads 'TEST MDI INTERFACE - TEST GROUP ACTION 1'
          And the last output line is of type 'INFO'
         When the action 'Test Action Group > Test Group Action 2' in the context menu of the entry is activated
         Then the last output line reads 'TEST MDI INTERFACE - TEST GROUP ACTION 2'
          And the last output line is of type 'INFO'
         When the action 'Test Action' in the context menu of the entry is activated
         Then the last output line reads 'TEST MDI INTERFACE - TEST ACTION'
          And the last output line is of type 'INFO'


    Scenario: The implemented DockWidgets work properly

        Given GTlab is running
         Then the dock widget 'Test Dock Widget' is registered
         When the dock widget is opened
         Then the widget of type 'QLabel' with the inscription 'Test Label' exists
         When the dock widget is closed
         Then the dock widget is not visible
          And the widget does not exists
         When the dock widget is opened
         Then the widget exists
         When the dock widget is set to float
         Then the dock widget is floating
         When the dock widget is set to anchor
         Then the dock widget is anchored


    Scenario: The implemented Mdi items work properly

        Given GTlab is running
         When the project 'mdi_test' is opened - expecting missing modules
          And the entry 'Test MDI Package' is selected
         # check the double clicked function
          And double clicked
         Then the last output line reads 'TEST MDI INTERFACE - DOUBLE CLICKED'
         # double click should open the only listed mdi item
          And the mdi tab 'Test Mdi Viewer' exists
         When the mdi tab is closed
         Then the mdi tab does not exists
         # check that action 'Open' opens the only listed mdi item
         When the action 'Open' in the context menu of the entry is activated
         Then the mdi tab exists
         # check that no multiple instances are allowed (open a second time, but the item still only exists once)
         When the same action in the context menu of the entry is activated
         Then the mdi tab exists once

    #Scenario: Interaction with the implemented GraphicsView Mdi Item

    #    Given GTlab is running
    #     Then the dock widget 'Explorer' is visible
    #     When the project 'mdi_test' is opened - expecting missing modules
    #      And the entry 'Test MDI Package' is selected
    #     # double click should open the only listed mdi item
    #      And double clicked
    #     Then the mdi tab 'Test Mdi Viewer' exists
    #      And the widget of type 'GtGraphicsView' exists
    #     When the widget is zoomed in max
    #     Then the widget exists

    Scenario: Remove the corresponding project from the active session

        Given GTlab is running
          And the dock widget 'Explorer' is visible
         When the project 'mdi_test' is removed from the session
         Then the project does not exists



