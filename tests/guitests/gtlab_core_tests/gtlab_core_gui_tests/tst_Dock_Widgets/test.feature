
Feature: Testing general features of the dock widgets

    Tests the general behaviour of dock widgets (eg. float, anchor, closing, opening)

     Scenario Outline: Open each dock widget, then close and reopend it

        Given GTlab is running
          And the dock widget '<dock_widget>' is visible
         When the dock widget is closed
         Then the dock widget is not listed
         When the dock widget is opened
         Then the dock widget is listed
         #When the dock widget is set to float
         #Then the dock widget is floating
         #When the dock widget is set to anchor
         #Then the dock widget is anchored
	Examples:
    	| dock_widget |
		| Explorer |
		| Post Processing |
		| Labels |
		| Processes/Calculators |
		| Properties |
		| Output |

