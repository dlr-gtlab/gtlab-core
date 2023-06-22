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
    flat: true

    icon.color: "transparent"
    icon.source: "Home-icon.svg"
    icon.width: 16
    icon.height: 16

    background: buttonBackground
    Rectangle {
        id: buttonBackground
        color: hovered ? "#ddeeff" : "transparent"
        //color: "#ddeeff"
        implicitWidth: 60
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3

        radius: 15
        //border.color: hovered ? "#047eff" : "transparent"

        //border.color: "#047eff"
        PropertyAnimation {
            id: active_animation
            target: buttonBackground
            property: "implicitWidth"
            to: 180
            duration: 100
        }

        PropertyAnimation {
            id: inactive_animation
            target: buttonBackground
            property: "implicitWidth"
            to: 60
            duration: 100
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
                //smooth: true
                //antialiasing: true
                //width: 10
                //height: 10
                //anchors.verticalCenter: parent.verticalCenter
                sourceSize: Qt.size(40, 40)

//                ColorOverlay {
//                    anchors.fill: myContentIcon
//                    source: myContentIcon
//                    color: hovered ? "#4582be" : "black"
//                }
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: mask
                }
            }

            Rectangle {
                id: mask
                //smooth: true
                //antialiasing: true
                width: 40
                height: 40
                radius: 20
                visible: false
            }

            Text {
                id: myIconText
                text: control.text
                font: control.font
                color: "#4582be"
                //anchors.verticalCenter: parent.verticalCenter
                visible: hovered ? true : false
            }
        }
    }

    //    Text {
    //        id: textItem
    //        text: control.text
    //        visible: hovered ? true : false

    //        opacity: enabled ? 1.0 : 0.3
    //        //color: "#93b5d9"
    //        color: "#047eff"
    //        horizontalAlignment: Text.AlignHCenter
    //        verticalAlignment: Text.AlignVCenter
    //    }
    HoverHandler {
        id: mouse
        acceptedDevices: PointerDevice.Mouse
        cursorShape: Qt.PointingHandCursor
        onHoveredChanged:
        {
            if(hovered)
            {
                active_animation.restart()
                //active_animation.running = true
            }
            else
            {
                inactive_animation.restart()
                //inactive_animation.running = true
            }

        }

    }

    MouseArea {
            anchors.fill: parent
            onClicked:
            {
                console.log("onclicked!")
                mainwin.openGitLabProjectOverview()
            }
    }

    //    states: [
    //        State {
    //            name: "normal"
    //            when: !control.down

    //            PropertyChanges {
    //                target: buttonBackground
    //                color: "#00000000"
    //                border.color: "#047eff"
    //                //implicitWidth: 80
    //            }

    //            PropertyChanges {
    //                target: textItem
    //                color: "#047eff"
    //            }
    //        },
    //        State {
    //            name: "down"
    //            when: control.down
    //            PropertyChanges {
    //                target: textItem
    //                color: "#ffffff"
    //            }

    //            PropertyChanges {
    //                target: buttonBackground
    //                color: "#047eff"
    //                border.color: "#00000000"
    //                implicitWidth: 120
    //            }
    //        }
    //    ]
}

/*##^##
Designer {
    D{i:0;formeditorZoom:3}
}
##^##*/

