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
#ifndef GLOBALHOOK_H
#define GLOBALHOOK_H
#include <QObject>
#include <QKeySequence>
#include <QMutex>
#include <QDebug>
#include <QMap>
#include <windows.h>

// 前置声明私有实现类
class GlobalHookPrivate;

class GlobalHook : public QObject {
    Q_OBJECT

public:
    // 显式构造函数，正确初始化QObject基类
    explicit GlobalHook(QObject* parent = nullptr);
    // 禁用拷贝和赋值
    GlobalHook(const GlobalHook&) = delete;
    GlobalHook& operator=(const GlobalHook&) = delete;
    // 析构函数
    ~GlobalHook() override;

public slots:
    // 设置要监控的全局按键（启动钩子）
    void setGlobalHook(Qt::Key key);
    // 停止全局钩子监控
    void stopGlobalHook();

signals:
    // 检测到指定按键按下时触发
    void targetKeyPressed(Qt::Key key);
    // 钩子安装失败信号
    void hookInstallFailed(int errorCode);

private:
    // PIMPL私有实现指针
    GlobalHookPrivate* d_ptr;
    // Qt PIMPL宏（必须放在私有成员最后）
    Q_DECLARE_PRIVATE(GlobalHook)

    // 静态钩子回调函数（Win32要求静态）
    static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    // 静态实例指针（C++11/14兼容：类内声明，类外初始化）
    static GlobalHook* s_instance;
};

// 私有实现类定义
class GlobalHookPrivate {
public:
    GlobalHookPrivate()
        : hookHandle(nullptr)
        , targetKey(Qt::Key_unknown)
        , isHookActive(false)
    {
    }

    HHOOK hookHandle;          // 钩子句柄
    Qt::Key targetKey;         // 监控的目标按键
    bool isHookActive;         // 钩子激活状态
    QMutex mutex;              // 线程安全锁
};

static const QMap<DWORD, Qt::Key> vkToQtKey {
    { VK_F1,Qt::Key_F1 },
    { VK_F2,Qt::Key_F2 },
    { VK_F3,Qt::Key_F3 },
    { VK_F4,Qt::Key_F4 },
    { VK_F5,Qt::Key_F5 },
    { VK_F6,Qt::Key_F6 },
    { VK_F7,Qt::Key_F7 },
    { VK_F8,Qt::Key_F8 },
    { VK_F9,Qt::Key_F9 },
    { VK_F10,Qt::Key_F10 },
    { VK_F11,Qt::Key_F11 },
    { VK_F12,Qt::Key_F12 },
    { VK_F13,Qt::Key_F13 },
    { VK_F14,Qt::Key_F14 },
    { VK_F15,Qt::Key_F15 },
    { VK_F16,Qt::Key_F16 },
    { VK_F17,Qt::Key_F17 },
    { VK_F18,Qt::Key_F18 },
    { VK_F19,Qt::Key_F19 },
    { VK_F20,Qt::Key_F20 },
    { VK_F21,Qt::Key_F21 },
    { VK_F22,Qt::Key_F22 },
    { VK_F23,Qt::Key_F23 },
    { VK_F24,Qt::Key_F24 },
    { NULL, Qt::Key_unknown }
};
#endif
