
Feature: Initialize and setup GTlab for GUI Testing

	Scenario: Load the standard project

	    Given GTlab is running
          And the dock widget 'Explorer' is visible
	      And the standard project is loaded from '../tests/guitests/testing_resources/_projects' relative to the AUT
