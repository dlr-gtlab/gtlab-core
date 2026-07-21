// GTlab - Gas Turbine laboratory
//
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Rectangle {
    implicitWidth: 1500
    implicitHeight: 50
    id: toolbarRoot

    property var theme: toolbar.theme

    property int spacing: 5

    color: theme.baseColor

    Rectangle {
        id: leftOverlay
        width: spacing + spacer.radius
        height: toolbarRoot.height - spacing * 2
        color: spacer.color
        y: spacing
    }

    Rectangle {
        id: rightOverlay
        width: spacing + spacer.radius
        height: toolbarRoot.height - spacing * 2
        color: spacer.color
        anchors.right: toolbarRoot.right
        y: spacing
    }

    // the list of groups
    Row {
        id: row

        spacing: toolbarRoot.spacing

        Repeater {
            id: toolbarRepeater
            model: toolbar.groups
            height: parent.height

            delegate: Column {

                property var groupActions: modelData
                property var groupName: modelData.name

                ToolbarGroup {
                    actions: groupActions
                    name: groupName
                    visible: modelData.visible
                    theme: toolbarRoot.theme
                }
            }
        }
    }

    // a resizable placeholder
    Rectangle {
        id: spacer

        radius: 10
        anchors.left: row.right
        anchors.leftMargin: toolbarRoot.spacing
        anchors.right: statusActions.count > 0 ? statusActions.left : toolbarRoot.right
        anchors.rightMargin: toolbarRoot.spacing
        color: toolbarRoot.theme.backgroundColor
        implicitHeight: toolbarRoot.implicitHeight - 2 * toolbarRoot.spacing
        y: toolbarRoot.spacing
    }

    // the list of status actions on the right
    ToolbarGroup {
        id: statusActions
        anchors.right: parent.right
        actions: toolbar.statusActions
        name: "status"
        theme: toolbarRoot.theme
    }
}
