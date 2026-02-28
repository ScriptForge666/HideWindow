#include <QApplication>
 // 包含你的 GlobalHook 实现
#include "main.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
