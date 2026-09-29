#include "brainclient.h"

#include <QJsonDocument>
#include <QJsonObject>

BrainClient::BrainClient(QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, [this] {
        m_connected = true;
        emit connectedChanged();
        emit logReceived(QStringLiteral("已连接大脑服务 %1").arg(m_socket.peerName()));
    });
    connect(&m_socket, &QWebSocket::disconnected, this, [this] {
        m_connected = false;
        emit connectedChanged();
        emit logReceived(QStringLiteral("大脑服务已断开"));
    });
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &BrainClient::onTextMessage);
    connect(&m_socket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit logReceived(QStringLiteral("错误：%1").arg(m_socket.errorString()));
    });
    // connect()用于建立信号槽连接，其内部原型：
    // 信号发送者, 信号(函数指针), 接收者/上下文, 槽函数或可调用的对象, (可选)连接类型
    // 将接收者指定为this，确保连接跟随BrainClient实例的生命周期，对象销毁时连接自动断开同时lambda不悬空
}

void BrainClient::connectToServer(const QUrl &url)
{
    if (m_socket.state() == QAbstractSocket::ConnectedState)
        m_socket.close();
    m_socket.open(url);
}

void BrainClient::disconnectFromServer()
{
    m_socket.close();
}

void BrainClient::sendChat(const QString &text)
{
    if (!m_connected)
        return;
    QJsonObject obj;
    obj[QStringLiteral("type")] = QStringLiteral("chat");
    obj[QStringLiteral("text")] = text;
    m_socket.sendTextMessage(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    // 上述操作是在向obj中插入/设置JSON键值对，然后以紧凑格式将JSON最终发送出去，大致链路是：
    // QJsonObject -> QJsonDocument -> QByteArray(JSON字节流) -> QString -> WebSocket
}

namespace {

enum class MessageType { Reply, Action, Accepted, Rejected, Result, Error, Unknown };

MessageType messageTypeFromString(const QString &type)
{
    if (type == QLatin1String("reply"))
        return MessageType::Reply;
    if (type == QLatin1String("action"))
        return MessageType::Action;
    if (type == QLatin1String("accepted"))
        return MessageType::Accepted;
    if (type == QLatin1String("rejected"))
        return MessageType::Rejected;
    if (type == QLatin1String("result"))
        return MessageType::Result;
    if (type == QLatin1String("error"))
        return MessageType::Error;
    return MessageType::Unknown;
}

// 匿名命名空间，这里面定义的所有东西只在当前.cpp文件里可见
} // namespace

void BrainClient::onTextMessage(const QString &message)
{
    const QJsonObject obj = QJsonDocument::fromJson(message.toUtf8()).object();

    switch (messageTypeFromString(obj[QStringLiteral("type")].toString())) {
    case MessageType::Reply:
        // 桌宠的回复（文字走replyReceived给对话记录，表情存到属性里给界面显示）
        setExpression(obj[QStringLiteral("expression")].toString());
        emit replyReceived(obj[QStringLiteral("text")].toString());
        break;
    case MessageType::Action:
        emit logReceived(QStringLiteral("动作：%1 %2")
                             .arg(obj[QStringLiteral("command")].toString())
                             .arg(obj[QStringLiteral("value")].toDouble()));
        break;
    case MessageType::Accepted:
        emit logReceived(QStringLiteral("动作已被执行层接受"));
        break;
    case MessageType::Rejected:
        emit logReceived(
            QStringLiteral("动作被拒绝：%1").arg(obj[QStringLiteral("message")].toString()));
        break;
    case MessageType::Result: {
        const bool success = obj[QStringLiteral("success")].toBool();
        emit logReceived(QStringLiteral("动作结果：%1 %2")
                             .arg(success ? QStringLiteral("成功") : QStringLiteral("失败"),
                                  obj[QStringLiteral("message")].toString()));
        break;
    }
    case MessageType::Error:
        emit logReceived(
            QStringLiteral("大脑错误：%1").arg(obj[QStringLiteral("message")].toString()));
        break;
    case MessageType::Unknown:
        emit logReceived(QStringLiteral("未知消息：%1").arg(message));
        break;
    }
}

void BrainClient::setExpression(const QString &expression)
{
    if (m_expression == expression)
        return;
    m_expression = expression;
    emit expressionChanged();
}
