// GTlab - Gas Turbine laboratory
// SPDX-License-Identifier: MPL-2.0+
// SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
import QtQuick 2.15

Item {
    property var theme: null

    property int margin: 4

    width: sep.width + 2 * margin
    height: 30

    Rectangle {
        id: sep

        width: 2
        height: parent.height - 6
        color: theme.dark_mode ? Qt.lighter(theme.baseColor,
                                            1.2) : theme.baseColor
        anchors.centerIn: parent
    }
}
