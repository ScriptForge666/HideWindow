// Copyright 2026 Scriptforge
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Window {
    
    // 统一的按钮样式定义
    property color windowBgColor: "black"
    property color buttonNormalColor: "green"
    property color buttonHoverColor: "lightgreen"
    property color buttonPressedColor: "darkgreen"
    property color textColor: "white"
    
    id: window
    visible: true
    width: 600
    height: 400
    title: "Windows 进程隐藏工具"
    color: windowBgColor
    
    property var selectedProcess: null
    
    
    
    Button {
        id: refreshBtn
        background: Rectangle {
            color: parent.pressed ? buttonPressedColor :
                   parent.hovered ? buttonHoverColor : buttonNormalColor
            radius: 10
        }
        contentItem: Text {
            text: parent.text
            color: textColor
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        text: "刷新进程列表(R)"
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_R) {
                clicked()
                event.accepted = true
            }
        }
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        onClicked: {
            console.log("刷新按钮被点击")
            processModel.enumerateWindowsProcesses()
        }
    }
    
    Button {
        id: hideBtn
        background: Rectangle {
            color: parent.pressed ? buttonPressedColor :
                   parent.hovered ? buttonHoverColor : buttonNormalColor
            radius: 10
        }
        contentItem: Text {
            text: parent.text
            color: textColor
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        text: "隐藏该进程(H)"
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_H) {
                clicked()
                event.accepted = true
            }
        }
        anchors.top: parent.top
        anchors.left: refreshBtn.right
        anchors.margins: 10
        onClicked: {
            console.log("隐藏按钮被点击")
            if (selectedProcess === null) {
                unselectedProcess.visible = true
                console.log("请先选择一个进程")
            } else {
                console.log("隐藏进程：", selectedProcess.name, "PID：", selectedProcess.pid)
                // 关键修改：传递 PID 而非整个模型对象
                hideProcess.hideProcess(selectedProcess.pid)
            }
        }
    }
Button {
        id: showBtn
        background: Rectangle {
            color: parent.pressed ? buttonPressedColor :
                   parent.hovered ? buttonHoverColor : buttonNormalColor
            radius: 10
        }
        contentItem: Text {
            text: parent.text
            color: textColor
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        text: "显示该进程(S)"
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_S) {
                clicked()
                event.accepted = true
            }
        }
        anchors.top: parent.top
        anchors.left: hideBtn.right
        anchors.margins: 10
        onClicked: {
            console.log("显示按钮被点击")
            if (selectedProcess === null) {
                unselectedProcess.visible = true
                console.log("请先选择一个进程")
            } else {
                console.log("显示进程：", selectedProcess.name, "PID：", selectedProcess.pid)
                // 关键修改：传递 PID 而非整个模型对象
                hideProcess.showProcess(selectedProcess.pid)
            }
        }
    }
    
    Button {
        id: openPopupBtn
        background: Rectangle {
            color: parent.pressed ? buttonPressedColor :
                   parent.hovered ? buttonHoverColor : buttonNormalColor
            radius: 10
        }
        contentItem: Text {
            text: parent.text
            color: textColor
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        text: "设置隐藏开关(S)"
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_P) {
                clicked()
                event.accepted = true
            }
        }
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        onClicked: {
        if (selectedProcess === null) {
                unselectedProcess.visible = true
                console.log("请先选择一个进程")
            } else {
                console.log("设置隐藏开关按钮被点击")
                setHideWindow.visible = true;
            }
        }
    }  
    
    ListView {
        id: processListView
        anchors.top: refreshBtn.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: copyrightText.top
        anchors.margins: 10
        clip: true

        model: processModel


        delegate: Rectangle {
            width: ListView.view.width
            height: 40
            color: index % 2 === 0 ? "#1a1a1a" : "#2a2a2a"
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (selectedProcess === model) {
                        console.log("取消选中进程：", name, "PID：", pid)
                        selectedProcess = null
                    }else{
                        console.log("选中进程：", name, "PID：", pid)
                        selectedProcess = model
                    }
                }
                
                Text {
                    id: processText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    text: name + " (PID: " + pid + ")"
                    color: selectedProcess === model ? buttonNormalColor : textColor
                    font.pixelSize: 14
                }
                
                ToolTip.text: file
                ToolTip.visible: containsMouse
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
    }

    Text {
        id: copyrightText
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 10
        }
        color: textColor
        text: "Copyright © 2026 Scriptforge "
    }
    
    Window {
        id: unselectedProcess
        visible: false
        width: 300
        height: 150
        title: "提示"
        color: windowBgColor
        modality: Qt.ApplicationModal
        
        
        
        Text {
            color: textColor
            text: "请先选择一个进程"
            font.pixelSize: 30
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
        }
    }


    Window {
        id: setHideWindow
        visible: false  // 初始不可见
        width: 300
        height: 200
        title: "弹出的新窗口"
        color: windowBgColor
        modality: Qt.ApplicationModal
        
        // 新窗口内容
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                color: textColor
                text: selectedProcess ? "当前选中进程: " + selectedProcess.name : "未选中任何进程"
                font.pixelSize: 18
                anchors.horizontalCenter: parent.horizontalCenter
            }
            
            Button {
                text: "关闭窗口"
                background: Rectangle {
                    color: parent.pressed ? buttonPressedColor :
                    parent.hovered ? buttonHoverColor : buttonNormalColor
                    radius: 10
                }
                contentItem: Text {
                    text: parent.text
                    color: textColor
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    setHideWindow.visible = false;
                }
            }
        }
    }
}