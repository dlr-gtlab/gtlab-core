// GTlab - Gas Turbine laboratory
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
import QtQuick 2.15
import QtQuick.Controls 2.15

import de.gtlab 2.1

Item {

    id: tbButton
    property ToolbarAction action: None
    property var theme: null

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
                if (tbButton.action && tbButton.action.checked !== checked) {
                    tbButton.action.checked = b.checked
                    tbButton.action.toggled(b.checked)
                }
            }

            function bgColor() {
                if (!checked)
                    return theme.backgroundColor
                return Qt.tint(theme.backgroundColor, "#30999999")
            }

            custom_backgroundColor: bgColor()

            icon.source: tbButton.action ? tbButton.action.iconUrl : ""
            darkMode: tbButton.theme.darkMode
            tooltipText: tbButton.action ? tbButton.action.toolTip : ""
            hasTooltip: tooltipText != ""
            custom_Enabled: tbButton.enabled
            checkable: tbButton.action ? tbButton.action.checkable : false
            custom_hoverColor: tbButton.theme.hoverColor

            function syncButtonCheckedState() {
                if (b.checked !== tbButton.action.checked) {
                    b.checked = tbButton.action.checked
                }
            }

            // make the button change, if the action changes
            Connections {
                target: tbButton.action

                function onCheckedChanged() {
                    syncButtonCheckedState()
                }
            }

            Component.onCompleted: {
                syncButtonCheckedState()
            }
        }
    }

    Component {
        id: sep

        ToolbarSeparator {
            theme: tbButton.theme
        }
    }
}
