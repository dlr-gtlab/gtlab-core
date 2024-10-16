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
    property bool darkMode: toolbar.darkMode
    color: darkMode ? "#152231" : "#f0f0f0"
    id: toolBar

    property int spacing: 5

    // the list of groups
    Row {
        id: row

        spacing: toolBar.spacing

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
                    darkMode: toolBar.darkMode
                    visible: modelData.visible
                }
            }
        }
    }

    // a resizable placeholder
    Rectangle {
        id: spacer

        radius: 10
        anchors.left: row.right
        anchors.leftMargin: toolBar.spacing
        anchors.right: statusActions.count > 0 ? statusActions.left : toolBar.right
        anchors.rightMargin: toolBar.spacing
        color: toolBar.darkMode ? "#0a111f" : "#ffffff"
        implicitHeight: toolBar.implicitHeight - 2 * toolBar.spacing
        y: toolBar.spacing
    }

    // the list of status actions on the right
    ToolbarGroup {
        id: statusActions
        anchors.right: parent.right
        actions: toolbar.statusActions
        name: "status"
        darkMode: toolBar.darkMode
    }
}
