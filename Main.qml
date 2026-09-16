import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import NonoDeskPet

ApplicationWindow {
    id: root

    width: 480
    height: 640
    visible: true
    title: qsTr("NonoDeskPet-控制台")

    // 实例化C++中的对象
    RelayClient {
        id: relay
    }

    // 监听指定对象的信号并处理
    Connections {
        target: relay

        function onLogReceived(line) {
            logModel.append({
                                "line": line
                            });
            logView.positionViewAtEnd();
        }
    }

    // 可内部嵌套式地排列也可同级排列，分水平和垂直，在顶端定义排列的规范
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // 连接栏：地址输入 + 连接按钮 + 状态（此处作为独立组件）
        ConnectionBar {
            Layout.fillWidth: true
            // 将Main里的id为relay的RelayClient对象注入给ConnectionBar的relay属性
            relay: relay
        }

        // ListView负责显示日志
        ListView {
            id: logView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ListModel {
                id: logModel
            }
            delegate: Label {
                required property string line

                text: line
            }
        }
    }
}
