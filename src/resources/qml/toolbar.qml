/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Quick Studio Components.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

Rectangle {
        id: toolBar
        implicitWidth: 1500
        implicitHeight: 50
        property bool dark_mode: false

        Rectangle {
            id: back_rect
            color: dark_mode ? "#152231" : "#f0f0f0"
            implicitWidth: parent.width
            implicitHeight: parent.height
        }

        Row {
            spacing: 5

            Rectangle {
                id: system_context
                color: dark_mode ? "#0a111f" : "#ffffff"
                implicitWidth: row.width + 10
                implicitHeight: 40

                radius: 10
                x: 5
                y: 5

                Row {
                    id: row
                    x: 5
                    y: 5
                    spacing: 2

                    AnimatedButton {
                        id: btnHome
                        objectName: "btnHome"
                        text: "Home"
                    }
                    AnimatedButton {
                        id: btnNewProject
                        objectName: "btnNewProject"
                        icon.source: "qml_icons/newproject-icon.svg"
                        icon.width: 20
                        icon.height: 20
//                        custom_hoverColor: "#f5deb3"
//                        custom_secondaryColor: "#cd853f"
//                        custom_iconColor: custom_secondaryColor
//                        custom_iconColorDark: custom_secondaryColor
//                        custom_backgroundColor: "#f5deb3"
                        text: "New Project"
                    }
                    AnimatedButton {
                        id: btnOpenProject
                        objectName: "btnOpenProject"
                        icon.source: "qml_icons/open-icon.svg"
                        icon.width: 16
                        icon.height: 16
                        text: "Open Project"
                    }
                }
            }

            Rectangle {
                id: project_context
                color: dark_mode ? "#0a111f" : "#ffffff"
                implicitWidth: visible ? project_context_row.width + 10 : 0
                implicitHeight: 40

                radius: 10
                x: 5
                y: 5

                ParallelAnimation {
                    id: project_context_active_anim
                    running: false
                    PropertyAnimation {
                        target: project_context
                        property: "implicitWidth"
                        from: -5
                        to: project_context_row.width + 10
                        duration: 200

                    }
                    onStarted: project_context.visible = true
                }

                ParallelAnimation {
                    id: project_context_inactive_anim
                    running: false
                    PropertyAnimation {
                        target: project_context
                        property: "implicitWidth"
                        from: project_context_row.width + 10
                        to: -5
                        duration: 200

                    }
                    onFinished: project_context.visible = false
                }

                visible: false

                Row {
                    id: project_context_row
                    x: 5
                    y: 5
                    spacing: 2

                    AnimatedButton {
                        id: btn_save_project
                        objectName: "btn_save_project"
                        icon.source: "qml_icons/save-icon.svg"
                        icon.width: 10
                        icon.height: 10
                        text: "Save Project"
                        visible: false
                    }

                    AnimatedButton {
                        id: btn_undo
                        objectName: "btn_undo"
                        icon.source: "qml_icons/undo-icon.svg"
                        icon.width: 10
                        icon.height: 10
                        text: "Undo"
                        visible: false
                    }

                    AnimatedButton {
                        id: btn_redo
                        objectName: "btn_redo"
                        icon.source: "qml_icons/redo-icon.svg"
                        icon.width: 10
                        icon.height: 10
                        text: "Redo"
                        visible: false
                    }

                    AnimatedButton {
                        id: btn_info
                        objectName: "btn_info"
                        icon.source: "qml_icons/info-icon.svg"
                        icon.width: 6
                        icon.height: 6
                        text: "Project Info"
                        visible: false
                    }
                }
            }

            Rectangle {
                id: editor_context
                color: dark_mode ? "#0a111f" : "#ffffff"
                implicitWidth: editor_context_row.width + 10
                implicitHeight: 40

                radius: 10
                x: 5
                y: 5

                ParallelAnimation {
                    id: editor_context_active_anim
                    running: false
                    PropertyAnimation {
                        target: editor_context
                        property: "implicitWidth"
                        from: -5
                        to: editor_context_row.width + 10
                        duration: 200

                    }
                    onStarted: editor_context.visible = true
                }

                ParallelAnimation {
                    id: editor_context_inactive_anim
                    running: false
                    PropertyAnimation {
                        target: editor_context
                        property: "implicitWidth"
                        from: editor_context_row.width + 10
                        to: -5
                        duration: 200

                    }
                    onFinished: editor_context.visible = false
                }

                visible: false

                Row {
                    id: editor_context_row
                    x: 5
                    y: 5
                    spacing: 2


                    AnimatedButton {
                        id: btnPrint
                        objectName: "btnPrint"
                        icon.source: "qml_icons/print-icon.svg"
                        icon.width: 10
                        icon.height: 10
                        text: "Print"
                        visible: false
                    }
                }
            }

            Rectangle {
                id: icon_group_final
                color: dark_mode ? "#0a111f" : "#ffffff"
                implicitWidth: toolBar.width - 10 - system_context.width - project_context.width - editor_context.width - 15
                implicitHeight: 40

                radius: 10
                x: 5
                y: 5

                RowLayout {
                    id: rowFinal
                    //width: 453
                    //height: 155
                    spacing: 2
                    anchors.fill: parent
                    layoutDirection: Qt.RightToLeft

//                    AnimatedButtonGitLab {
//                        //x: parent.width - width - 30
//                        y: 10
//                        id: gitlabBtn
//                        icon.source: global_icon
//                        icon.width: 10
//                        icon.height: 10
//                        text: global_fullname
//                        Layout.rightMargin: 5
//                    }
                }
            }
        }




        Connections
        {
            target: gtapp

            function onThemeChanged(dark)
            {
                console.log("QML: Theme changed!");
                dark_mode = dark;
            }

        }
        Connections
        {
            target: mainwin

            function onProjectOpened(open)
            {
                console.log(open);
                if(open)
                {
//                    project_context.visible = true;
                    btn_save_project.active_animation.start();
                    btn_info.active_animation.start();
                    project_context_active_anim.start();
                }
                else
                {
                    btn_save_project.inactive_animation.restart();
                    btn_info.inactive_animation.restart();
                    project_context_inactive_anim.restart();
                    btn_undo.inactive_animation.start()
                    btn_redo.inactive_animation.start()
//                    project_context.visible = false;
                }
            }

            function onCurrentMdiItemPrintable(printable)
            {
                if(printable)
                {
                    if (!btnPrint.visible)
                    {
                        btnPrint.active_animation.start();
                        editor_context_active_anim.start();
                    }
                }
                else
                {
                    btnPrint.inactive_animation.restart();
                    editor_context_inactive_anim.restart();
                }
            }
        }
        Connections
        {
            target: undostack

            function onCanUndoChanged(enabled)
            {
                if (!btn_undo.visible)
                {
                    if(enabled)
                    {
                        btn_undo.custom_Enabled = true
                        btn_undo.active_animation.start()
                    }
                }
                else
                {
                    if(enabled)
                    {
                        btn_undo.custom_Enabled = true
                    }
                    else
                    {
                        btn_undo.custom_Enabled = false
                    }
                }
            }

            function onCanRedoChanged(enabled)
            {
                if (!btn_redo.visible)
                {
                    if(enabled)
                    {
                        btn_redo.custom_Enabled = true
                        btn_redo.active_animation.start()
                    }
                }
                else
                {
                    if(enabled)
                    {
                        btn_redo.custom_Enabled = true
                    }
                    else
                    {
                        btn_redo.custom_Enabled = false
                    }
                }
            }
        }
}


/*##^##
Designer {
    D{i:0;formeditorZoom:0.5}
}
##^##*/
