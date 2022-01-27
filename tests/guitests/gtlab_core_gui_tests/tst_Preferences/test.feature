

Feature: Testing specific features of the Preferences window

    Scenario: Tabs under Preferences have a dedicated page

        Given GTlab is running
          And the Preferences are open
         When the 'Application' tab in the Preferences is pressed
         Then the correct Preferences page is visible
         When the 'Session' tab in the Preferences is pressed
         Then the correct Preferences page is visible
         When the 'Perspectives' tab in the Preferences is pressed
         Then the correct Preferences page is visible
         When the 'Short Cuts' tab in the Preferences is pressed
         Then the correct Preferences page is visible
         When the 'Plugins' tab in the Preferences is pressed
         Then the correct Preferences page is visible
         When the 'Language' tab in the Preferences is pressed
         Then the correct Preferences page is visible
          And the Preferences page 'Access' is greyed out
         When the 'Environment' tab in the Preferences is pressed
         Then the correct Preferences page is visible


#    Scenario: No plugins are listed
#
#        Given GTlab is running
#          And the Preferences are open
#         When the 'Plugins' tab in the Preferences is pressed
#         Then the correct Preferences page is visible
#          And no plugins are listed
