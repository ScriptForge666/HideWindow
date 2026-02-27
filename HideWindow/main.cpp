#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ProcessListModel.h"


int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    ProcessListModel processModel;
	HideProcess hideProcess;

    // 加载 QML
    QQmlApplicationEngine engine;
    // 将模型注册到 QML 上下文
    engine.rootContext()->setContextProperty("processModel", &processModel);
    engine.rootContext()->setContextProperty("hideProcess", &hideProcess);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}