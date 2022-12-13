
Feature: Initialize and setup GTlab for GUI Testing

	Scenario: Load the standard project

        Given The default settings are restored
	      And GTlab is running
          And the dock widget 'Explorer' is visible
          # remove the old project as it may not be the correct one
          And the standard project is removed from the session
          # load the correct project
	      And the standard project is loaded from '../tests/guitests/testing_resources/_projects' relative to the AUT
