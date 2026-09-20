import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NonoDeskPet

// 聊天面板：连接大脑服务 + 对话记录 + 输入框
ColumnLayout {
    id: root
    spacing: 8
    required property BrainClient brain // 必须由外部传入brain属性

    // 连接行：大脑服务地址 + 连接/断开按钮 + 状态文字 + 当前表情
    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        TextField {
            id: urlField
            Layout.fillWidth: true
            text: "ws://localhost:8767"
            placeholderText: qsTr("大脑地址")
        }

        Button {
            text: root.brain.connected ? qsTr("断开") : qsTr("连接")
            onClicked: root.brain.connected ? root.brain.disconnectFromServer() :
                                              root.brain.connectToServer(urlField.text)
        }

        Label {
            text: root.brain.connected ? qsTr("已连接") : qsTr("未连接")
        }

        Label {
            text: qsTr("表情: %1").arg(root.brain.expression)
        }
    }

    // 对话记录：用户发送的内容和桌宠回复的内容按时间排在一起
    ListView {
        id: chatView
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true

        model: ListModel {
            id: chatModel
        }

        delegate: Label {
            required property string line
            text: line
            width: chatView.width // 定宽才能触发自动换行
            wrapMode: Text.WordWrap
        }
    }

    // 输入行：输入框 + 发送按钮，没连上大脑时禁用
    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        TextField {
            id: inputField
            Layout.fillWidth: true
            placeholderText: qsTr("说点什么…")
            enabled: root.brain.connected
            onAccepted: sendButton.clicked() // 回车等于点发送
        }

        Button {
            id: sendButton
            text: qsTr("发送")
            enabled: root.brain.connected && inputField.text.trim().length > 0
            onClicked: {
                chatModel.append({
                                     "line": qsTr("你: %1").arg(inputField.text)
                                 });
                chatView.positionViewAtEnd();
                root.brain.sendChat(inputField.text);
                inputField.clear();
            }
        }
    }

    // 监听brain的信号，回复和过程消息都追加到对话记录里，新消息自动滚动到底部
    Connections {
        target: root.brain

        function onReplyReceived(text) {
            chatModel.append({
                                 "line": qsTr("桌宠: %1").arg(text)
                             });
            chatView.positionViewAtEnd();
        }

        function onLogReceived(line) {
            chatModel.append({
                                 "line": qsTr("[%1]").arg(line)
                             });
            chatView.positionViewAtEnd();
        }
    }
}
