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

#include <QMap>
#include <QtGlobal>
#include <windows.h> // 必须包含，否则识别不了VK_*宏

static const QMap<DWORD, Qt::Key> vkToQtKey{
    // 鼠标按键（标记为unknown，Qt不处理）
    { VK_LBUTTON, Qt::Key_unknown },
    { VK_RBUTTON, Qt::Key_unknown },
    { VK_MBUTTON, Qt::Key_unknown },
    { VK_XBUTTON1, Qt::Key_unknown },
    { VK_XBUTTON2, Qt::Key_unknown },

    // 基础功能键
    { VK_CANCEL, Qt::Key_Cancel },
    { VK_BACK, Qt::Key_Backspace },
    { VK_TAB, Qt::Key_Tab },
    { VK_CLEAR, Qt::Key_Clear },
    { VK_RETURN, Qt::Key_Return },
    { VK_SHIFT, Qt::Key_Shift },
    { VK_CONTROL, Qt::Key_Control },
    { VK_MENU, Qt::Key_Alt },
    { VK_PAUSE, Qt::Key_Pause },
    { VK_CAPITAL, Qt::Key_CapsLock },
    { VK_ESCAPE, Qt::Key_Escape },
    { VK_SPACE, Qt::Key_Space },
    { VK_PRIOR, Qt::Key_PageUp },
    { VK_NEXT, Qt::Key_PageDown },
    { VK_END, Qt::Key_End },
    { VK_HOME, Qt::Key_Home },
    { VK_LEFT, Qt::Key_Left },
    { VK_UP, Qt::Key_Up },
    { VK_RIGHT, Qt::Key_Right },
    { VK_DOWN, Qt::Key_Down },
    { VK_SELECT, Qt::Key_Select },
    { VK_PRINT, Qt::Key_Printer },
    { VK_EXECUTE, Qt::Key_Execute },
    { VK_SNAPSHOT, Qt::Key_Print },
    { VK_INSERT, Qt::Key_Insert },
    { VK_DELETE, Qt::Key_Delete },
    { VK_HELP, Qt::Key_Help },

    // Windows键/应用键
    { VK_LWIN, Qt::Key_Meta },
    { VK_RWIN, Qt::Key_Meta },
    { VK_APPS, Qt::Key_Menu },
    { VK_SLEEP, Qt::Key_Sleep },

    // 小键盘
    { VK_NUMPAD0, Qt::Key_0 },
    { VK_NUMPAD1, Qt::Key_1 },
    { VK_NUMPAD2, Qt::Key_2 },
    { VK_NUMPAD3, Qt::Key_3 },
    { VK_NUMPAD4, Qt::Key_4 },
    { VK_NUMPAD5, Qt::Key_5 },
    { VK_NUMPAD6, Qt::Key_6 },
    { VK_NUMPAD7, Qt::Key_7 },
    { VK_NUMPAD8, Qt::Key_8 },
    { VK_NUMPAD9, Qt::Key_9 },
    { VK_MULTIPLY, Qt::Key_Asterisk },
    { VK_ADD, Qt::Key_Plus },
    { VK_SEPARATOR, Qt::Key_unknown },
    { VK_SUBTRACT, Qt::Key_Minus },
    { VK_DECIMAL, Qt::Key_unknown },
    { VK_DIVIDE, Qt::Key_Slash },

    // 功能键F1-F24
    { VK_F1, Qt::Key_F1 },
    { VK_F2, Qt::Key_F2 },
    { VK_F3, Qt::Key_F3 },
    { VK_F4, Qt::Key_F4 },
    { VK_F5, Qt::Key_F5 },
    { VK_F6, Qt::Key_F6 },
    { VK_F7, Qt::Key_F7 },
    { VK_F8, Qt::Key_F8 },
    { VK_F9, Qt::Key_F9 },
    { VK_F10, Qt::Key_F10 },
    { VK_F11, Qt::Key_F11 },
    { VK_F12, Qt::Key_F12 },
    { VK_F13, Qt::Key_F13 },
    { VK_F14, Qt::Key_F14 },
    { VK_F15, Qt::Key_F15 },
    { VK_F16, Qt::Key_F16 },
    { VK_F17, Qt::Key_F17 },
    { VK_F18, Qt::Key_F18 },
    { VK_F19, Qt::Key_F19 },
    { VK_F20, Qt::Key_F20 },
    { VK_F21, Qt::Key_F21 },
    { VK_F22, Qt::Key_F22 },
    { VK_F23, Qt::Key_F23 },
    { VK_F24, Qt::Key_F24 },

    // 数字锁定/滚动锁定
    { VK_NUMLOCK, Qt::Key_NumLock },
    { VK_SCROLL, Qt::Key_ScrollLock },

    // 日文/特殊键盘
    { VK_OEM_FJ_MASSHOU, Qt::Key_Massyo },
    { VK_OEM_FJ_TOUROKU, Qt::Key_Touroku },

    // 左右修饰键（单独映射，和主键值一致）
    { VK_LSHIFT, Qt::Key_Shift },
    { VK_RSHIFT, Qt::Key_Shift },
    { VK_LCONTROL, Qt::Key_Control },
    { VK_RCONTROL, Qt::Key_Control },
    { VK_LMENU, Qt::Key_Alt },
    { VK_RMENU, Qt::Key_Alt },

    // 浏览器/多媒体键
    { VK_BROWSER_BACK, Qt::Key_Back },
    { VK_BROWSER_FORWARD, Qt::Key_Forward },
    { VK_BROWSER_REFRESH, Qt::Key_Refresh },
    { VK_BROWSER_STOP, Qt::Key_Stop },
    { VK_BROWSER_SEARCH, Qt::Key_Search },
    { VK_BROWSER_FAVORITES, Qt::Key_Favorites },
    { VK_BROWSER_HOME, Qt::Key_HomePage },
    { VK_VOLUME_MUTE, Qt::Key_VolumeMute },
    { VK_VOLUME_DOWN, Qt::Key_VolumeDown },
    { VK_VOLUME_UP, Qt::Key_VolumeUp },
    { VK_MEDIA_NEXT_TRACK, Qt::Key_MediaNext },
    { VK_MEDIA_PREV_TRACK, Qt::Key_MediaPrevious },
    { VK_MEDIA_STOP, Qt::Key_MediaStop },
    { VK_MEDIA_PLAY_PAUSE, Qt::Key_MediaTogglePlayPause },
    { VK_LAUNCH_MAIL, Qt::Key_LaunchMail },
    { VK_LAUNCH_MEDIA_SELECT, Qt::Key_LaunchMedia },
    { VK_LAUNCH_APP1, Qt::Key_Launch0 },
    { VK_LAUNCH_APP2, Qt::Key_Launch1 },

    // 其他多媒体键
    { VK_PLAY, Qt::Key_Play },
    { VK_ZOOM, Qt::Key_Zoom },

    // 兜底（空值）
    { NULL, Qt::Key_unknown }
};

