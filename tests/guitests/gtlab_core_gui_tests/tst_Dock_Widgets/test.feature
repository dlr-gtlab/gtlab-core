

Feature: Testing specific features of the main window

    Some textual description of the business value of this feature goes
    here. The text is free-form.

    The description can span multiple paragraphs.

    Scenario Outline: Open each dock widget, then close and reopend it

        Given GTlab is running
          And the dock widget '<dock_widget>' is visible
         When the dock widget is closed
         Then the dock widget is not listed
         When the dock widget is opened
         Then the dock widget is listed
         When the dock widget is set to float
         Then the dock widget is floating
         When the dock widget is set to anchor
         Then the dock widget is anchored
	Examples:
		| dock_widget |
		| Explorer |
		| Post Processing |
		| Labels |
		| Processes/Calculators |
		| Properties |
		| Output |

