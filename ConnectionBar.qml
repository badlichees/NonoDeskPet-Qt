import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NonoDeskPet

// 连接栏：地址输入 + 连接/断开按钮 + 状态文字
RowLayout {
    id: root
    spacing: 8
    required property RelayClient relay // 必须由外部传入relay属性

    TextField {
        id: urlField
        Layout.fillWidth: true
        text: "ws://localhost:8765"
        placeholderText: qsTr("中继地址")
    }

    Button {
        text: root.relay.connected ? qsTr("断开") : qsTr("连接")
        onClicked: root.relay.connected ? root.relay.disconnectFromServer() :
                                          root.relay.connectToServer(urlField.text)
    }

    Label {
        text: root.relay.connected ? qsTr("已连接") : qsTr("未连接")
    }
}
