#ifndef BRAINCLIENT_H
#define BRAINCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QtQmlIntegration/qqmlintegration.h>

// 连接brain.py的客户端，会发送聊天内容，然后接收回复/表情/动作执行进展
class BrainClient : public QObject
{
    Q_OBJECT // 启用Qt的元对象系统，所有需要信号槽的类都需要这个宏
    QML_ELEMENT // 注册该类为QML类型

    // 将C++类中的成员声明为Qt属性，让Qt的元对象系统认识它
    // 读取设定好的属性时，调用C++对应的成员函数
    // 格式：数据类型 属性名 关键字 成员函数 关键字 信号
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString expression READ expression NOTIFY expressionChanged)

public:
    // 构造函数，explicit防止隐式类型转换
    explicit BrainClient(QObject *parent = nullptr);

    bool connected() const { return m_connected; }
    QString expression() const { return m_expression; }

    Q_INVOKABLE void connectToServer(const QUrl &url);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendChat(const QString &text);
    // Q_INVOKABLE标记代表这些方法可以从QML中直接调用

signals:
    void connectedChanged();
    void expressionChanged();
    void replyReceived(const QString &text); // 桌宠回了一句话，用于对话记录显示
    void logReceived(const QString &line); // 过程消息（动作、受理、结果、错误）

private:
    void onTextMessage(const QString &message);
    void setExpression(const QString &expression);

    QWebSocket m_socket;
    bool m_connected = false;
    QString m_expression = QStringLiteral("neutral"); // 桌宠当前表情名，和brain.py里的枚举一致
};

#endif // BRAINCLIENT_H
