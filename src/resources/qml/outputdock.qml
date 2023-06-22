// GTlab - Gas Turbine laboratory
//
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

import QtQuick 2.4
import QtQuick.Controls 1.0

Item {
//    width: _width
//    height: 200

    ListView {
        id: outputdock
        anchors.fill: parent
        spacing: 2

        model: _cppModel

        delegate: Rectangle {
            width: ListView.view.width
            height: message.height

//            color: "lightblue"

//            Image {
//                id: imageItem
//                height: parent.height; width: 25
//                anchors.left: parent.left
//                // delegate can directly use ListElement role name
//                source: model.decoration
//              }

            Text {
                id: message
                anchors {
//                    fill: parent
//                    left: imageItem.right
                    margins: 1
                }

                text: qsTr("%1").arg(model.display)
            }
        }

        onCountChanged: {
            var newIndex = count - 1 // last index
            outputdock.positionViewAtEnd()
            currentIndex = newIndex
        }
    }

//    Row {
//        height: 50
//        anchors {
//            left: parent.left
//            right: parent.right
//            bottom: parent.bottom
//        }

//        Button {
//            text: "Add"
//            onClicked: _cppModel.prependNewSong()
//        }

//        Button {
//            text: "Remove"
//            onClicked: _cppModel.removeLast()
//        }
//    }
}
