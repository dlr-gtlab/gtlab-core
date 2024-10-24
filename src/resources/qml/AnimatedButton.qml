// GTlab - Gas Turbine laboratory
//
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
//import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.15

Button {
    id: control

    property string tooltipText: ""
    property string tooltipPrefix: ""
    property bool hasTooltip: false

    property bool darkMode: true

    implicitWidth: Math.max(
                       buttonBackground ? buttonBackground.implicitWidth : 0,
                       myContentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(
                        buttonBackground ? buttonBackground.implicitHeight : 0,
                        myContentItem.implicitHeight + topPadding + bottomPadding)
    leftPadding: 4
    rightPadding: 4

    text: "My Button"
    hoverEnabled: true
    checkable: false
    flat: true

    signal clicked
    signal pressed
    signal released

    property color custom_hoverColor: "#ddeeff"
    property color custom_backgroundColor: "transparent"
    property color custom_secondaryColor: "#4582be"
    property color custom_iconColor: "black"
    property color custom_iconColorDark: "white"
    property color custom_secondaryColorInactive: "gainsboro"
    property color custom_secondaryColorInactiveDark: "#353b4a"
    property bool custom_Enabled: true

    icon.color: "transparent"
    icon.source: "../icons/home.svg"
    icon.width: 16
    icon.height: 16
    property ParallelAnimation active_animation: active_animation
    property ParallelAnimation inactive_animation: inactive_animation

    background: buttonBackground
    Rectangle {
        id: buttonBackground
        color: hovered ? (custom_Enabled ? custom_hoverColor : custom_backgroundColor) : custom_backgroundColor
        //y: 5
        implicitWidth: 60
        implicitHeight: 30
        opacity: enabled ? 1 : 0.3

        radius: 15

        border.color: checked ? (darkMode ? Qt.lighter(custom_backgroundColor)
                                          : Qt.darker(custom_backgroundColor, 1.1)) : "transparent"

        //border.color: "#047eff"
        ColorAnimation on color {
            id: pressed_animation
            //target: buttonBackground
            to: Qt.lighter(custom_hoverColor, 1.04)
            duration: 200
            running: false
        }

        ColorAnimation on color {
            id: released_animation
            //target: buttonBackground
            to: custom_hoverColor
            duration: 200
            running: false
        }

        PropertyAnimation {
            id: hoverenter_animation
            target: buttonBackground
            property: "implicitWidth"
            to: 120
            duration: 100
        }

        PropertyAnimation {
            id: hoverleave_animation
            target: buttonBackground
            property: "implicitWidth"
            to: 60
            duration: 100
        }

        ParallelAnimation {
            id: active_animation
            running: false
            PropertyAnimation {
                target: control
                property: "implicitWidth"
                from: 0
                to: Math.max(
                        buttonBackground ? buttonBackground.implicitWidth : 0,
                        myContentItem.implicitWidth + leftPadding + rightPadding)
                duration: 200

            }
            PropertyAnimation {
                target: control
                property: "scale"
                from: 0.
                to: 1.
                duration: 200
            }
            onStarted: control.visible = true
        }

        ParallelAnimation {
            id: inactive_animation
            running: false
            PropertyAnimation {
                target: control
                property: "implicitWidth"
                from: Math.max(
                          buttonBackground ? buttonBackground.implicitWidth : 0,
                          myContentItem.implicitWidth + leftPadding + rightPadding)
                to: 0
                duration: 200

            }
            PropertyAnimation {
                target: control
                property: "scale"
                from: 1.
                to: 0.
                duration: 200
            }
            onFinished: control.visible = false
        }
    }

    contentItem: myContentItem

    Item {
        id: myContentItem
        width: parent.width
        height: parent.height

        //anchors.centerIn: buttonBackground
        RowLayout {

            anchors.centerIn: parent
            spacing: 5
            Image {
                id: myContentIcon
                source: control.icon.source
                //width: 10
                //height: 10
                //anchors.verticalCenter: parent.verticalCenter
                sourceSize: Qt.size(20, 20)
                smooth: true
                antialiasing: true
                layer.enabled: true
                layer.effect: ColorOverlay {
                    color: hovered ? (custom_Enabled ? custom_secondaryColor : (control.darkMode ? custom_secondaryColorInactiveDark : custom_secondaryColorInactive)) : control.darkMode ? (custom_Enabled ? custom_iconColorDark : custom_secondaryColorInactiveDark) : (custom_Enabled ? custom_iconColor : custom_secondaryColorInactive)
                }
            }

            Text {
                id: myIconText
                text: control.text
                font: control.font
                color: control.darkMode ? (custom_Enabled ? custom_secondaryColor : custom_secondaryColorInactiveDark) : (custom_Enabled ? custom_secondaryColor : custom_secondaryColorInactive)
                //anchors.verticalCenter: parent.verticalCenter
                visible: hovered ? true : false
            }
        }


    }


    HoverHandler {
        id: mouse
        acceptedDevices: PointerDevice.Mouse
        cursorShape: Qt.PointingHandCursor
        onHoveredChanged: {
            toolTip.visible = hasTooltip && tooltipText !== "" ? hovered : false

            if (hovered) {
                hoverenter_animation.restart()
                //active_animation.running = true
            } else {
                hoverleave_animation.restart()
                //inactive_animation.running = true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            control.clicked()
            if (control.checkable) {
                control.checked = !control.checked
            }
        }

        onPressed: control.pressed()
        onReleased: control.released()

        ToolTip {
            id: toolTip
            delay: 1000
            timeout: 5000
            text: tooltipPrefix + tooltipText
            visible: false
            x: parent.width * 2 / 3
            y: parent.height / 2 + 10

            onTextChanged: {
                if (visible) {
                    visible = (tooltipText !== "")
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:3}
}
##^##*/

