#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

/*
主窗口
*/

class MainWindow: public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

    bool initialize();
    
};

#endif