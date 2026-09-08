#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    // 创建QGuiApplication实例用于处理应用程序级别的资源和设置
    QGuiApplication app(argc, argv);

    // 创建QML应用程序引擎实例负责加载解析QML文件并管理其中的对象树
    QQmlApplicationEngine engine;

    // 从指定的QML模块中加载QML组件
    engine.loadFromModule("NonoDeskPet", "Main");

    // 启动应用程序的事件循环，与用户交互
    return app.exec();
}
