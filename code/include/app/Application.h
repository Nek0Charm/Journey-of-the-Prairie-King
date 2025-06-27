#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <view/mainwindow.h>

/*
主程序
*/

class Application: public QApplication {
public:
    Application(int &argc, char **argv): QApplication(argc, argv){

    }

    int run();

private:
    MainWindow* mw;
};

#endif