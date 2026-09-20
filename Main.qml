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

    BrainClient {
        id: brain
    }

    // 可内部嵌套式地排列也可同级排列，分水平和垂直，在顶端定义排列的规范
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // 连接栏：地址输入 + 连接按钮 + 状态（独立组件）
        ConnectionBar {
            Layout.fillWidth: true
            // 将Main里的id为relay的RelayClient对象注入给ConnectionBar的relay属性
            relay: relay
        }

        // 控制面板：参数输入 + 方向盘 + 进度条（独立组件）
        ControlPad {
            Layout.fillWidth: true
            relay: relay
        }

        // 聊天面板：大脑连接 + 对话记录 + 输入框（独立组件）
        ChatPanel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            brain: brain
        }

        // 日志面板：消息记录列表（独立组件）
        LogPanel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            relay: relay
        }
    }
}
