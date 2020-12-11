#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QString qss;
        QFile qssFile("E:\\October\\multiMediaPlayer\\myStyle.qss");
        qssFile.open(QFile::ReadOnly);

        if(qssFile.isOpen())
        {
            qss = QLatin1String(qssFile.readAll());
            qApp->setStyleSheet(qss);
            qssFile.close();
        }

    MainWindow w;
    w.setWindowTitle(u8"媒体播放器");
    w.show();
    return a.exec();
}
