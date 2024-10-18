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
            text: tbButton.action ? tbButton.action.text : ""
            onClicked: tbButton.action.triggered()
            icon.source: tbButton.action ? tbButton.action.iconUrl : ""
            darkMode: tbButton.darkMode
            tooltipText: tbButton.action ? tbButton.action.toolTip : ""
            hasTooltip: tooltipText != ""
            custom_Enabled: tbButton.enabled
        }
    }

    Component {
        id: sep

        ToolbarSeparator {
            darkMode: tbButton.darkMode
        }
    }
}
