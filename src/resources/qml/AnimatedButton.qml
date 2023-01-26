

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
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

    signal clicked()
    signal pressed()
    signal released()

    property color custom_hoverColor: "#ddeeff"
    property color custom_backgroundColor: "transparent"
    property color custom_secondaryColor: "#4582be"
    property color custom_iconColor: "black"
    property color custom_iconColorDark: "white"
    property color custom_secondaryColorInactive: "gainsboro"
    property color custom_secondaryColorInactiveDark: "#353b4a"
    property bool custom_Enabled: true

    icon.color: "transparent"
    icon.source: "qml_icons/home-icon.svg"
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
        //border.color: hovered ? "#047eff" : "transparent"

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
            }

            ColorOverlay {
                source: myContentIcon
                color: hovered ? (custom_Enabled ? custom_secondaryColor : (toolBar.dark_mode ? custom_secondaryColorInactiveDark : custom_secondaryColorInactive)) : toolBar.dark_mode ? (custom_Enabled ? custom_iconColorDark : custom_secondaryColorInactiveDark) : (custom_Enabled ? custom_iconColor : custom_secondaryColorInactive)
            }

            Text {
                id: myIconText
                text: control.text
                font: control.font
                color: toolBar.dark_mode ? (custom_Enabled ? custom_secondaryColor : custom_secondaryColorInactiveDark) : (custom_Enabled ? custom_secondaryColor : custom_secondaryColorInactive)
                //anchors.verticalCenter: parent.verticalCenter
                visible: hovered ? true : false
            }
        }
    }


    HoverHandler {
        id: mouse
        acceptedDevices: PointerDevice.Mouse
        cursorShape: Qt.PointingHandCursor
        onHoveredChanged:
        {
            if(hovered)
            {
                hoverenter_animation.restart()
                //active_animation.running = true
            }
            else
            {
                hoverleave_animation.restart()
                //inactive_animation.running = true
            }

        }
    }

    MouseArea {
            anchors.fill: parent
            onClicked: control.clicked()
            onPressed: control.pressed()
            onReleased: control.released()
    }

}

/*##^##
Designer {
    D{i:0;formeditorZoom:3}
}
##^##*/

