// GTlab - Gas Turbine laboratory
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
import QtQuick 2.15
import QtQuick.Controls 2.15

import de.gtlab 2.1

Item {

    id: tbButton
    property ToolbarAction action: None
    property bool darkMode: false

    width: l.width
    height: l.height
    visible: action ? action.visible : false
    enabled: action ? action.enabled : false

    Loader {
        id: l
        sourceComponent: action ? (action.isSeparator() ? sep : btn) : null
    }

    Component {
        id: btn

        AnimatedButton {
            id: b

            text: tbButton.action ? tbButton.action.text : ""
            onClicked: tbButton.action.triggered()

            onCheckedChanged: {
                if (tbButton.action) {
                    tbButton.action.checked = b.checked
                    tbButton.action.toggled(b.checked)
                }
            }

            function bgColor() {
                if (!checked)
                    return "transparent"
                else
                    return darkMode ? "#1e2a3a" : "#efefef"
            }

            custom_backgroundColor: bgColor()

            icon.source: tbButton.action ? tbButton.action.iconUrl : ""
            darkMode: tbButton.darkMode
            tooltipText: tbButton.action ? tbButton.action.toolTip : ""
            hasTooltip: tooltipText != ""
            custom_Enabled: tbButton.enabled
            checkable: tbButton.action ? tbButton.action.checkable : false
        }
    }

    Component {
        id: sep

        ToolbarSeparator {
            darkMode: tbButton.darkMode
        }
    }
}
