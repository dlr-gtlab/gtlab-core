
Feature: Test specific features of the Output Dock

    Scenario: Test Navigation in the Output Dock

        Given GTlab is running in DEV mode
          And the dock widget 'Output' is visible
          # wait until initial output is printed
          And wait 2 seconds
         When the output is cleared
         Then no output lines are listed
         When the output is tested
         Then 5 output lines are listed
          And the 1. output line is of type 'DEBUG'
          And the 2. output line is of type 'INFO'
          And the 3. output line is of type 'WARNING'
          And the 4. output line is of type 'ERROR'
          And the 5. output line is of type 'FATAL'
