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
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ProcessListModel.h"
#include "GlobalHook.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    ProcessListModel processModel;
	HideProcess hideProcess;
	GlobalHook globalHook(&processModel);

    // 加载 QML
    QQmlApplicationEngine engine;
    // 将模型注册到 QML 上下文
    engine.rootContext()->setContextProperty("processModel", &processModel);
    engine.rootContext()->setContextProperty("hideProcess", &hideProcess);
	engine.rootContext()->setContextProperty("globalHook", &globalHook);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
