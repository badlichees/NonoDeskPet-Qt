import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import NonoDeskPet

// 控制面板：参数输入 + 方向盘 + 进度条
ColumnLayout {
    id: root
    required property RelayClient relay

    // 界面上用厘米和角度（整数好输入），发送时换算成米和弧度
    function sendMove(command) {
        relay.sendGoal(command, distanceBox.value / 100.0);
    }
    function sendTurn(command) {
        relay.sendGoal(command, angleBox.value * Math.PI / 180.0);
    }

    // 参数区
    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: qsTr("距离(cm)")
        }
        SpinBox {
            id: distanceBox

            from: 5
            to: 100
            value: 20
            stepSize: 5
            editable: true
        }
        Label {
            text: qsTr("角度(°)")
        }
        SpinBox {
            id: angleBox

            from: 15
            to: 180
            value: 90
            stepSize: 15
            editable: true
        }
    }

    // 方向按键：十字布局，四个角是占位，中央是STOP
    GridLayout {
        Layout.alignment: Qt.AlignHCenter
        columns: 3
        rowSpacing: 8
        columnSpacing: 8

        Item {
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
        }
        Button {
            text: qsTr("前")
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            enabled: root.relay.connected && !root.relay.busy
            onClicked: root.sendMove("move_forward")
        }
        Item {
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
        }

        Button {
            text: qsTr("左")
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            enabled: root.relay.connected && !root.relay.busy
            onClicked: root.sendTurn("turn_left")
        }
        Button {
            id: stopButton

            text: qsTr("停")
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            enabled: root.relay.busy
            onClicked: root.relay.cancel()
            background: Rectangle {
                color: stopButton.enabled ? (stopButton.pressed ? "#c62828" : "#e53935") : "#bdbdbd"
                radius: 4
            }
        }
        Button {
            text: qsTr("右")
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            enabled: root.relay.connected && !root.relay.busy
            onClicked: root.sendTurn("turn_right")
        }

        Item {
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
        }
        Button {
            text: qsTr("后")
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
            enabled: root.relay.connected && !root.relay.busy
            onClicked: root.sendMove("move_backward")
        }
        Item {
            Layout.preferredWidth: 72
            Layout.preferredHeight: 72
        }
    }

    // 动作进度，执行中才显示
    ProgressBar {
        Layout.fillWidth: true
        visible: root.relay.busy
        value: root.relay.progress
    }
}
