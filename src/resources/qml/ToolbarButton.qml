import QtQuick 2.15
import QtQuick.Controls 2.15

import de.gtlab 2.1

Item {

    id: tbButton
    property ToolbarAction action: None
    property bool dark_mode: false

    width: _button.width
    height: _button.height
    visible: action ? action.visible : false
    enabled: action ? action.enabled : false

    AnimatedButton {
        id: _button
        text: tbButton.action ? tbButton.action.text : ""
        onClicked: tbButton.action.triggered()
        icon.source: tbButton.action ? tbButton.action.iconUrl : ""
        darkmode: dark_mode
        tooltipText: tbButton.action ? tbButton.action.tooltip : ""
        hasTooltip: _button.tooltipText != ""
        custom_Enabled: tbButton.enabled
    }
}
