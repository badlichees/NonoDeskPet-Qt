# NonoDeskPet-Qt

NonoDeskPet 桌面宠物机器人的安卓 / 桌面控制端 App（Qt 6 + QML）。

主项目（机器人本体、大脑、仿真）：[nonodeskpet](https://github.com/badlichees/nonodeskpet)

## 功能

- **聊天面板**：连大脑服务（brain.py，端口 8767），打字聊天，显示回复、表情名和动作执行进展
- **控制面板**：连中继节点（relay_server，端口 8765），手动下发前进/后退/转向指令，带进度条和取消
- **日志面板**：中继链路的消息记录

两条 WebSocket 连接互相独立，可以只连其中一个。

## 构建

Qt 6.8+，CMake。桌面端直接构建运行；安卓端在Qt Creator里选择Android kit部署。

```bash
cmake -B build -DCMAKE_PREFIX_PATH=<Qt安装路径>/6.x/gcc_64
cmake --build build
```

## 协议

App 是 WebSocket 客户端，对端协议见主项目 README。聊天走 brain 服务（chat/reply），
控制走 relay 中继（goal/cancel/feedback/result）。

## License

Apache-2.0
