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

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: urlField

                Layout.fillWidth: true
                text: "ws://localhost:8765"
                placeholderText: qsTr("中继地址")
            }

            Button {
                text: relay.connected ? qsTr("断开") : qsTr("连接")
                onClicked: relay.connected ? relay.disconnectFromServer() : relay.connectToServer(
                                                 urlField.text)
            }
        }

        Label {
            text: relay.connected ? qsTr("状态：已连接") : qsTr("状态：未连接")
        }

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
