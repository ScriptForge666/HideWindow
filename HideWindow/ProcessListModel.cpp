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
#include "ProcessListModel.h"
#include <QDebug>
#include <qfileinfo.h>
#include <Psapi.h>
#include <tlhelp32.h>

// 链接所需的 Windows 库
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Kernel32.lib")

// ===================== Process 类实现 =====================
Process::Process(QObject* parent)
    : QObject(parent)
{
}

Process::Process(HANDLE handle, QObject* parent)
    : QObject(parent)
    , m_handle(handle)
{
}

Process::~Process() {
    // 关闭进程句柄，避免资源泄漏
    if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

qint64 Process::getPID() const {
    if (m_handle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    return GetProcessId(m_handle);
}

QString Process::getFile() const {
    if (m_handle == INVALID_HANDLE_VALUE) {
        return QString();
    }

    // 获取进程可执行文件路径
    WCHAR szPath[MAX_PATH] = { 0 };
    if (GetModuleFileNameExW(m_handle, nullptr, szPath, MAX_PATH)) {
        return QString::fromWCharArray(szPath);
    }
    else {
        qWarning() << "Failed to get process file path. Error:" << GetLastError();
        return QString();
    }
}

QString Process::getName() const {
    QString filePath = getFile();
    if (filePath.isEmpty()) {
        return QString("Unknown Process (PID: %1)").arg(getPID());
    }
    // 从路径中提取文件名
    return QFileInfo(filePath).fileName();
}

HANDLE Process::getHandle() const {
    return m_handle;
}

qint64 Process::getHandleAsInt() const {
    return reinterpret_cast<qint64>(m_handle);
}

// ===================== ProcessListModel 类实现 =====================
ProcessListModel::ProcessListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

ProcessListModel::~ProcessListModel() {
    // 清理所有 Process 对象
    clearProcesses();
}

int ProcessListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_processes.count();
}

QVariant ProcessListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_processes.count()) {
        return QVariant();
    }

    Process* process = m_processes.at(index.row());
    switch (role) {
    case NameRole:
        return process->getName();
    case PidRole:
        return process->getPID();
    case FileRole:
        return process->getFile();
    case HandleRole:
        return process->getHandleAsInt();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ProcessListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PidRole] = "pid";
    roles[FileRole] = "file";
    return roles;
}

void ProcessListModel::addProcess(Process* process) {
    if (!process) {
        return;
    }

    // Qt 模型添加数据的标准流程：beginInsertRows -> 添加数据 -> endInsertRows
    beginInsertRows(QModelIndex(), m_processes.count(), m_processes.count());
    m_processes.append(process);
    endInsertRows();
}

void ProcessListModel::clearProcesses() {
    if (m_processes.isEmpty()) {
        return;
    }

    // Qt 模型删除数据的标准流程
    beginRemoveRows(QModelIndex(), 0, m_processes.count() - 1);
    // 释放所有 Process 对象内存
    qDeleteAll(m_processes);
    m_processes.clear();
    endRemoveRows();
}

void ProcessListModel::enumerateWindowsProcesses() {
    // 先清空现有进程列表
    clearProcesses();

    // 创建进程快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        qWarning() << "Failed to create process snapshot. Error:" << GetLastError();
        return;
    }

    PROCESSENTRY32W pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    // 遍历第一个进程
    if (!Process32FirstW(hSnapshot, &pe32)) {
        qWarning() << "Failed to get first process. Error:" << GetLastError();
        CloseHandle(hSnapshot);
        return;
    }

    do {
        // 打开进程（获取查询权限）
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE,
            pe32.th32ProcessID
        );
        
        if(hProcess == 0)
            {
            qWarning() << "OpenProcess failed for PID:" << pe32.th32ProcessID
                << ". Error:" << GetLastError();
            continue; // 跳过无法打开的进程
		}

        if (hProcess != INVALID_HANDLE_VALUE) {
            // 创建 Process 对象并添加到模型
            Process* process = new Process(hProcess, this);
            addProcess(process);
        }
        else {
            // 有些系统进程无法打开，属于正常情况，仅打印警告
            qWarning() << "Cannot open process (PID:" << pe32.th32ProcessID
                << "). Error:" << GetLastError();
        }

    } while (Process32NextW(hSnapshot, &pe32));

    // 关闭快照句柄
    CloseHandle(hSnapshot);
}

