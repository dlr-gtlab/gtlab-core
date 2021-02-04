

Feature: Test specific features of the properties dock



    Scenario: Opening a project

        Given GTlab is running
	  	  And the dock widgets 'Explorer, Properties' are visible
         When the standard testing project is opened
         Then the property 'Path' exists
    #     And the entry 'PreDesign > Fan' is selected
	#	 Then under the category 'Main' the property 'Center of Gravity > cogZ' exists
	#	  And has the unit 'm'
	#	  And has the value '0'
	#	 When the value is set to '123'
	#	 Then the value is set correctly
	#	 When the unit is set to 'dm'
	#	 Then the unit is set correctly
	#	  And has the value '1230'



