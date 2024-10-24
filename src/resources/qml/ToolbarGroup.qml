// GTlab - Gas Turbine laboratory
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: toolbarGroup
    property string name: "group"
    property var actions: []
    property int count: 0

    property var theme: null

    width: rect.width
    height: rect.height

    visible: true

    Rectangle {
        id: rect
        implicitWidth: row.width + 10
        implicitHeight: 40

        visible: toolbarGroup.visible && toolbarGroup.count > 0
        color: toolbarGroup.theme.backgroundColor

//        Behavior on implicitWidth {
//            NumberAnimation {
//                duration: 100
//                easing.type: Easing.InOutQuad
//            }
//        }

        radius: 10
        x: 0
        y: 5

        Row {
            id: row
            x: 5
            y: 5
            spacing: 2

            // Animate width changes
            Repeater {
                model: actions

                ToolbarButton {
                    action: modelData
                    theme: toolbarGroup.theme
                }

                onCountChanged: {
                    toolbarGroup.count = count
                }
            }
        }
    }
}
