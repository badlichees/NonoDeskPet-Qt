#include "relayclient.h"

#include <QJsonDocument>
#include <QJsonObject>

RelayClient::RelayClient(QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, [this] {
        m_connected = true;
        emit connectedChanged();
        emit logReceived(QStringLiteral("已连接 %1").arg(m_socket.peerName()));
    });
    connect(&m_socket, &QWebSocket::disconnected, this, [this] {
        m_connected = false;
        emit connectedChanged();
        setBusy(false);
        emit logReceived(QStringLiteral("已断开"));
    });
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &RelayClient::onTextMessage);
    connect(&m_socket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit logReceived(QStringLiteral("错误：%1").arg(m_socket.errorString()));
    });
    // connect()内原型：
    // 信号发送者, 信号(函数指针), 接收者/上下文, 槽函数或可调用的对象, (可选)连接类型
    // 提醒自己--Lambda表达式：
    // [ 捕获列表 ] ( 参数列表 ) -> 返回类型 { 函数体 }
    // 捕获列表决定Lambda内部可以访问哪些外部变量、怎么访问；返回类型通常可省略（自动推导）
    // 将接收者指定为this，确保让socket连接跟随RelayClient实例的生命周期，确保对象销毁时连接自动断开同时lambda不悬空
}

void RelayClient::connectToServer(const QUrl &url)
{
    if (m_socket.state() == QAbstractSocket::ConnectedState)
        m_socket.close();
    m_socket.open(url);
}

void RelayClient::disconnectFromServer()
{
    m_socket.close();
}

void RelayClient::sendGoal(const QString &command, double value)
{
    if (!m_connected)
        return;
    QJsonObject obj;
    obj[QStringLiteral("type")] = QStringLiteral("goal");
    obj[QStringLiteral("command")] = command;
    obj[QStringLiteral("value")] = value;
    m_socket.sendTextMessage(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    // QStringLiteral是用来将C++字符串在编译期直接变成QString（Qt自实现字符串）可用的数据的宏
    // 上述操作是在向obj中插入/设置JSON键值对，然后以紧凑格式将JSON最终发送出去，大致链路是：
    // QJsonObject -> QJsonDocument -> QByteArray(JSON字节流) -> QString -> WebSocket
}

void RelayClient::cancel()
{
    if (!m_connected)
        return;
    QJsonObject obj;
    obj[QStringLiteral("type")] = QStringLiteral("cancel");
    m_socket.sendTextMessage(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

namespace {

enum class MessageType { Accepted, Rejected, Feedback, Result, Error, Unknown };

MessageType messageTypeFromString(const QString &type)
{
    if (type == QLatin1String("accepted"))
        return MessageType::Accepted;
    if (type == QLatin1String("rejected"))
        return MessageType::Rejected;
    if (type == QLatin1String("feedback"))
        return MessageType::Feedback;
    if (type == QLatin1String("result"))
        return MessageType::Result;
    if (type == QLatin1String("error"))
        return MessageType::Error;
    return MessageType::Unknown;
}

// 匿名命名空间，这里面定义的所有东西只在当前.cpp文件里可见
} // namespace

void RelayClient::onTextMessage(const QString &message)
{
    const QJsonObject obj = QJsonDocument::fromJson(message.toUtf8()).object();

    switch (messageTypeFromString(obj[QStringLiteral("type")].toString())) {
    case MessageType::Accepted:
        setBusy(true);
        setProgress(0.0);
        emit logReceived(QStringLiteral("目标已接受"));
        break;
    case MessageType::Rejected:
        emit logReceived(
            QStringLiteral("目标被拒绝：%1").arg(obj[QStringLiteral("message")].toString()));
        break;
    case MessageType::Feedback:
        setProgress(obj[QStringLiteral("progress")].toDouble());
        break;
    case MessageType::Result: {
        setBusy(false);
        const bool success = obj[QStringLiteral("success")].toBool();
        const QString resultMessage = obj[QStringLiteral("message")].toString();
        emit logReceived(QStringLiteral("结果：%1").arg(resultMessage));
        emit goalFinished(success, resultMessage);
        break;
    }
    case MessageType::Error:
        emit logReceived(
            QStringLiteral("中继错误：%1").arg(obj[QStringLiteral("message")].toString()));
        break;
    case MessageType::Unknown:
        emit logReceived(QStringLiteral("未知消息：%1").arg(message));
        break;
    }
}

void RelayClient::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

void RelayClient::setProgress(qreal progress)
{
    if (qFuzzyCompare(m_progress, progress))
        return;
    m_progress = progress;
    emit progressChanged();
}
