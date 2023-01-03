import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

Rectangle {
        id: dockBar
        implicitWidth: 50
        implicitHeight: 25
        property bool dark_mode: false
        color: dark_mode ? "#0a111f" : "#ffffff"

//        Rectangle {
//            id: back_rect
//            color: dark_mode ? "#152231" : "#f0f0f0"
//            implicitWidth: parent.width
//            implicitHeight: parent.height
//        }
		
		Text {
            x: 5
			y: 4
            text: dockwidget.objectName
			font.family: "Helvetica"
            font.pointSize: 8
			color: "black"
		}

        RowLayout {
            anchors.fill: parent
            spacing: 2
            layoutDirection: Qt.RightToLeft

            DockBarButton {
                id: btn_close
                objectName: "btn_close"
                icon.source: "qml_icons/close-icon.svg"
                text: "Close"
                visible: false

                ParallelAnimation {
                    id: btn_close_active_anim
                    running: false
                    PropertyAnimation {
                        target: btn_close
                        property: "scale"
                        from: 0
                        to: 1
                        duration: 200

                    }
                    onStarted: btn_close.visible = true
                }

                ParallelAnimation {
                    id: btn_close_inactive_anim
                    running: false
                    PropertyAnimation {
                        target: btn_close
                        property: "scale"
                        from: 1
                        to: 0
                        duration: 200

                    }
                    onFinished: btn_close.visible = false
                }
            }
        }

        HoverHandler {
            id: mouse
            acceptedDevices: PointerDevice.Mouse

            onHoveredChanged:
            {

                console.log("onHoveredChanged!")
                if(hovered)
                {
                    btn_close_inactive_anim.complete()
                    btn_close_active_anim.restart()
                }
                else
                {
                    btn_close_active_anim.complete()
                    btn_close_inactive_anim.restart()
                }

            }
        }
}
