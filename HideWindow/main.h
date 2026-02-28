// Copyright 2026 Scriptforge
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include <QMainWindow>
#include <QDebug>
#include "GlobalHook.h"
// 自定义主窗口类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        // 初始化全局钩子
        hook = new GlobalHook(this);

        // 连接信号到槽
        connect(hook, &GlobalHook::targetKeyPressed, this, &MainWindow::onKeyPressed);
        connect(hook, &GlobalHook::hookInstallFailed, this, &MainWindow::onHookFailed);

        // 设置监控的全局按键（例如 F1）
        hook->setGlobalHook(Qt::Key_A);

        // 设置窗口标题
        setWindowTitle("Global Hook Example");
    }

    ~MainWindow()
    {
        // 停止钩子监控
        hook->stopGlobalHook();
        delete hook;
    }

private slots:
    void onKeyPressed(Qt::Key key)
    {
        qDebug() << "Detected key press:" << key;
        // 在这里添加你的自定义逻辑
    }

    void onHookFailed(int errorCode)
    {
        qDebug() << "Failed to install hook. Error code:" << errorCode;
        // 处理错误，例如显示错误信息
    }

private:
    GlobalHook* hook;
};