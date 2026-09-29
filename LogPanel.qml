import QtQuick
import QtQuick.Controls
import NonoDeskPet

// 日志面板：显示中继来回的消息记录
ListView {
    id: root
    required property RelayClient relay
    clip: true

    model: ListModel {
        id: logModel
    }

    delegate: Label {
        required property string line
        text: line
    }

    // 监听relay的日志信号，收到一条就往列表追加一条，新消息自动滚动到底部
    Connections {
        target: root.relay

        function onLogReceived(line) {
            logModel.append({
                                "line": line
                            });
            root.positionViewAtEnd();
        }
    }
}