// ===================== HideProcess 类实现 =====================
HideProcess::HideProcess(QObject* parent)
    : QObject(parent)
{
}

// 辅助函数：递归查找并隐藏指定PID的所有窗口
void HideProcess::findWindows(HWND parentHwnd, HWND startHwnd, DWORD targetPid, std::function<void(HWND, DWORD)> run) {
    // 查找父窗口下的子窗口（从 startHwnd 开始）
    HWND hwnd = FindWindowEx(parentHwnd, startHwnd, nullptr, nullptr);
    if (hwnd == nullptr) {
        return; // 没有更多子窗口，终止递归
    }

    run(hwnd, targetPid);

    // 1. 递归遍历当前窗口的子窗口（深度优先）
    //findWindows(hwnd, nullptr, targetPid, run);
    // 2. 继续遍历同级窗口（广度优先）
    findWindows(parentHwnd, hwnd, targetPid, run);
}

void HideProcess::hideProcess(Process* process) {
    if (!process) {
        qWarning() << "Process pointer is null!";
        return;
    }

    qint64 pid = process->getPID();
    hideProcess(pid);
}

void HideProcess::hideProcess(qint64 pid) {
    if (pid <= 0) {
        qWarning() << "Invalid PID:" << pid;
        return;
    }

    DWORD targetPid = static_cast<DWORD>(pid);
    // 先校验进程是否存在（可选，增加鲁棒性）
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid);
    if (hProcess == INVALID_HANDLE_VALUE) {
        qWarning() << "Cannot open process (PID:" << pid << "), need admin rights?";
        return;
    }
    CloseHandle(hProcess);

    qDebug() << "Trying to hide process (PID:" << pid << ")";

    // 从桌面窗口开始遍历，初始 startHwnd 为 nullptr
    findWindows(HWND_DESKTOP, nullptr, targetPid, &hideWindow);
}

void HideProcess::showProcess(qint64 pid) {
    if (pid <= 0) {
        qWarning() << "Invalid PID:" << pid;
        return;
    }

    DWORD targetPid = static_cast<DWORD>(pid);
    // 先校验进程是否存在（可选，增加鲁棒性）
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid);
    if (hProcess == INVALID_HANDLE_VALUE) {
        qWarning() << "Cannot open process (PID:" << pid << "), need admin rights?";
        return;
    }
    CloseHandle(hProcess);

    qDebug() << "Trying to hide process (PID:" << pid << ")";

    // 从桌面窗口开始遍历，初始 startHwnd 为 nullptr
    findWindows(HWND_DESKTOP, nullptr, targetPid, &showWindow);
}



void hideWindow(HWND hwnd, DWORD targetPid) {
    // 获取窗口所属进程ID
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    // 如果是目标进程的窗口且可见，隐藏它
    if (pid == targetPid && IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
        qDebug() << "Hidden window:" << hwnd << "Title:" << GetWindowTextW(hwnd, new WCHAR[256], 256)
            << "for PID:" << targetPid;
    }
}

void showWindow(HWND hwnd, DWORD targetPid) {
    // 获取窗口所属进程ID
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    // 如果是目标进程的窗口且可见，隐藏它
    if (pid == targetPid && !IsWindowVisible(hwnd) && IsWindow(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
        qDebug() << "Hidden window:" << hwnd << "Title:" << GetWindowTextW(hwnd, new WCHAR[256], 256)
            << "for PID:" << targetPid;
    }
}