// 补充：字母/数字键的映射（因为原表中是0，这里手动补全，全局钩子必备）
static const QMap<DWORD, Qt::Key> charVkToQtKey{
    // 主键盘数字 0-9
    { 0x30, Qt::Key_0 },
    { 0x31, Qt::Key_1 },
    { 0x32, Qt::Key_2 },
    { 0x33, Qt::Key_3 },
    { 0x34, Qt::Key_4 },
    { 0x35, Qt::Key_5 },
    { 0x36, Qt::Key_6 },
    { 0x37, Qt::Key_7 },
    { 0x38, Qt::Key_8 },
    { 0x39, Qt::Key_9 },

    // 字母 A-Z
    { 0x41, Qt::Key_A },
    { 0x42, Qt::Key_B },
    { 0x43, Qt::Key_C },
    { 0x44, Qt::Key_D },
    { 0x45, Qt::Key_E },
    { 0x46, Qt::Key_F },
    { 0x47, Qt::Key_G },
    { 0x48, Qt::Key_H },
    { 0x49, Qt::Key_I },
    { 0x4A, Qt::Key_J },
    { 0x4B, Qt::Key_K },
    { 0x4C, Qt::Key_L },
    { 0x4D, Qt::Key_M },
    { 0x4E, Qt::Key_N },
    { 0x4F, Qt::Key_O },
    { 0x50, Qt::Key_P },
    { 0x51, Qt::Key_Q },
    { 0x52, Qt::Key_R },
    { 0x53, Qt::Key_S },
    { 0x54, Qt::Key_T },
    { 0x55, Qt::Key_U },
    { 0x56, Qt::Key_V },
    { 0x57, Qt::Key_W },
    { 0x58, Qt::Key_X },
    { 0x59, Qt::Key_Y },
    { 0x5A, Qt::Key_Z }
};

// 合并查询函数（优先查功能键，再查字符键，全局钩子直接用这个）
Qt::Key getQtKeyFromVK(DWORD vkCode);
#endif
