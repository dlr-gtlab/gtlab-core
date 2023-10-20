// GTlab - Gas Turbine laboratory
//
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
	id: mainScreen
    width: 820
    height: 680
    visible: true
	property bool dark_mode: false

    Rectangle {
        id: text_rect
        anchors.fill: parent
        color: dark_mode ? "#0a111f" : "white"

        ScrollView {
            anchors.fill: parent
            clip: true

            TextArea {
                readOnly: true
                clip: true
                textFormat: TextEdit.MarkdownText
                text: markdown_text
                wrapMode: Text.WordWrap
                color: dark_mode ? "white" : "black"
            }
        }
    }

    Connections
    {
        target: gtapp

        function onThemeChanged(dark)
        {
            dark_mode = dark;
        }

    }

    Component.onCompleted:
    {
        dark_mode = gtapp.inDarkMode();
    }
}
