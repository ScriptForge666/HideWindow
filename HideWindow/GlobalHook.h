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
#include <windows.h>

// 前置声明私有实现类
class GlobalHookPrivate;

class GlobalHook : public QObject
{
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
class GlobalHookPrivate
{
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

#endif