
Feature: Test the Module Interface

    Scenario: Setup

        Given GTlab is running
          And only the module 'Test Module Interface' is included at startup


    Scenario: Test whether the Module is listed correctly in the Preferences

        # enable the required plugins in case they caused a crash
        Given the plugins 'ModuleInterface' are marked as reenabled at startup
        Given GTlab is running
          And the Preferences are open
         When the 'Plugins' tab in the Preferences is pressed
         Then the correct Preferences page is visible
          And the 'Test Module Interface' is a listed plugin
          And the version of the plugin is '0.1.2-desc'
          And the tooltip of the plugin reads 'Test Module Interface Description'