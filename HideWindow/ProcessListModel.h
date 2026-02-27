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
#ifndef PROCESSLISTMODEL_H
#define PROCESSLISTMODEL_H
#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <QList>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

class Process : public QObject {
    Q_OBJECT
public:
    explicit Process(QObject* parent = nullptr);
    explicit Process(HANDLE handle, QObject* parent = nullptr);
    ~Process();
public slots:
    // 1. 替换 std::size_t 为 qint64（Qt 元对象支持的整数类型）
    qint64 getPID() const;
    // 2. 替换 fs::path 为 QString（对外暴露 Qt 类型，内部仍可用 fs::path）
    QString getFile() const;
    QString getName() const;
    HANDLE getHandle() const;
    qint64 getHandleAsInt() const;

private:
    HANDLE m_handle = INVALID_HANDLE_VALUE; // 初始化避免野指针
};

class ProcessListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ProcessRoles {
        NameRole = Qt::DisplayRole,
        PidRole = Qt::UserRole + 1,
        FileRole = Qt::UserRole + 2,
        HandleRole = Qt::UserRole + 3
    };
    Q_ENUM(ProcessRoles) // 确保枚举被 MOC 处理

    explicit ProcessListModel(QObject* parent = nullptr);
    ~ProcessListModel() override;
public slots:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void addProcess(Process* process);
    void clearProcesses();
    void enumerateWindowsProcesses();
private:
    QList<Process*> m_processes;
};


// 先修正HideProcess类的定义（原定义有QObject拷贝问题）
class HideProcess : public QObject {
    Q_OBJECT
public:
    // 2. 改为接收Process指针（避免QObject拷贝）
    explicit HideProcess(QObject* parent = nullptr);
    void hideProcess(Process* process);
public slots:
    void hideProcess(qint64 pid = 0);
    void showProcess(qint64 pid = 0);
private:

    // 辅助函数：递归查找所有窗口（包括子窗口）
    void findWindows(HWND parentHwnd, HWND startHwnd, DWORD targetPid, std::function<void(HWND, DWORD)> run);
};

void hideWindow(HWND hwnd, DWORD targetPid);
void showWindow(HWND hwnd, DWORD targetPid);
#endif