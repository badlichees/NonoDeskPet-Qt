#ifndef RELAYCLIENT_H
#define RELAYCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QtQmlIntegration/qqmlintegration.h>

// 主动向relay服务器发起WebSocket连接、发送请求、接受服务端推送
class RelayClient : public QObject
{
    Q_OBJECT // 启用Qt的元对象系统，所有需要信号槽的类都需要这个宏
    QML_ELEMENT // 注册该类为QML类型

    // 读取设定好的属性时，调用C++对应的成员函数
    // 格式：数据类型 属性名 关键字 成员函数 关键字 信号
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public:
    // 构造函数，explicit防止隐式类型转换
    explicit RelayClient(QObject *parent = nullptr);

    bool connected() const { return m_connected; }
    bool busy() const { return m_busy; }
    qreal progress() const { return m_progress; } // qreal是Qt版的double，跨平台时可统一浮点精度

    Q_INVOKABLE void connectToServer(const QUrl &url);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendGoal(const QString &command, double value);
    Q_INVOKABLE void cancel();
    // Q_INVOKABLE标记代表这些方法可以从QML中直接调用
    // 格式：Q_INVOKABLE 数据类型 成员函数

signals:
    void connectedChanged();
    void busyChanged();
    void progressChanged();
    void logReceived(const QString &line);
    void goalFinished(bool success, const QString &message);

private:
    void onTextMessage(const QString &message);
    void setBusy(bool busy);
    void setProgress(qreal progress);

    QWebSocket m_socket;
    bool m_connected = false;
    bool m_busy = false;
    qreal m_progress = 0.0;
};

#endif // RELAYCLIENT_H
