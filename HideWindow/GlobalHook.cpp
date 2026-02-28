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
#include "GlobalHook.h"

// ========== 关键修复：静态成员类外初始化（兼容所有C++版本） ==========
GlobalHook* GlobalHook::s_instance = nullptr;

// 钩子回调函数实现
LRESULT CALLBACK GlobalHook::lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // nCode >= 0 才处理，且实例必须有效
    if (nCode >= 0 && GlobalHook::s_instance != nullptr) {
        GlobalHookPrivate* d = GlobalHook::s_instance->d_ptr;
        if (!d || !d->isHookActive || d->targetKey == Qt::Key_unknown) {
            return CallNextHookEx(d->hookHandle, nCode, wParam, lParam);
        }

        // 只处理按键按下事件
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            // Win32虚拟键码转Qt::Key（数值兼容）
            Qt::Key qtKey = static_cast<Qt::Key>(pKeyStruct->vkCode);

            // 检测到目标按键，发射信号
            if (qtKey == d->targetKey) {
                emit GlobalHook::s_instance->targetKeyPressed(qtKey);
                // 如需拦截按键，取消下面注释（慎用，会阻止系统接收该按键）
                // return 1;
            }
        }
    }

    // 必须传递给下一个钩子，否则系统可能异常
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// 构造函数
GlobalHook::GlobalHook(QObject* parent)
    : QObject(parent)  // 显式初始化QObject基类
    , d_ptr(new GlobalHookPrivate) {
    // 赋值静态实例指针
    GlobalHook::s_instance = this;
}

// 析构函数
GlobalHook::~GlobalHook() {
    // 析构时自动停止钩子，释放资源
    stopGlobalHook();
    delete d_ptr;
    GlobalHook::s_instance = nullptr;
}

// 设置全局钩子（监控指定按键）
void GlobalHook::setGlobalHook(Qt::Key key) {
    GlobalHookPrivate* d = this->d_ptr;
    QMutexLocker locker(&d->mutex); // 线程安全锁

    // 先停止已有激活的钩子
    if (d->isHookActive) {
        stopGlobalHook();
    }

    // 设置目标按键
    d->targetKey = key;

    // 安装低级键盘全局钩子
    d->hookHandle = SetWindowsHookExW(
        WH_KEYBOARD_LL,                // 钩子类型：低级键盘钩子
        lowLevelKeyboardProc,          // 回调函数
        GetModuleHandleW(nullptr),     // 当前模块句柄（LL钩子无需DLL）
        0                              // 0 = 全局监控所有线程
    );

    // 检查钩子安装是否成功
    if (d->hookHandle == nullptr) {
        int errorCode = GetLastError();
        // 修复：避免重载解析错误，拆分输出内容
        qCritical() << "全局钩子安装失败，错误码：" << errorCode;
        emit hookInstallFailed(errorCode);
        d->isHookActive = false;
        return;
    }

    d->isHookActive = true;
    // 修复：拆分QKeySequence输出，避免重载冲突
    QString keyText = QKeySequence(key).toString();
    qInfo() << "全局钩子已启动，监控按键：" << keyText;
}

// 停止全局钩子
void GlobalHook::stopGlobalHook() {
    GlobalHookPrivate* d = this->d_ptr;
    QMutexLocker locker(&d->mutex);

    if (d->isHookActive && d->hookHandle != nullptr) {
        // 卸载钩子
        UnhookWindowsHookEx(d->hookHandle);
        d->hookHandle = nullptr;
        d->isHookActive = false;
        d->targetKey = Qt::Key_unknown;
        qInfo() << "全局钩子已停止";
    }
}