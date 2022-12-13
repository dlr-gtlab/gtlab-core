
Feature: GTlab allows the use of shortcuts for certain actions

	Tests the functionality the shortcut system in GTlab
	
    Scenario: Restoring the shortcuts via the preferences
		Given GTlab is running
        And   the Preferences are open
        When  the 'Short Cuts' tab in the Preferences is pressed
        Then  the correct Preferences page is visible
        And   the 'Restore' button is pressed
        When  the 'Save' button is pressed

    Scenario: Changing shortcuts in the preferences
		Given GTlab is running
        And   the Preferences are open
        When  the 'Short Cuts' tab in the Preferences is pressed
        Then  the correct Preferences page is visible
        When  the shortcut view is sorted by name
        # restore defaults
        And   the 'Restore' button is pressed
        # valid key sequence
        Then  the shortcut 'openContextMenu' exists
        When  the shortcut is changed to 'Ctrl+F3'
        Then  the shortcut is accepted
        # only a simple key -> invalid shortcut
        Then  the shortcut 'showFootprint' exists
        When  the shortcut is changed to 'R'
        Then  the shortcut is unchanged
         And  the shortcut is restored
        When  the shortcut is changed to 'Alt+F2'
        Then  the shortcut is accepted
        # exit preferences
        When  the 'Save' button is pressed
        # reopen preferences and check shortcuts
        Given the Preferences are open
        When  the 'Short Cuts' tab in the Preferences is pressed
        Then  the shortcut 'showFootprint' exists
         And  the shortcut is set to 'Alt+F2'
        Then  the shortcut 'openContextMenu' exists
         And  the shortcut is set to 'Ctrl+F3'
        # exit the input widget
        When  the shortcut is changed to 'ESC'
        Then  the shortcut is blank

    Scenario: Restoring default shortcuts
        Given GTlab is running
        And   the Preferences are open
        When  the 'Short Cuts' tab in the Preferences is pressed
        Then  the correct Preferences page is visible
        When  the shortcut view is sorted by name
        # restore defaults
        And   the 'Restore' button is pressed
        # change a shortcut
        Then  the shortcut 'openContextMenu' exists
        When  the shortcut is changed to 'Ctrl+O'
        Then  the shortcut is accepted
        When  the 'Restore' button is pressed
        Then  the shortcut is restored
        # clear a shortcut
        Then  the shortcut 'showFootprint' exists
        When  the shortcut is changed to 'Esc'
        Then  the shortcut is blank
        When  the 'Restore' button is pressed
        Then  the shortcut is restored
